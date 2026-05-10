#include <iostream>
#include <vector>
#include <atomic>
#include <optional>
#include <thread>
#include <chrono>
#include <iomanip>

struct AdasRadarData {
    uint64_t timestamp_ms;
    uint8_t  sensor_id;         // e.g., 0x01 for Front Center Radar
    float    target_distance_m; // Distance to the car ahead (meters)
    float    relative_speed_mps;// Relative speed (meters per second)
    bool     collision_warning; // ADAS flag
};

// --- Global State ---
std::atomic<bool> is_running{true};
CircularBuffer<AdasRadarData, 10> data_buffer; // buffer is 10

template <typename T, size_t Size>
class CircularBuffer {
public:
    CircularBuffer() : buffer(Size), head(0), tail(0) {}

    bool push(const T& item) {
        size_t current_head = head.load(std::memory_order_relaxed);
        size_t next_head = (current_head + 1) % Size;

        if (next_head == tail.load(std::memory_order_acquire)) {
            return false; // Buffer Full (Overrun)
        }

        buffer[current_head] = item;
        head.store(next_head, std::memory_order_release);
        return true;
    }

    std::optional<T> pop() {
        size_t current_tail = tail.load(std::memory_order_relaxed);
        if (current_tail == head.load(std::memory_order_acquire)) {
            return std::nullopt; // Buffer Empty
        }

        T item = buffer[current_tail];
        tail.store((current_tail + 1) % Size, std::memory_order_release);
        return item;
    }

private:
    std::vector<T> buffer; 
    std::atomic<size_t> head;
    std::atomic<size_t> tail;
};

void acquisition_task() {
    std::cout << "[RADAR HW] Initializing Front Radar sensor..." << std::endl;
    uint64_t time = 0;
    
    // Starting conditions for our simulation
    float current_distance = 50.0f;     // Start 50 meters away
    float closing_speed = 25.0f;        // We are catching up at 25 m/s (90 km/h)

    while (is_running) {
        // Calculate dynamic ADAS data
        current_distance -= (closing_speed * 0.01f); // Update distance based on 10ms loop
        bool warning = (current_distance < 15.0f);   // Warn if closer than 15 meters

        AdasRadarData data = { 
            time, 
            0x01, // Front Center Radar ID
            current_distance, 
            closing_speed, 
            warning 
        };
        
        if (!data_buffer.push(data)) {
            std::cerr << "[RADAR HW] ERROR: Frame Dropped! CAN bus / Buffer full." << std::endl;
        }
        
        time += 10;
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // 100Hz Radar refresh rate
    }
}

void storage_task() {
    std::cout << "[LOGGER] SSD Write sequence started." << std::endl;
    
    while (is_running) {
        auto data = data_buffer.pop();
        
        if (data.has_value()) {
            std::cout << "[LOGGER] T+" << std::setw(4) << data->timestamp_ms << "ms "
                      << "| ID: 0x0" << (int)data->sensor_id 
                      << " | Dist: " << std::fixed << std::setprecision(2) << std::setw(6) << data->target_distance_m << "m "
                      << "| Rel.Spd: " << data->relative_speed_mps << "m/s "
                      << "| Warn: " << (data->collision_warning ? "YES (AEB Triggered!)" : "NO") 
                      << std::endl;
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(2)); 
        }
    }
}

int main() {
    std::cout << "--- Starting Automotive Data Acquisition Demo ---" << std::endl;
    
    // Start threads
    std::thread hw_thread(acquisition_task);
    std::thread sw_thread(storage_task);
    
    // Let it run for 3 seconds to gather data
    std::this_thread::sleep_for(std::chrono::seconds(3));
    
    // Shut down cleanly
    std::cout << "\n--- Shutting down gracefully ---" << std::endl;
    is_running = false;
    
    hw_thread.join();
    sw_thread.join();
    
    std::cout << "Demo complete." << std::endl;
    return 0;
}