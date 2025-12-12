# Smart Hospital Simulator

A comprehensive C-based hospital management simulator demonstrating advanced operating system concepts including CPU scheduling, inter-process communication, synchronization, and dynamic memory management.

## 📋 Features

### OS Concepts Implemented

- **Process Creation**: Fork and execv for department processes
- **CPU Scheduling**: 
  - FCFS (First-Come-First-Served) for patient queuing
  - Round Robin for message processing
- **Inter-Process Communication**:
  - Message Queues for patient routing
  - Shared Memory for global hospital state
- **Synchronization**:
  - Semaphores for resource control (doctors, machines, pharmacists)
  - Mutexes for shared memory protection
- **Dynamic Memory**: Malloc/free for patient linked lists
- **Time Tracking**: Comprehensive metrics using time.h
- **Logging**: Thread-safe logging system

### Hospital Configuration

#### Departments and Resources

| Department  | Resources | Semaphore Value |
|------------|-----------|-----------------|
| Emergency  | 2 doctors | 2               |
| OPD        | 3 doctors | 3               |
| Radiology  | 1 machine | 1               |
| Pharmacy   | 2 pharmacists | 2           |
| Billing    | 1 cashier | 1               |

#### Patient Routes

- **Route A (Normal OPD)**: OPD → Pharmacy → Billing → Exit
- **Route B (Emergency)**: Emergency → Radiology → Pharmacy → Billing → Exit
- **Route C (Radiology Only)**: Radiology → OPD → Billing → Exit
- **Route D (Pharmacy Only)**: Pharmacy → Billing → Exit

## 🏗️ Project Structure

```
Smart-Hospital-Simulator/
├── include/              # Header files
│   ├── hospital.h        # Main configuration
│   ├── patient.h         # Patient structures
│   ├── department.h      # Department management
│   ├── scheduler.h       # CPU scheduling
│   ├── shared_memory.h   # Shared memory IPC
│   ├── message_queue.h   # Message queue IPC
│   ├── synchronization.h # Mutexes and semaphores
│   ├── logger.h          # Logging system
│   └── metrics.h         # Performance metrics
├── src/                  # Source files
│   ├── main.c            # Main simulator
│   ├── patient.c         # Patient management
│   ├── department.c      # Department processes
│   ├── scheduler.c       # Scheduling algorithms
│   ├── shared_memory.c   # Shared memory operations
│   ├── message_queue.c   # Message queue operations
│   ├── synchronization.c # Sync primitives
│   ├── logger.c          # Logging implementation
│   └── metrics.c         # Metrics tracking
├── bin/                  # Compiled executable
├── obj/                  # Object files
├── Makefile              # Build configuration
├── run_demo.sh           # Demo script
└── README.md             # This file
```

## 🚀 Getting Started

### Prerequisites

- GCC compiler
- Linux operating system
- POSIX threads library (pthread)
- Real-time library (rt)

### Building the Project

```bash
# Clean and build
make clean && make

# Or simply
make
```

### Running the Simulation

```bash
# Run directly
./bin/hospital_simulator

# Or use the demo script
./run_demo.sh
```

### Cleaning Up

```bash
# Clean build artifacts
make clean

# Clean IPC resources
make clean-ipc

# Full cleanup
make distclean
```

## 📊 Output

The simulator provides:

1. **Real-time Console Output**: Shows patient flow through departments
2. **Patient Journey Report**: Detailed table with arrival, discharge, waiting, and treatment times
3. **Global Statistics**: Average times, throughput, and system performance
4. **Hospital State**: Final status of all departments
5. **Log File** (`hospital_simulation.log`): Complete event history

### Sample Output

```
╔════════════════════════════════════════════════════════════════╗
║                    PATIENT JOURNEY REPORT                      ║
╚════════════════════════════════════════════════════════════════╝

┌──────┬──────────┬────────────┬──────────────┬──────────┬────────────┬───────────┐
│  ID  │  Route   │  Arrival   │  Discharge   │ Waiting  │ Treatment  │   Total   │
│      │          │    Time    │     Time     │   (s)    │    (s)     │    (s)    │
├──────┼──────────┼────────────┼──────────────┼──────────┼────────────┼───────────┤
│ 1    │ Route A  │ 16:26:43   │ 16:26:51     │     0.00 │       0.00 │      8.00 │
│ 2    │ Route A  │ 16:26:43   │ 16:26:55     │     0.00 │       0.00 │     12.00 │
│ 3    │ Route A  │ 16:26:43   │ 16:26:59     │     0.00 │       0.00 │     16.00 │
...
```

## 🔧 Technical Details

### Process Architecture

```
Main Process
├── Emergency Process (fork)
├── OPD Process (fork)
├── Radiology Process (fork)
├── Pharmacy Process (fork)
└── Billing Process (fork)
```

Each department runs as an independent process, communicating via message queues.

### Synchronization Flow

1. **Patient Creation**: Dynamic allocation with malloc
2. **Message Dispatch**: Round Robin scheduler sends patients to departments
3. **Resource Acquisition**: Semaphore wait (FCFS enforced)
4. **Treatment**: Simulated with sleep()
5. **Resource Release**: Semaphore post
6. **State Update**: Mutex-protected shared memory writes
7. **Completion Message**: Sent back to scheduler

### IPC Resources

- **Message Queue**: Key `0x2000`, stores patient routing messages
- **Shared Memory**: Key `0x1234`, stores hospital state
- **Named Semaphores**: `/sem_emergency`, `/sem_opd`, etc.

## 📝 Logging

All events are logged to `hospital_simulation.log` with timestamps:

```
[2025-12-12 16:26:43] [INFO] Created Patient 1 with Route Type 0
[2025-12-12 16:26:44] [INFO] Department OPD: Patient 1 arrived
[2025-12-12 16:26:44] [INFO] Department OPD: Treating Patient 1 (waited 0.00s)
[2025-12-12 16:26:46] [INFO] Department OPD: Patient 1 treatment complete (2.00s)
```

## 🎯 Makefile Targets

| Target      | Description                                    |
|-------------|------------------------------------------------|
| `all`       | Build the simulator (default)                  |
| `clean`     | Remove build artifacts                         |
| `clean-ipc` | Remove IPC resources (queues, memory, sems)    |
| `distclean` | Complete cleanup (build + IPC)                 |
| `run`       | Build and run the simulator                    |
| `help`      | Display help message                           |

## 🐛 Troubleshooting

### IPC Resources Not Cleaned

```bash
# View existing IPC resources
ipcs

# Manual cleanup
ipcrm -a

# Remove semaphores
rm -f /dev/shm/sem.sem_*
```

### Compilation Errors

Ensure you have the required libraries:
```bash
# Debian/Ubuntu
sudo apt-get install build-essential

# The project uses:
# -pthread for POSIX threads
# -lrt for real-time extensions
```

## 📚 Learning Outcomes

This project demonstrates:

- ✅ Multi-process programming with `fork()`
- ✅ Inter-process communication (message queues, shared memory)
- ✅ Synchronization primitives (mutexes, semaphores)
- ✅ CPU scheduling algorithms (FCFS, Round Robin)
- ✅ Dynamic memory management
- ✅ Time-based metrics and analytics
- ✅ Modular C programming with header files
- ✅ Make build system
- ✅ Event logging and debugging

## 👥 Author

Created as an OS Lab Semester Project demonstrating comprehensive operating system concepts in C.

## 📄 License

This project is for educational purposes.