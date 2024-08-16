#include <chrono>
#include <sys/resource.h>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
#include <cmath>

using namespace std;

class Metrics
{
private:
    std::chrono::high_resolution_clock::time_point start;
    double cpuUsage;
    double memoryUsage;
    double executeTime;

    double measureCpuUsage();
    double measureMemoryUsage();

public:
    Metrics() : cpuUsage(0.0), memoryUsage(0.0), executeTime(0.0) {};
    void init();
    void end();
    double getCpuUsage();
    double getMemoryUsage();
    double getExecuteTime();
};

void Metrics::init()
{
    this->start = std::chrono::high_resolution_clock::now();
    this->cpuUsage = measureCpuUsage();
    this->memoryUsage = measureMemoryUsage();
}

void Metrics::end()
{
    auto end = std::chrono::high_resolution_clock::now();
    this->executeTime = std::chrono::duration<double, std::milli>(end - start).count();
    this->cpuUsage = (executeTime / sysconf(_SC_CLK_TCK)) * 100.0;
}

double Metrics::measureCpuUsage()
{
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return (usage.ru_utime.tv_sec * 1000 + usage.ru_utime.tv_usec / 1000) / 1000.0;
}

double Metrics::measureMemoryUsage()
{
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss; // en KB
}

double Metrics::getCpuUsage()
{
    return this->cpuUsage;
}

double Metrics::getExecuteTime()
{
    return executeTime / 1000.0; // convertir milisegundos a segundos
}

double Metrics::getMemoryUsage()
{
    return memoryUsage;
}

void computeTask(int start, int end, vector<double> &results)
{
    for (int i = start; i < end; ++i)
    {
        results[i] = sqrt(i) * sin(i);
    }
}