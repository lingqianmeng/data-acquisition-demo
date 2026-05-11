# 🚗 Automotive ADAS Data Acquisition System

![C++](https://img.shields.io/badge/C++-17-blue.svg)
![OS](https://img.shields.io/badge/OS-Cross--Platform%20%7C%20QNX%20Target-lightgrey)
![Status](https://img.shields.io/badge/Status-Prototype-success)

## 📌 Overview
This repository contains a C++ prototype for an **Advanced Driver Assistance Systems (ADAS)** data acquisition module. It simulates a high-frequency front-facing radar sensor feeding telemetry data to a storage logging thread via a thread-safe, lock-free ring buffer.

This project demonstrates **Zero-Copy** data architecture, deterministic memory management, and concurrent programming techniques essential for Real-Time Operating Systems (RTOS) like **QNX** or **AUTOSAR** environments.

### 🎮 Live Interactive Demo
**[Click here to view the Interactive Lock-Free Ring Buffer Simulator](https://lingqianmeng.github.io/data-acquisition-demo/)**

## 🏗️ Architecture

The system is decoupled into two primary threads to ensure high-frequency hardware interrupts are not blocked by slower I/O operations (like writing to an SSD).

1. **Acquisition Thread (Producer):** Simulates a hardware Radar sensor running at 100Hz. It calculates dynamic distance, relative speed, and triggers Autonomous Emergency Braking (AEB) warnings.
2. **Lock-Free Circular Buffer:** A template-based ring buffer utilizing `std::atomic` operations (Memory Order Acquire/Release) to pass data safely without heavy `std::mutex` locks. Pre-allocated to guarantee deterministic execution.
3. **Storage Thread (Consumer):** Simulates a blackbox logger writing telemetry to an SSD.

## ✨ Key Features

* **Zero-Copy Transfer:** Utilizes atomic pointers to manage buffer indices without duplicating heavy structs in memory.
* **Deterministic Allocation:** No `malloc` or `new` calls during runtime to prevent heap fragmentation and latency spikes.
* **Overrun Protection:** Gracefully handles non-deterministic latency by dropping oldest frames and logging warnings rather than crashing.
* **ADAS Telemetry:** Simulates real-world automotive data (Distance, Speed, Collision Flags).

## 🚀 Getting Started

### Prerequisites
* A C++ compiler supporting **C++17** (GCC, Clang, or MSVC)
* **CMake** (v3.10 or higher)
* *(Optional)* **Bazel** for enterprise build scaling

### Build & Run with CMake

```bash
# 1. Generate the build files
cmake -S . -B build

# 2. Compile the project
cmake --build build

# 3. Run the executable (Linux/Mac)
./build/adas_demo

# 3. Run the executable (Windows)
./build/Debug/adas_demo.exe
