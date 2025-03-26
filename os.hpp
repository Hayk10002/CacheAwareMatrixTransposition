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
    else std::cout << "Pinned to core " << core_id << " successfully.\n";
#else
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core_id, &cpuset);
    if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset) != 0) {
        std::cerr << "Failed to set CPU affinity.\n";
        return false;
    }
    else std::cout << "Pinned to core " << core_id << " successfully.\n";
#endif
    return true;
}

// Get L1 cache size and associativity for a given core
std::pair<int, int> get_L1_cache_info(int core_id) {
    int size_bytes;
    int associativity;
#ifdef _WIN32
    SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX buffer[128];
    DWORD len = sizeof(buffer);

    if (!GetLogicalProcessorInformationEx(RelationCache, buffer, &len)) {
        std::cerr << "Failed to get cache info.\n";
        return {-1, -1};
    }

    for (BYTE *ptr = reinterpret_cast<BYTE *>(buffer); ptr < reinterpret_cast<BYTE *>(buffer) + len;) {
        SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *info = reinterpret_cast<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *>(ptr);
        if (info->Relationship == RelationCache && info->Cache.Level == 1) {
            return {info->Cache.CacheSize, info->Cache.Associativity};
        }
        ptr += info->Size;
    }

    return {-1, -1};
#else
    // Read cache size
    std::string size_path = "/sys/devices/system/cpu/cpu" + std::to_string(core_id) + "/cache/index0/size";
    std::ifstream size_file(size_path);
    int size_kb = -1;
    if (size_file.is_open()) {
        std::string size_str;
        size_file >> size_str;
        size_file.close();

        try {
            size_kb = std::stoi(size_str);
        } catch (...) {
            std::cerr << "Failed to parse cache size.\n";
        }
    } else {
        std::cerr << "Failed to read cache size.\n";
    }
    size_bytes = (size_kb != -1) ? size_kb * 1024 : -1; // Convert KB to bytes

    // Read associativity
    std::string assoc_path = "/sys/devices/system/cpu/cpu" + std::to_string(core_id) + "/cache/index0/ways_of_associativity";
    std::ifstream assoc_file(assoc_path);
    if (assoc_file.is_open()) {
        assoc_file >> associativity;
        assoc_file.close();
    } else {
        std::cerr << "Failed to read cache associativity.\n";
        associativity = -1;
    }

    return {size_bytes, associativity};
#endif
}