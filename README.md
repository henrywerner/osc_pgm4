## OS Concepts Programming Assignment 4

### Problem Statement
The task was to create a Monte Carlo simulation of multiple short-term scheduling algorithms using C/C++. Service times for each simulated process are randomly generated according to normal distribution with a mean of 10 and a standard deviation of 5. The program should simulate the results of the following scheduling algorithms: First Come First Serve, Round Robin (quantum = 1), Highest Response Ratio Next, and Feedback (quantum = 1). As per the requirements, each of these algorithms are tested 1000 times each with a set of 1000 imitation process.

### Approach
I developed the program primarily within Windows using the MinGW C++ compiler, then completed the final portion of the program on a Linux Ubuntu virtual machine. I chose to imitate the time steps of the simulation using integer values, which are referenced within the code as ‘ticks’. Within each scheduling simulation, the runtime of sorting calculations is excluded from the total tick duration but are included in the real time measurements. My testing approach involves generating a vector of processes using the system’s clock as a seed for random generation. Once a vector is generated, the same vector is passed to all four algorithms, allowing for a more direct comparison between the results.

### Build and Execution
```console
gherkin@Gherkin-VM:~/Documents/pgm4$ g++ monteCarlo.cpp -o pgm4.exe
gherkin@Gherkin-VM:~/Documents/pgm4$ ./pgm4.exe

[##############################] 100 %

[FCFS Results Avg.]
Time................... 9910.28 ticks
Turnaround Time........ 9.91029
Normalized TAT......... 1
Real Time.............. 0.018 ms
Total Real Time........ 18 ms
Norm. Response Time.... 0.001

[RR Results Avg.]
Time................... 9910.28 ticks
Turnaround Time........ 6539.69
Normalized TAT......... 672.678
Real Time.............. 2.07 ms
Total Real Time........ 2070 ms
Norm. Response Time.... 0.659889

[HRRN Results Avg.]
Time................... 9910.28 ticks
Turnaround Time........ 9.91029
Normalized TAT......... 1
Real Time.............. 7.085 ms
Total Real Time........ 7085 ms
Norm. Response Time.... 0.001

[FB Results Avg.]
Time................... 9910.28 ticks
Turnaround Time........ 6539.69
Normalized TAT......... 672.678
Real Time.............. 21.407 ms
Total Real Time........ 21407 ms
Norm. Response Time.... 0.659889
gherkin@Gherkin-VM:~/Documents/pgm4$
```
