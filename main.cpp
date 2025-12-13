#include <iostream>
#include <vector>
#include <queue>
#include <cstdlib>
#include <ctime>
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
    string waitingOn;  // NEW: "S1", "S2", "S3", or "-"

    Thread(int id, int burst, int prio)
        : id(id), burstTime(burst), cpuUsed(0),
          priority(prio), state(READY), waitingOn("-") {}
};

vector<Thread> threadList;

// ====================== SEMAPHORE CLASS ========================
class Semaphore {
public:
    string name;
    int value;
    queue<int> waitQueue;

    Semaphore(string name, int v) : name(name), value(v) {}

    void waitOp(int tid) {
        value--;
        if (value < 0) {
            cout << "  -> Thread " << tid << " BLOCKED on " << name << "\n";
            waitQueue.push(tid);
            threadList[tid].state = BLOCKED;
            threadList[tid].waitingOn = name;
	    waitQueue.push(tid);
        } else {
            cout << "  -> Thread " << tid << " acquired " << name << "\n";
        }
    }

    void signalOp() {
        value++;
        if (value <= 0 && !waitQueue.empty()) {
            int tid = waitQueue.front();
            waitQueue.pop();
	    cout << " -> SIGNAL " << name << ": unblocked Thread " << tid << "\n";
            threadList[tid].state = READY;
            threadList[tid].waitingOn = "-";
        } else {
            cout << "  -> SIGNAL " << name << ": no waiting threads\n";
        }
    }
};

// Create 3 semaphores
Semaphore S1("S1", 1);
Semaphore S2("S2", 1);
Semaphore S3("S3", 1);

// ====================== MONITOR CLASS ==========================
class Monitor {
public:
    Semaphore mutex{"MUTEX", 1};
    queue<int> conditionQueue;

    void enter(int tid) {
        cout << "Thread " << tid << " ENTER monitor\n";
        mutex.waitOp(tid);
    }

    void exitMonitor(int tid) {
        cout << "Thread " << tid << " EXIT monitor\n";
        mutex.signalOp();
    }

    void waitCV(int tid) {
        cout << "Thread " << tid << " WAIT on MonitorCV\n";

        // Thread releases mutex and becomes BLOCKED
        conditionQueue.push(tid);
        threadList[tid].state = BLOCKED;
        threadList[tid].waitingOn = "MONITOR";

        mutex.signalOp();  // release monitor lock
    }

    void signalCV(int tid) {
        cout << "Thread " << tid << " SIGNAL MonitorCV\n";

        if (!conditionQueue.empty()) {
            int wakingThread = conditionQueue.front();
            conditionQueue.pop();

            cout << "  -> MonitorCV wakes Thread " << wakingThread << "\n";

            threadList[wakingThread].state = READY;
            threadList[wakingThread].waitingOn = "-";
        }
    }
};

Monitor monitorObj;

// ====================== VISUALIZATION ==========================
string stateStr(ThreadState s) {
    switch (s) {
        case READY: return "READY";
        case RUNNING: return "RUNNING";
        case BLOCKED: return "BLOCKED";
        case TERMINATED: return "TERMINATED";
    }
    return "UNKNOWN";
}

void printTable() {
    cout << "\n------------------------------------------------------------\n";
    cout << "| TID |   STATE    | CPU USED | BURST |   WAITING ON        |\n";
    cout << "-------------------------------------------------------------\n";

    for (auto &t : threadList) {
        string st = stateStr(t.state);
        int pad = 11 - st.length();

        cout << "|  " << t.id << "   | " << st;
        while (pad--) cout << " ";
        cout << "|     " << t.cpuUsed
             << "     |   " << t.burstTime
             << "   |     " << t.waitingOn << "           |\n";
    }

    cout << "-------------------------------------------------------------\n";
}


// ====================== ROUND ROBIN SCHEDULER ==================
void runRoundRobin(int quantum) {
    cout << "\n===== Starting Round Robin Scheduler (Monitor Demo) =====\n";

    bool allDone = false;

    while (!allDone) {
        allDone = true;

        for (auto &t : threadList) {

            if (t.state == TERMINATED || t.state == BLOCKED)
                continue;

            allDone = false;

            t.state = RUNNING;
            cout << "\n[RUNNING] Thread " << t.id << "\n";

            for (int q = 0; q < quantum; q++) {

                t.cpuUsed++;
                cout << "  Tick: CPU used = " << t.cpuUsed
                     << "/" << t.burstTime << "\n";

                // ================= MONITOR DEMO RULES =================

                if (t.id == 0 && t.cpuUsed == 2) {
                    monitorObj.enter(0);
                }

                if (t.id == 0 && t.cpuUsed == 3) {
                    monitorObj.waitCV(0);
                    break;
                }

                if (t.id == 1 && t.cpuUsed == 2) {
                    monitorObj.enter(1);
                }

                if (t.id == 1 && t.cpuUsed == 3) {
                    monitorObj.signalCV(1);
                }

                if (t.id == 1 && t.cpuUsed == 4) {
                    monitorObj.exitMonitor(1);
                }

                // After waking, Thread 0 will re-enter monitor
                if (t.id == 0 && t.cpuUsed == 4) {
                    cout << "Thread 0 re-enters monitor after waking\n";
                    monitorObj.enter(0);
                }

                // =======================================================

                if (t.cpuUsed >= t.burstTime) {
                    t.state = TERMINATED;
                    cout << "  -> Thread " << t.id << " TERMINATED\n";
                    break;
                }
            }

            if (t.state == RUNNING)
                t.state = READY;

            printTable();
        }
    }

    cout << "\n===== All Threads Completed =====\n";
}

// ====================== THREAD CREATION ========================
void createThread(int burst, int prio) {
    int id = threadList.size();
    threadList.emplace_back(id, burst, prio);
    cout << "Thread " << id << " created\n";
}

// ================================ MAIN ==========================
int main() {
    srand(time(0));

    createThread(10, 1);  // Thread 0 (wait S1)
    createThread(10, 1);   // Thread 1 (wait S2)
    createThread(8, 2);   // Thread 2 (signal S1)
    createThread(7, 3);   // Thread 3 (signal S2)

    cout << "\nInitial Thread States:\n";
    printTable();

    runRoundRobin(2);

    return 0;
}
