#include <iostream>

struct AdasRadarData {
    uint64_t timestamp_ms;
    uint8_t  sensor_id;         // e.g., 0x01 for Front Center Radar
    float    target_distance_m; // Distance to the car ahead (meters)
    float    relative_speed_mps;// Relative speed (meters per second)
    bool     collision_warning; // ADAS flag
};

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


int main() {
    std::cout << "--- Starting Automotive Data Acquisition Demo ---" << std::endl;

    // todo...
    
    std::cout << "Demo complete." << std::endl;
    return 0;
}