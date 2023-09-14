#include <chrono>
#include <sys/resource.h>
#include <sstream>
#include <iostream>
#include <unistd.h>
class Metrics
{
private:
    std::chrono::high_resolution_clock::time_point start;
   //std::clock_t start;
    double cpuUsage;
    double memoryUsage;
    double executeTime;
public:
    Metrics():cpuUsage(0.0),memoryUsage(0.0){};
    /**
     * @brief Iniciliza los parametros de las metricas de uso de recursos RAM, CPU, Tiempo de ejecucion.
    */
    void init();
    /**
     * @brief Calculas las metricas.
    */
    void end();
    /**
     * @brief Retorna el valor de % de uso de CPU.
    */
    double getCpuUsage();
    double getCpuUsage(pid_t pid);
    /**
     * @brief Retorna el valor de la memoria utilzada en KB.
    */
    double getMemoryUsage();
    /**
     * @brief Retorna el tiemp de ejecucion en segundos.
    */
    double getExecuteTime();
};

void Metrics::init(){
    this->start=std::chrono::high_resolution_clock::now();
    struct rusage usage;
    getrusage(RUSAGE_SELF,&usage);
    this->cpuUsage=(usage.ru_utime.tv_sec*1000 + usage.ru_utime.tv_usec/1000)/1000.0;
    this->memoryUsage=usage.ru_maxrss;///1024.0;
    
}

void Metrics::end(){
    auto end=std::chrono::high_resolution_clock::now();
    this->executeTime= std::chrono::duration<double, std::milli>(end - start).count();///sysconf(_SC_CLK_TCK);//static_cast<double>(end - start)/CLOCKS_PER_SEC;
    this->cpuUsage= (executeTime/sysconf(_SC_CLK_TCK))*100.0; //(this->cpuUsage/(this->cpuUsage + this->executeTime))*100.0;

}

double Metrics::getCpuUsage(){
    return this->cpuUsage;
}

double Metrics::getExecuteTime(){
    return executeTime/1000.0;
}

double Metrics::getMemoryUsage(){
    return memoryUsage;
}

double Metrics::getCpuUsage(pid_t pid){
    double cpu_usage = 0.0;
    ifstream stat("/proc/stat");
    if (stat.is_open()) {
        string line;
        while (getline(stat, line)) {
            if (line.substr(0, 3) == "cpu") {
                stringstream ss(line);
                string cpu;
                ss >> cpu;
                cout<<"debug: "<<cpu<<endl;
                if (stoi(cpu.substr(4, 8)) == pid) {
                    int user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
                    ss >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
                    int total_time = user + nice + system + idle + iowait + irq + softirq + steal;
                    int idle_time = idle + iowait;
                    cpu_usage = (total_time - idle_time) * 100.0 / total_time;
                    break;
                }
            }
        }
        stat.close();
    }
    return cpu_usage;
}




