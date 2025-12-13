# 🧵 OS Concurrency Simulator  
A complete multi-threaded operating system simulation that demonstrates **CPU scheduling**, **semaphores**, **thread synchronization**, and **monitors** with condition variables.  
Built entirely in **C++**, this project visually shows how threads change state over time inside an OS-like environment.

---

## 🚀 Features

### ✔ Round Robin CPU Scheduler  
- Time-sliced execution  
- Tracks thread states (READY, RUNNING, BLOCKED, TERMINATED)  
- Real-time CPU tick visualization  

### ✔ Semaphores (S1, S2, S3)  
- Classic WAIT (P) and SIGNAL (V) operations  
- Thread blocking and unblocking  
- Queue of waiting threads  

### ✔ Controlled Random Behavior  
- Threads randomly perform semaphore WAIT/SIGNAL  
- Helps simulate unpredictable multi-thread interactions  
- Keeps output readable while still dynamic  

### ✔ **Monitors with Condition Variables (Final Version)**  
This project includes a full monitor implementation:

- `enter()` — acquires monitor mutex  
- `exit()` — releases mutex  
- `wait()` — releases mutex & blocks thread  
- `signal()` — wakes exactly one blocked thread  

This demonstrates textbook monitor semantics exactly as taught in OS curriculum.

### ✔ Visual Thread State Table  
After every scheduling quantum, the following table updates:


