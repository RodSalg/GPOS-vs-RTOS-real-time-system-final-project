# Real-Time Control System Comparison: RT vs GPOS

## Authors
- Thiago Rodrigo Monteiro Salgado - Graduate Student, PPGEE/UFAM
- Aris Canto - Graduate Student, PPGEE/UFAM

Advisor: Prof. Dr. Lucas Cordeiro  
Institution: Federal University of Amazonas (UFAM)  
Program: Graduate Program in Electrical Engineering (PPGEE)

---

## About This Project

This repository contains the implementation and experimental results of a comparative study between Real-Time Operating Systems (RTOS) and General-Purpose Operating Systems (GPOS) for distributed control applications. 

The main goal is to demonstrate quantitatively how temporal determinism impacts the stability and precision of cyber-physical systems, specifically through a mobile robot control simulation.

### What We Built

We developed a multi-threaded distributed control system in C that simulates a non-holonomic mobile robot following a sinusoidal reference trajectory. The system consists of 7 concurrent threads operating at different frequencies (30ms to 120ms), communicating through shared memory protected by mutexes.

Two versions were implemented:

1. RT Version (TP_rt): Uses Linux PREEMPT_RT kernel with:
   - SCHED_FIFO scheduling policy
   - Rate-Monotonic priority assignment
   - Priority inheritance protocol (PIP) for mutex
   - Memory locking (mlockall)
   - High-precision timers (clock_nanosleep)

2. GPOS Version (TP_gpos): Uses standard Linux kernel with:
   - SCHED_OTHER (CFS) scheduling
   - Default dynamic priorities
   - Standard POSIX mutexes
   - No real-time configurations

### Why This Matters

Real-time systems are critical for applications like:
- Industrial automation
- Autonomous vehicles
- Medical devices
- Aerospace control systems

Understanding when you need an RTOS vs when GPOS is sufficient can save significant development costs and complexity.

---

## Repository Structure

.
├── src/
│   ├── main.c              # Main program entry point
│   ├── robot.c             # Robot kinematics simulation (30ms thread)
│   ├── control.c           # Control and linearization threads (40-50ms)
│   ├── reference.c         # Reference generation and model (50-120ms)
│   ├── shared.c            # Shared data structure and mutex
│   └── utils.c             # Logging and UI thread (100ms)
├── include/                # Header files
├── scripts/
│   ├── analyze_times.py    # Statistical analysis of execution times
│   └── generate_plots.py   # Visualization generation
├── data/                   # Output directory for logs and CSV
└── README.md

---

## System Requirements

### Hardware
- x86_64 CPU (tested on Intel Core i3-10110U)
- Minimum 4GB RAM (16GB recommended)

### Software
- OS: Ubuntu 24.04 LTS
- Compiler: GCC 11.4+
- Python: 3.10+ (for analysis scripts)
- Real-time kernel: Linux 6.8.0-rt (for RT version)

---

## Installation Guide

### Step 1: Install Ubuntu Pro (for RT kernel)

First, register for Ubuntu Pro (free for personal use):

# Check if ubuntu-advantage-tools is installed
apt show ubuntu-advantage-tools

# If not, install it
sudo apt update && sudo apt install ubuntu-advantage-tools

Go to ubuntu.com/pro and create a free account to get your token.

### Step 2: Enable Real-Time Kernel

# Attach your Ubuntu Pro token
sudo pro attach <YOUR_TOKEN>

# Enable real-time kernel
sudo pro enable realtime-kernel

# Reboot to load the new kernel
sudo reboot

### Step 3: Verify RT Kernel Installation

# Check kernel version (should show "PREEMPT_RT")
uname -a

# Expected output contains: "realtime" and "PREEMPT_RT"

### Step 4: Install Dependencies

# Install build tools
sudo apt install build-essential

# Install Python dependencies
sudo apt install python3-pip python3-venv
python3 -m venv venv
source venv/bin/activate
pip install numpy pandas matplotlib

---

## How to Build and Run

### Compile the Programs

# For RT version
cd src
gcc -std=gnu17 -pthread -lm -lrt main.c robot.c control.c reference.c shared.c utils.c -o TP_rt

# For GPOS version (if you want to compare)
gcc -std=gnu17 -pthread -lm main.c robot.c control.c reference.c shared.c utils.c -o TP_gpos

### Run the RT Version

# Important: RT version needs root privileges
sudo ./TP_rt

What happens when you run:
- The program starts 7 threads simulating the robot control system
- Every 100ms, terminal shows current robot position and reference
- You can change control gains by typing: 3.5 4.0 and pressing Enter
- Runs for 20 seconds then stops automatically
- Saves two files:
  - data/log.txt - Robot trajectory data
  - data/times.csv - Thread execution time statistics

### Run the GPOS Version (Optional)

# GPOS version doesn't need sudo
./TP_gpos

---

## Understanding the Code

### Thread Architecture

The system uses 7 threads with different periods (Rate-Monotonic scheduling):

Thread ID | Name                | Period | Priority (RT) | Function
----------|---------------------|--------|---------------|---------------------------
0         | Robot Simulator     | 30ms   | 98 (highest)  | Simulates kinematics
1         | Linearization       | 40ms   | 97            | Converts virtual to real control
2         | Control             | 50ms   | 96            | Calculates control signal
3         | Reference Generator | 120ms  | 94            | Generates trajectory
4         | Reference Model X   | 50ms   | 96            | Smooths X reference
5         | Reference Model Y   | 50ms   | 96            | Smooths Y reference
6         | Logger/UI           | 100ms  | 95            | Prints status and saves data

### Key Files Explained

main.c
- Initializes shared data structure
- Creates all 7 threads
- Waits 20 seconds
- Stops simulation and saves results

robot.c
- Implements robot kinematics equations
- Updates position (x, y, θ) based on velocity commands
- Runs every 30ms (highest priority)

control.c
- thread_control: Computes virtual control velocities
- thread_linearization: Transforms to real robot velocities (v, ω)

reference.c
- Generates sinusoidal reference trajectory
- Implements reference model filtering (first-order system)

shared.c
- Defines SharedData structure containing all state variables
- Implements mutex-protected getter/setter functions
- Uses PTHREAD_PRIO_INHERIT in RT version to prevent priority inversion

utils.c
- Logger thread that prints status every 100ms
- Allows runtime parameter changes
- Saves trajectory and timing data to CSV

---

## Running Tests and Generating Results

### Step 1: Run with Different Stress Levels

To compare performance under load, use stress tool:

# Install stress if needed
sudo apt install stress

# Run without stress (baseline)
sudo ./TP_rt

# Run with 8 CPU workers
stress --cpu 8 --timeout 180s &
sudo ./TP_rt

# Run with 16 CPU workers
stress --cpu 16 --timeout 180s &
sudo ./TP_rt

# Run with 32 CPU workers
stress --cpu 32 --timeout 180s &
sudo ./TP_rt

# Run with 64 CPU workers
stress --cpu 64 --timeout 180s &
sudo ./TP_rt

### Step 2: Analyze Results

# Activate Python virtual environment
source venv/bin/activate

# Run statistical analysis
python scripts/analyze_times.py

# Generate comparison plots
python3 scripts/generate_plots.py

### Step 3: Check CPU Usage During Tests

# Monitor CPU usage in another terminal
top

# Or for real-time monitoring
htop  # install with: sudo apt install htop

---

## Testing Kernel Latency with Cyclictest

For more detailed latency analysis:

# Install rt-tests package
sudo apt install rt-tests

# Run cyclictest (measures scheduler latency)
sudo cyclictest -t1 -p80 -i1000 -l100000 -h 100 --histfile=/tmp/histograma.txt

# Generate histogram plot with gnuplot
gnuplot -persist -e "set title 'Latency Distribution'; \
set xlabel 'Latency (us)'; \
set ylabel 'Occurrences'; \
set grid; \
plot '/tmp/histograma.txt' using 1:2 with lines title 'Thread 0'"

---

## Expected Results

### RT System (PREEMPT_RT)
- Jitter: ~1.5 μs (under 64 CPU stress)
- Behavior: Consistent and predictable
- Deadline misses: 0
- Suitable for: Hard real-time applications

### GPOS System (Standard Linux)
- Jitter: ~4.0 μs (under 64 CPU stress)
- Behavior: Variable with occasional outliers (up to 16.5 μs)
- Deadline misses: 0 (but with less margin)
- Suitable for: Soft real-time applications with low CPU utilization

---

## Switching Between RT and GPOS Kernels

### Disable RT Kernel (back to GPOS)

sudo pro disable realtime-kernel
sudo reboot

# Verify you're on standard kernel
uname -a  # Should NOT show "PREEMPT_RT"

### Re-enable RT Kernel

sudo pro enable realtime-kernel
sudo reboot

---

## Common Problems and Solutions

### Problem: "Permission denied" when running TP_rt
Solution: RT version needs root. Use sudo ./TP_rt

### Problem: Kernel doesn't show PREEMPT_RT after installation
Solution: 
1. Check if installation was successful: sudo pro status
2. Make sure you selected RT kernel in GRUB menu during boot
3. Try: sudo update-grub then reboot

### Problem: High jitter even on RT kernel
Solution: 
1. Make sure you're actually running the RT version (sudo ./TP_rt)
2. Disable CPU frequency scaling: sudo cpupower frequency-set -g performance
3. Close unnecessary applications

### Problem: Python scripts not working
Solution:

# Make sure virtual environment is activated
source venv/bin/activate

# Install missing packages
pip install numpy pandas matplotlib

---

## Understanding the Results

After running experiments, you'll have:

1. data/log.txt - Contains robot trajectory:
   - Columns: x, y, θ, output_x, output_y, time, ref_x, ref_y

2. data/times.csv - Contains execution times for each thread:
   - Columns: thread_id, time_ms
   - Use this to calculate jitter, standard deviation, etc.

3. Analysis outputs (from Python scripts):
   - Statistical summary tables
   - Jitter comparison graphs
   - Timeline plots showing temporal consistency

---

## Citation

If you use this code or methodology in your research, please cite:

Salgado, T.R.M., Canto, A. (2024). 
Comparative Performance Analysis of Distributed Control Systems in RTOS and GPOS.
Graduate Program in Electrical Engineering, Federal University of Amazonas.
Advisor: Prof. Dr. Lucas Cordeiro.

---

## License

This project was developed for academic purposes as part of graduate research at UFAM.

---

## Contact

- Thiago Salgado: thiago.salgado@icomp.ufam.edu.br
- Aris Canto: ariscanto21@gmail.com

Advisor: Prof. Dr. Lucas Cordeiro  
Institution: PPGEE - UFAM

---

## Acknowledgments

This research was conducted as part of the Graduate Program in Electrical Engineering at the Federal University of Amazonas (PPGEE/UFAM) under the supervision of Prof. Dr. Lucas Cordeiro.

We thank Ubuntu Pro for providing free access to the real-time kernel for academic research.

---

Last updated: December 2024