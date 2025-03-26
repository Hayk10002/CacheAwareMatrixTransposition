#include <iostream>
#include <random>
#include <thread>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sched.h>
    #include <unistd.h>
    #include <fstream>
#endif

void pin_to_core(int core_id) {
#ifdef _WIN32
    DWORD_PTR mask = 1ULL << core_id;
    if (SetThreadAffinityMask(GetCurrentThread(), mask) == 0) {
        std::cerr << "Failed to set CPU affinity.\n";
    }
#else
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset) != 0) {
        std::cerr << "Failed to set CPU affinity.\n";
    }
#endif
}

int get_L1_cache_associativity(int core_id) {
#ifdef _WIN32
    std::cout << "Windows\n";
    SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX buffer[128];
    DWORD len = sizeof(buffer);

    if (!GetLogicalProcessorInformationEx(RelationCache, buffer, &len)) {
        std::cerr << "Failed to get cache info.\n";
        return -1;
    }

    for (BYTE *ptr = reinterpret_cast<BYTE *>(buffer); ptr < reinterpret_cast<BYTE *>(buffer) + len;) {
        SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *info = reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *>(ptr);
        if (info->Relationship == RelationCache && info->Cache.Level == 1) {
            return info->Cache.Associativity;
        }
        ptr += info->Size;
    }
    return -1;
#else
    std::cout << "Linux\n";
    std::string path = "/sys/devices/system/cpu/cpu" + std::to_string(core_id) + "/cache/index0/ways_of_associativity";
    std::ifstream file(path);
    int associativity = -1;
    if (file.is_open()) {
        file >> associativity;
        file.close();
    } else {
        std::cerr << "Failed to read cache associativity.\n";
    }
    return associativity;
#endif
}

int generateRandomNumber(int min = 0, int max = 100) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(min, max);
    return dis(gen);
}


int main(int argc, char* argv[]) 
{
    int core_id = 0;  // Change this to the desired core ID
    pin_to_core(core_id);

    int associativity = get_L1_cache_associativity(core_id);
    if (associativity != -1) {
        std::cout << "L1 Cache Associativity for Core " << core_id << ": " << associativity << "\n";
    } else {
        std::cerr << "Failed to determine L1 cache associativity.\n";
    }

    //std::cout << generateRandomNumber();
    return 0;
}