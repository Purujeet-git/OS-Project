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

    Semaphore(string name, int val) : name(name), value(val) {}

    void waitOp(int tid) {
        value--;
        if (value < 0) {
            cout << "  -> Thread " << tid << " BLOCKED on " << name << "\n";
            waitQueue.push(tid);
            threadList[tid].state = BLOCKED;
            threadList[tid].waitingOn = name;
        } else {
            cout << "  -> Thread " << tid << " acquired " << name << "\n";
        }
    }

    void signalOp() {
        value++;
        if (value <= 0 && !waitQueue.empty()) {
            int unblocked = waitQueue.front();
            waitQueue.pop();
            threadList[unblocked].state = READY;
            threadList[unblocked].waitingOn = "-";
            cout << "  -> SIGNAL " << name << ": ublocked Thread "
                 << unblocked << "\n";
        } else {
            cout << "  -> SIGNAL " << name << ": no waiting threads\n";
        }
    }
};

// Create 3 semaphores
Semaphore S1("S1", 1);
Semaphore S2("S2", 1);
Semaphore S3("S3", 1);

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

Semaphore* randomSemaphore(){
    int r = rand() % 3;
    if (r == 0) return &S1;
    if (r == 1) return &S2;
    return &S3;
}

// ====================== ROUND ROBIN SCHEDULER ==================
void runRoundRobin(int quantum) {
    cout << "\n===== Starting Round Robin Scheduler (Multiple Semaphores) =====\n";

    bool allDone = false;

    while (!allDone) {
        allDone = true;

        for (auto &t : threadList) {

            // Skip blocked or terminated threads
            if (t.state == TERMINATED || t.state == BLOCKED)
                continue;

            allDone = false;
            t.state = RUNNING;
            cout << "\n[RUNNING] Thread " << t.id << "\n";

            for (int q = 0; q < quantum; q++) {
                t.cpuUsed++;
                cout << "  Tick: CPU used = " << t.cpuUsed
                     << "/" << t.burstTime << "\n";

                // ---------------- DEMONSTRATION RULES ----------------
		if(t.cpuUsed >= t.burstTime) {
		    t.state = TERMINATED;
		    t.waitingOn = "-";
		    cout << "  -> Thead " << t.id << " TERMINATED\n";
		    break;
		}

		int action = rand() % 100;

		if(action < 10){
		    Semaphore* s = randomSemaphore();
		    cout << "Thread " << t.id << " RANDOM WAIT on "
			 << s->name << "\n";
		    s->waitOp(t.id);

		    if (t.state == BLOCKED) break;
		} else if (action < 20) {
		    Semaphore* s = randomSemaphore();
		    cout << "Thread " << t.id << " RANDOM SIGNAL "
			 << s->name << "\n";
		    s->signalOp();
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
    createThread(8, 1);   // Thread 1 (wait S2)
    createThread(9, 2);   // Thread 2 (signal S1)
    createThread(6, 3);   // Thread 3 (signal S2)

    cout << "\nInitial Thread States:\n";
    printTable();

    runRoundRobin(2);

    return 0;
}
