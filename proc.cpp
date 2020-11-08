class proc
{
private:
    /* data */
public:
    proc(int x, int pTime);
    ~proc();
    int getTAT();
    double getNormTAT();
    double getResRatio(int current_time);
    int pid, burst, wait_time, arr_time, tat;
    int service_time;
    int waiting_start;
    bool isWaiting, hasArrived;
};

proc::proc(int x, int pTime)
{
    this->pid = x;
    this->burst = pTime; //service time remaining
    this->service_time = pTime;
    this->wait_time = 0;
    this->isWaiting = false;
    this->hasArrived = false;
}

proc::~proc()
{
}

int proc::getTAT()
{
    return wait_time + service_time;
}

double proc::getNormTAT()
{
    return (double)this->getTAT() / (double)service_time;
}

double proc::getResRatio(int current_time)
{
    double time_spent_waiting = current_time - waiting_start;
    return time_spent_waiting + (double)burst / (double)burst;
}
