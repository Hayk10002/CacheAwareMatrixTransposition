#pragma once
#include <iostream>
#include <utility>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <sched.h>
    #include <fstream>
    #include <string>
#endif

// Get the core ID of the current thread
int get_current_core_id() {
#ifdef _WIN32
    return GetCurrentProcessorNumber();
#else
    return sched_getcpu();
#endif
}

// Pin the process to a specific core
bool pin_to_core(int core_id) {
#ifdef _WIN32
    DWORD_PTR mask = 1ULL << core_id;
    if (SetThreadAffinityMask(GetCurrentThread(), mask) == 0) {
        std::cerr << "Failed to set CPU affinity.\n";
        return false;
    }

#else
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset) != 0) {
        std::cerr << "Failed to set CPU affinity.\n";
        return false;
    }
#endif
    return true;
}

// Get L1 cache size
int get_L1_cache_size(int core_id) {
#ifdef _WIN32
    SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX buffer[128];
    DWORD len = sizeof(buffer);

    if (!GetLogicalProcessorInformationEx(RelationCache, buffer, &len)) {
        std::cerr << "Failed to get cache info.\n";
        return -1;
    }

    for (BYTE *ptr = reinterpret_cast<BYTE *>(buffer); ptr < reinterpret_cast<BYTE *>(buffer) + len;) {
        SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *info = reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *>(ptr);
        if (info->Relationship == RelationCache && info->Cache.Level == 1) {
            return info->Cache.CacheSize;
        }
        ptr += info->Size;
    }
    return -1;
#else
    std::string path = "/sys/devices/system/cpu/cpu" + std::to_string(core_id) + "/cache/index0/size";
    std::ifstream file(path);
    int size_kb = -1;
    if (file.is_open()) {
        file >> size_kb;
        file.close();
    } else {
        std::cerr << "Failed to read cache size.\n";
    }
    return size_kb * 1024;
#endif
}

// Get L1 cache associativity
int get_L1_cache_associativity(int core_id) {
#ifdef _WIN32
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

// Get L1 cache line size
int get_L1_cache_line_size(int core_id) {
#ifdef _WIN32
    SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX buffer[128];
    DWORD len = sizeof(buffer);

    if (!GetLogicalProcessorInformationEx(RelationCache, buffer, &len)) {
        std::cerr << "Failed to get cache info.\n";
        return -1;
    }

    for (BYTE *ptr = reinterpret_cast<BYTE *>(buffer); ptr < reinterpret_cast<BYTE *>(buffer) + len;) {
        SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *info = reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *>(ptr);
        if (info->Relationship == RelationCache && info->Cache.Level == 1) {
            return info->Cache.LineSize;
        }
        ptr += info->Size;
    }
    return -1;
#else
    std::string path = "/sys/devices/system/cpu/cpu" + std::to_string(core_id) + "/cache/index0/coherency_line_size";
    std::ifstream file(path);
    int line_size = -1;
    if (file.is_open()) {
        file >> line_size;
        file.close();
    } else {
        std::cerr << "Failed to read cache line size.\n";
    }
    return line_size;
#endif
}
