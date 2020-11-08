#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <vector>
#include <queue>
#include <chrono>
#include "proc.cpp"

using namespace std;
using namespace std::chrono;

#define PROCESS_NUM 1000

vector<proc> gen_proc(int ammount, unsigned seed);
void fcfs(queue<proc> q);
void rr(queue<proc> q, int quantum);
vector<string> hrrn(vector<proc> q);
string print_stats(queue<proc> completed, int total_time);
vector<string> get_stats(queue<proc> completed, int total_time);

int main()
{
    typedef std::chrono::high_resolution_clock globalClock;
    globalClock::time_point beginning = globalClock::now();
    globalClock::duration d = globalClock::now() - beginning;
    unsigned seed = d.count();

    vector<proc> processes;
    processes = gen_proc(PROCESS_NUM, seed);

    queue<proc> q;

    for (proc p : processes)
    {
        //cout << "proc " << p.pid << ".\t burst: " << p.burst << endl;
        q.push(p);
    }

    //First Come First Serve
    auto tStart = high_resolution_clock::now();
    //fcfs(q);
    auto tStop = high_resolution_clock::now();
    auto tDuration = duration_cast<milliseconds>(tStop - tStart);
    cout << "\nFCFS finished in " << (double)tDuration.count() << "ms" << endl;

    //Round Robin Quantum 1
    tStart = high_resolution_clock::now();
    //rr(q, 1);
    tStop = high_resolution_clock::now();
    tDuration = duration_cast<milliseconds>(tStop - tStart);
    cout << "\nRR finished in " << (double)tDuration.count() << "ms" << endl;

    vector<vector<string>> results_hrrn;
    for (int x = 1; x <= 1000; x++) //TEST
    {
        globalClock::duration d = globalClock::now() - beginning;
        unsigned seed = d.count();

        vector<proc> processes = gen_proc(PROCESS_NUM, seed);

        //Highest Response Ratio Next
        tStart = high_resolution_clock::now();

        vector<string> H = hrrn(processes);

        tStop = high_resolution_clock::now();
        tDuration = duration_cast<milliseconds>(tStop - tStart);
        H.push_back(to_string(tDuration.count()));

        results_hrrn.push_back(H);

        cout << "[HRRN " << to_string(x) << "]  T: " << H[0] << "  TAT: " << H[1].substr(0, 6)
             << "  NTAT: " << H[2].substr(0, 3) << "\tReal Time: " << H[3] << "ms" << endl;
    }

    vector<double> results; //it's 2:36am, i know this code is trash. I'll fix it later.
    results.push_back(0);
    results.push_back(0);
    results.push_back(0);
    results.push_back(0);
    for (int r = 0; r < results_hrrn.size(); r++)
    {
        for (int c = 0; c < 4; c++)
        {
            results[c] += stod(results_hrrn[r][c]);
        }
    }

    for (int c = 0; c < 4; c++)
    {
        results[c] /= results_hrrn.size();
    }

    cout << "\nHRRN Results.\n";
    cout << "Avg Time: " << results[0] << " ticks\n";
    cout << "Avg TAT: " << results[1] << endl;
    cout << "Avg NTAT: " << results[2] << endl;
    cout << "Avg Real Time: " << results[3] << "ms " << endl;

    /*
    //Highest Response Ratio Next
    tStart = high_resolution_clock::now();
    hrrn(processes);
    tStop = high_resolution_clock::now();
    tDuration = duration_cast<milliseconds>(tStop - tStart);
    cout << "\nHRRN finished in " << (double)tDuration.count() << "ms" << endl;
    */

    return 0;
}

vector<proc> gen_proc(int ammount, unsigned seed)
{
    vector<proc> processes;
    default_random_engine g;
    g.seed(seed);
    normal_distribution<double> distribution(10.0, 5.0);

    for (int x = 1; x <= ammount; x++)
    {
        double number = distribution(g);
        if (number >= 1.0)
        {
            proc *k = new proc(x, number);
            processes.push_back(*k);
        }
        else
        {
            x--;
        }
    }

    return processes;
}

//generate data

//FCFS
void fcfs(queue<proc> q)
{
    //auto fcfs_start = high_resolution_clock::now();
    int total_exeTime = 0;

    while (!q.empty())
    {
        int exeTime = 0;
        while (q.front().burst != 0)
        {
            //cout << q.front().burst;
            exeTime++;
            q.front().burst--;
        }
        int pid = q.front().pid;
        q.pop();
        cout << "pid " << pid << " - exeTime: " << exeTime << endl;
        total_exeTime += exeTime;
    }

    cout << "\nFCFS Complete - Total execution time: " << total_exeTime << endl;
    //auto fcfs_stop = high_resolution_clock::now();
    //auto fcfs_duration = duration_cast<nanoseconds>(fcfs_stop - fcfs_start);
    //cout << "FCFS finished in " << (double)fcfs_duration.count() << " ns" << endl;
}

//RR q=1
void rr(queue<proc> q, int quantum)
{
    queue<proc> completed;
    int total_time = 0;
    while (!q.empty())
    {
        int exeTime = 0;
        while (q.front().burst != 0 && exeTime < quantum)
        {
            if (!q.front().hasArrived) //proc accessed for first time
            {
                q.front().hasArrived = true;
                q.front().arr_time = total_time;
            }
            if (q.front().isWaiting) //proc was waiting
            {
                q.front().isWaiting = false;
                q.front().wait_time += total_time - q.front().waiting_start;
                q.front().waiting_start = 0;
            }

            exeTime++;
            total_time++;
            q.front().burst--;
        }
        if (q.front().burst == 0) //proc finished
        {
            proc p = q.front();
            //int pid = q.front().pid;
            q.pop();
            completed.push(p);
            string o = "pid " + to_string(p.pid);
            o += " ar:" + to_string(p.arr_time);
            o += " w:" + to_string(p.wait_time);
            o += " exe:" + to_string(p.service_time);
            o += " TAT:" + to_string(p.getTAT());
            o += " NTAT:" + to_string(p.getNormTAT());
            o += "\n";
            cout << o;
        }
        else //exited before completed
        {
            proc p = q.front();
            p.isWaiting = true;
            p.waiting_start = total_time;
            q.pop();
            q.push(p); //move to back of queue
        }
    }

    string stats = print_stats(completed, total_time);
    cout << "\nRR Complete." << stats;
}

//HRRN
vector<string> hrrn(vector<proc> q)
{
    queue<proc> completed;
    int total_time = 0;
    while (!q.empty())
    {
        while (q.front().burst != 0)
        {
            int exeTime = 0;
            if (!q.front().hasArrived) //proc accessed for first time
            {
                q.front().hasArrived = true;
                q.front().arr_time = total_time;
            }

            exeTime++;
            total_time++;
            q.front().burst--;
        }

        //Complete process
        proc p = q.front();
        proc b = q.back();
        q.front() = b; //swap front with back
        q.back() = p;
        q.pop_back(); //remove back
        completed.push(p);

        // string o = "pid " + to_string(p.pid);
        // o += " ar:" + to_string(p.arr_time);
        // o += " w:" + to_string(p.wait_time);
        // o += " exe:" + to_string(p.service_time);
        // o += " TAT:" + to_string(p.getTAT());
        // o += " NTAT:" + to_string(p.getNormTAT());
        // o += "\n";
        // cout << o;

        //search for hrr
        int hrr_proc = 0, hrr = 0;
        for (int x = 0; x < q.size(); x++)
        {
            int y = q[x].getResRatio(total_time);
            if (y > hrr)
            {
                hrr = y;
                hrr_proc = x;
            }
        }
        //cout << "HRR: pid" << to_string(hrr_proc) << " - " << to_string(hrr) << endl;
        //swap hrr to front
        p = q.front();
        b = q[hrr_proc];
        q.front() = b;
        q[hrr_proc] = p;
    }
    //print results
    //string stats = print_stats(completed, total_time);
    //cout << "\nHRRN Completed." << stats;
    return get_stats(completed, total_time);
}

//FB q=1

string print_stats(queue<proc> completed, int total_time)
{
    double mean_tat = 0, mean_ntat = 0;
    while (!completed.empty())
    {
        mean_tat += completed.front().getTAT();
        mean_ntat += completed.front().getNormTAT();
        completed.pop();
    }
    mean_tat /= PROCESS_NUM;
    mean_ntat /= PROCESS_NUM;

    string x = "\nTotal time: " + to_string(total_time);
    x += "\nTAT mean: " + to_string(mean_tat);
    x += "\nNTAT mean: " + to_string(mean_ntat);
    x += "\n";

    return x;
}

vector<string> get_stats(queue<proc> completed, int total_time)
{
    vector<string> stats;
    double mean_tat = 0, mean_ntat = 0;
    while (!completed.empty())
    {
        mean_tat += completed.front().getTAT();
        mean_ntat += completed.front().getNormTAT();
        completed.pop();
    }
    mean_tat /= PROCESS_NUM;
    mean_ntat /= PROCESS_NUM;

    //[total_time, tat, ntat]
    stats.push_back(to_string(total_time));
    stats.push_back(to_string(mean_tat));
    stats.push_back(to_string(mean_ntat));

    return stats;
}

/*
for (each experiment)
{
    generate data (1,000 process service times)
    run algorithms to produce statistics
    accumulate statistics
}
print statistics
*/

/*
- min service time is one
*/