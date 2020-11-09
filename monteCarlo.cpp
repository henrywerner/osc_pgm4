/*
    file: monteCarlo.cpp
    author: Gherkin
    modification history: 
        Gherkin
        November 8th, 2020
    procedures:
        main - Calls generation and scheduling simulation functions, generates randomization seeds, and records simulation results.
        gen_proc - Generates a vector of imitation process objects based on a given seed.
        fcfc - Simulates the First Come First Serve scheduling algorithm and returns the simulation results.
        rr - Simulates the Round Robin scheduling algorithm using a provided quantum value and returns the simulation results.
        hrrn - Simulates the Highest Response Ratio Next scheduling algorithm and returns the simulation results.
        fb - Simulates the Feedback scheduling algorithm using a provided quantum value and returns the simulation results.
        get_stats - Parses the information from a completed simulation and translates it into a readable format.
*/

#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <vector>
#include <queue>
#include <math.h>
#include <chrono>
#include "proc.cpp"

using namespace std;
using namespace std::chrono;

#define PROCESS_NUM 1000
#define TEST_NUM 1000

vector<proc> gen_proc(int ammount, unsigned seed);
vector<string> fcfs(queue<proc> q);
vector<string> rr(queue<proc> q, int quantum);
vector<string> hrrn(vector<proc> q);
vector<string> fb(vector<proc> q, int quantum);
vector<string> get_stats(queue<proc> completed, int total_time);

/*
    int main()
    author: Gherkin
    date: Nov 8, 2020
    description: Calls generation and scheduling simulation functions, generates randomization seeds, and records simulation results.
*/
int main()
{
    //seed generation based on system clock
    typedef std::chrono::high_resolution_clock globalClock;
    globalClock::time_point beginning = globalClock::now();
    globalClock::duration d = globalClock::now() - beginning;
    unsigned seed = d.count();

    vector<vector<string>> results_fcfs, results_rr, results_hrrn, results_fb;

    cout << endl;

    //run set ammount of experiments
    for (int x = 1; x <= TEST_NUM; x++)
    {
        //gen new seed
        globalClock::duration d = globalClock::now() - beginning;
        unsigned seed = d.count();

        //gen new process vector
        vector<proc> processes = gen_proc(PROCESS_NUM, seed);
        queue<proc> q;
        for (proc p : processes)
        {
            q.push(p);
        }

        //FCFS
        auto tStart_FC = high_resolution_clock::now(); //start rt timer
        vector<string> FC = fcfs(q);
        auto tStop_FC = high_resolution_clock::now(); //stop rt timer
        auto tDuration_FC = duration_cast<milliseconds>(tStop_FC - tStart_FC);
        FC.push_back(to_string(tDuration_FC.count()));
        results_fcfs.push_back(FC);

        // cout << "[FC " << to_string(x) << "]  T: " << FC[0] << "  TAT: " << FC[1].substr(0, 6)
        //      << "  NTAT: " << FC[2].substr(0, 3) << "\tReal Time: " << FC[3] << "ms" << endl;

        //RR
        auto tStart_R = high_resolution_clock::now(); //start rt timer
        vector<string> R = rr(q, 1);
        auto tStop_R = high_resolution_clock::now(); //stop rt timer
        auto tDuration_R = duration_cast<milliseconds>(tStop_R - tStart_R);
        R.push_back(to_string(tDuration_R.count()));
        results_rr.push_back(R);

        // cout << "[RR " << to_string(x) << "]  T: " << R[0] << "  TAT: " << R[1].substr(0, 6)
        //      << "  NTAT: " << R[2].substr(0, 3) << "\tReal Time: " << R[3] << "ms" << endl;

        //HRRN
        auto tStart_H = high_resolution_clock::now();
        vector<string> H = hrrn(processes);
        auto tStop_H = high_resolution_clock::now();
        auto tDuration_H = duration_cast<milliseconds>(tStop_H - tStart_H);
        H.push_back(to_string(tDuration_H.count()));
        results_hrrn.push_back(H);

        // cout << "[HRRN " << to_string(x) << "]  T: " << H[0] << "  TAT: " << H[1].substr(0, 6)
        //      << "  NTAT: " << H[2].substr(0, 3) << "\tReal Time: " << H[3] << "ms" << endl;

        //FB
        auto tStart_FB = high_resolution_clock::now();
        vector<string> FB = fb(processes, 1);
        auto tStop_FB = high_resolution_clock::now();
        auto tDuration_FB = duration_cast<milliseconds>(tStop_FB - tStart_FB);
        FB.push_back(to_string(tDuration_FB.count()));
        results_fb.push_back(FB);

        // cout << "[FB " << to_string(x) << "]  T: " << FB[0] << "  TAT: " << FB[1].substr(0, 6)
        //      << "  NTAT: " << FB[2].substr(0, 3) << "\tReal Time: " << FB[3] << "ms" << endl;

        //display progress bar
        if (x % 25 == 0)
        {
            int barWidth = 30;
            float progress = (float)x / 1000;
            std::cout << "[";
            int pos = barWidth * progress;
            for (int i = 0; i < barWidth; ++i)
            {
                if (i <= pos)
                    std::cout << "#";
                else
                    std::cout << "_";
            }
            std::cout << "] " << int(progress * 100.0) << " %\r";
            std::cout.flush();
        }
    }

    cout << endl;

    //Analize results
    vector<vector<double>> results; //it's 2:36am, i know this code is trash. I'll fix it later.
    vector<double> temp;
    temp.assign(4, 0);
    results.assign(4, temp);
    temp.clear();

    for (int r = 0; r < TEST_NUM; r++) //combine all results into one double to calculate the total average
    {
        for (int c = 0; c < 4; c++)
        {
            results[0][c] += stod(results_fcfs[r][c]);
            results[1][c] += stod(results_rr[r][c]);
            results[2][c] += stod(results_hrrn[r][c]);
            results[3][c] += stod(results_fb[r][c]);
        }
    }

    for (int c = 0; c < 4; c++)
    {
        for (int d = 0; d < 4; d++)
        {
            results[d].push_back(results[d][3]); //add additional total real time element to each vector
            results[c][d] /= TEST_NUM;
        }

        switch (c)
        {
        case 0:
            cout << "\n[FCFS Results Avg.]\n";
            break;
        case 1:
            cout << "\n[RR Results Avg.]\n";
            break;
        case 2:
            cout << "\n[HRRN Results Avg.]\n";
            break;
        case 3:
            cout << "\n[FB Results Avg.]\n";
            break;
        default:
            break;
        }

        double norm_responsetime = (results[c][1] / results[c][0]);
        double processor_utilization = 1 - pow(norm_responsetime, -1);

        cout << "Time................... " << results[c][0] << " ticks\n"; 
        cout << "Turnaround Time........ " << results[c][1] << endl;
        cout << "Normalized TAT......... " << results[c][2] << endl;
        cout << "Real Time.............. " << results[c][3] << " ms " << endl;
        cout << "Total Real Time........ " << results[c][4] << " ms " << endl;
        cout << "Norm. Response Time.... " << norm_responsetime << endl;
        cout << "Processor Utilization.. " << processor_utilization << endl;
    }

    return 0;
}


/*
    vector<proc> gen_proc(int ammount, unsigned seed)
    author: Gherkin
    date: Nov 8, 2020
    description: Generates a vector of imitation process objects within a normal distribution based on a given seed.
    parameters:
        ammount   I/P  int           Desired number of processes in the generated vector
        seed      I/P  unsigned      Randomization seed used for normal distribution generation
        gen_proc  O/P  vector<proc>  Vector containing the randomly generated imitation processes
*/
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

/*
    vector<string> fcfs(queue<proc> q)
    author: Gherkin
    date: Nov 8, 2020
    description: Simulates the First Come First Serve scheduling algorithm and returns the simulation results.
    parameters:
        q      I/P   queue<proc>     Ready queue containing the imitation processes to be handled by the scheduling algorithm 
        fcfs   O/P   vector<string>  Vector containing the final results of the simulation
*/
vector<string> fcfs(queue<proc> q)
{
    int total_time = 0;
    queue<proc> completed;

    while (!q.empty())
    {
        while (q.front().burst != 0)
        {
            if (!q.front().hasArrived) //proc accessed for first time
            {
                q.front().hasArrived = true;
                q.front().arr_time = total_time;
            }
            total_time++;
            q.front().burst--;
        }
        proc p = q.front();
        q.pop();
        completed.push(p);
    }
    return get_stats(completed, total_time);
}

/*
    vector<string> rr(queue<proc> q, int quantum)
    author: Gherkin
    date: Nov 8, 2020
    description: Simulates the Round Robin scheduling algorithm using a provided quantum value and returns the simulation results.
    parameters:
        q        I/P  queue<proc>     Ready queue containing the imitation processes to be handled by the scheduling algorithm 
        quantum  I/P  int             Value representing the time before a clock interrupt occurs
        rr       O/P  vector<string>  Vector containing the final results of the simulation
*/
vector<string> rr(queue<proc> q, int quantum)
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
            q.pop();
            completed.push(p);
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
    return get_stats(completed, total_time);
}

/*
    vector<string> hrrn(queue<proc> q)
    author: Gherkin
    date: Nov 8, 2020
    description: Simulates the Highest Response Ratio Next scheduling algorithm and returns the simulation results.
    parameters:
        q      I/P   queue<proc>      Ready queue containing the imitation processes to be handled by the scheduling algorithm 
        hrrn   O/P   vector<string>   Vector containing the final results of the simulation
*/
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
        //swap hrr to front
        p = q.front();
        b = q[hrr_proc];
        q.front() = b;
        q[hrr_proc] = p;
    }
    return get_stats(completed, total_time);
}

/*
    vector<string> fb(vector<proc> q, int quantum)
    author: Gherkin
    date: Nov 8, 2020
    description: Simulates the Feedback scheduling algorithm using a provided quantum value and returns the simulation results.
    parameters:
        q        I/P  queue<proc>     Ready queue containing the imitation processes to be handled by the scheduling algorithm 
        quantum  I/P  int             Value representing the time before a clock interrupt occurs
        fb       O/P  vector<string>  Vector containing the final results of the simulation
*/
vector<string> fb(vector<proc> q, int quantum)
{
    vector<vector<proc>> rq;
    rq.push_back(q);
    int priority = 0;
    queue<proc> completed;
    int total_time = 0;
    while (!rq.empty()) //remember to delete empty priority queues!
    {
        if (rq.size() == priority + 1)
        {
            vector<proc> new_ready_queue;
            rq.push_back(new_ready_queue);
        }

        while (!rq[priority].empty())
        {
            int exeTime = 0;
            while (rq[priority].front().burst != 0 && exeTime < quantum)
            {
                if (!rq[priority].front().hasArrived) //proc accessed for first time
                {
                    rq[priority].front().hasArrived = true;
                    rq[priority].front().arr_time = total_time;
                }
                if (rq[priority].front().isWaiting) //proc was waiting
                {
                    rq[priority].front().isWaiting = false;
                    rq[priority].front().wait_time += total_time - rq[priority].front().waiting_start;
                    rq[priority].front().waiting_start = 0;
                }

                exeTime++;
                total_time++;
                rq[priority].front().burst--;
            }
            if (rq[priority].front().burst == 0) //proc finished
            {
                proc p = rq[priority].front();
                completed.push(p);
                rq[priority].erase(rq[priority].begin());
            }
            else //exited before completed
            {
                proc p = rq[priority].front();
                p.isWaiting = true;
                p.waiting_start = total_time;
                rq[priority + 1].push_back(p);            //move to lower priority queue
                rq[priority].erase(rq[priority].begin());
            }
        }

        rq.erase(rq.begin()); //delete the now empty ready queue
        if (rq[0].empty())    //if the next additional ready queue is also empty, delete that too
            rq.erase(rq.begin());
        //priority++;
    }
    return get_stats(completed, total_time);
}

/*
    vector<string> get_stats(queue<proc> completed, int total_time)
    author: Gherkin
    date: Nov 8, 2020
    description: Parses the information from a completed simulation and translates the important details into a string vector.
    parameters:
        completed    I/P   queue<proc>     Queue containing all completed processes from a simulation
        total_time   I/P   int             Total duration of the simulation mesured in ticks
        get_status   O/P   vector<string>  Vector containing the final results of the simulation
*/
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
