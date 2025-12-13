#include <iostream>
#include <vector>
#include <queue>
using namespace std;

// ====================== THREAD STATES ==========================
enum ThreadState {
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
};

// ====================== THREAD STRUCT ==========================
struct Thread {
    int id;
    int burstTime;
    int cpuUsed;
    int priority;
    ThreadState state;

    Thread(int id, int burst, int prio)
        : id(id), burstTime(burst), cpuUsed(0),
          priority(prio), state(READY) {}
};

// ====================== THREAD LIST ============================
vector<Thread> threadList;

// ====================== SEMAPHORE CLASS ========================
class Semaphore {
public:
    int value;
    queue<int> waitingQueue;

    Semaphore(int v) : value(v) {}

    void wait(int threadId) {
        value--;
        if (value < 0) {
            cout << "  -> Thread " << threadId << " BLOCKED on semaphore\n";
            waitingQueue.push(threadId);
            threadList[threadId].state = BLOCKED;
        } else {
            cout << "  -> Thread " << threadId << " acquired semaphore\n";
        }
    }

    void signal() {
        value++;
        if (value <= 0 && !waitingQueue.empty()) {
            int unblockedId = waitingQueue.front();
            waitingQueue.pop();
            threadList[unblockedId].state = READY;
            cout << "  -> Semaphore SIGNAL: Thread "
                 << unblockedId << " UNBLOCKED\n";
        } else {
            cout << "  -> Semaphore SIGNAL (no waiting threads)\n";
        }
    }
};

// Create a global semaphore
Semaphore S1(0); // forced blocking version

string stateToStr(ThreadState state) {
    switch (state) {
        case READY: return "READY";
        case RUNNING: return "RUNNING";
        case BLOCKED: return "BLOCKED";
        case TERMINATED: return "TERMINATED";
    }
    return "UNKNOWN";
}

void printThreadTable() {
    cout << "\n-------------------------------------------------------------\n";
    cout << "| TID |   STATE    | CPU USED | BURST |   WAITING ON        |\n";
    cout << "-------------------------------------------------------------\n";

    for (auto &t : threadList) {
        string waiting = "-";

        // If blocked, we show semaphore name
        if (t.state == BLOCKED) waiting = "S1";

        cout << "|  " << t.id << "   | "
             << stateToStr(t.state);

        // Adjust spacing for alignment
        int padding = 11 - stateToStr(t.state).length();
        while (padding--) cout << " ";

        cout << "|     " << t.cpuUsed
             << "     |   " << t.burstTime
             << "   |     " << waiting
             << "           |\n";
    }

    cout << "-------------------------------------------------------------\n";
}


// ====================== ROUND ROBIN SCHEDULER ==================
void runRoundRobin(int timeQuantum) {
    cout << "\n===== Starting Round Robin Scheduler (with blocking) =====\n";

    bool allDone = false;

    while (!allDone) {
        allDone = true;

        for (auto &t : threadList) {
            // Skip threads not runnable
            if (t.state == TERMINATED || t.state == BLOCKED)
                continue;

            allDone = false;

            // Make thread RUNNING
            t.state = RUNNING;
            cout << "\n[RUNNING] Thread " << t.id << "\n";

            // Execute for time quantum
            for (int q = 0; q < timeQuantum; q++) {
                if (t.cpuUsed < t.burstTime)
                    t.cpuUsed++;

                cout << "  Tick: CPU used = " << t.cpuUsed
                     << "/" << t.burstTime << "\n";

                // ---------------- SEMAPHORE DEMO BEHAVIOR ----------------
                if (t.id == 0 && t.cpuUsed == 3) {
                    cout << "Thread 0 trying WAIT on S1...\n";
                    S1.wait(0);
                    if (t.state == BLOCKED) {
                        cout << "  Thread 0 BLOCKED - stopping quantum\n";
                        break;
                    }
                }

                if (t.id == 1 && t.cpuUsed == 4) {
                    cout << "Thread 1 doing SIGNAL on S1...\n";
                    S1.signal();
                }
                // ----------------------------------------------------------

                // If thread finished
                if (t.cpuUsed == t.burstTime) {
                    t.state = TERMINATED;
                    cout << "  -> Thread " << t.id << " TERMINATED\n";
                    break;
                }
            }


            if (t.state == RUNNING)
                t.state = READY; // send back to ready queue

        }
	printThreadTable();
    }


    cout << "\n===== All Threads Completed =====\n";
}

// ====================== CREATE THREAD FUNCTION =================
void createThread(int burst, int priority) {
    int id = threadList.size();
    threadList.emplace_back(id, burst, priority);
    cout << "Thread " << id << " created (burst="
         << burst << ", priority=" << priority << ")\n";
}

// ================================ MAIN ==========================
int main() {

    // Create sample threads
    createThread(10, 1); // Thread 0
    createThread(6, 2);  // Thread 1
    createThread(8, 1);  // Thread 2

    cout << "\nInitial Thread States:\n";
    for (auto &t : threadList)
        cout << "Thread " << t.id
             << " | READY | Burst=" << t.burstTime
             << " | Priority=" << t.priority << "\n";

    // Run scheduler with time quantum = 2
    runRoundRobin(2);

    return 0;
}

