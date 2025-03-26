#include <iostream>
#include <random>
#include <optional>

#include "os.hpp"

std::optional<std::pair<int, int>> setup()
{
    int core_id = get_current_core_id();
    std::cout << "Process started on Core ID: " << core_id << "\n";
    if(!pin_to_core(core_id)) return std::nullopt;

    auto [l1_cache_size, associativity] = get_L1_cache_info(core_id);

    if (l1_cache_size == -1 || associativity == -1) return std::nullopt;
    
    std::cout << "L1 Cache Size for Core " << core_id << ": " << l1_cache_size / 1024 << " KB\n";
    std::cout << "L1 Cache Associativity for Core " << core_id << ": " << associativity << "-way\n";
    
    return std::pair{l1_cache_size, associativity};
}

int main() {
    
    auto info = setup();
    if (!info) return 1;

    auto [l1_cache_size, associativity] = *info;

    

    return 0;
}
