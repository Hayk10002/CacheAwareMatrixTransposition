#include <iostream>
#include <random>
#include <optional>
#include <numeric>
#include <chrono>

#include "os.hpp"

int generateRandomNumber(int min = 0, int max = 100) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(min, max);
    return dis(gen);
}

bool setup(int& l1_cache_size, int& associativity, int& cache_line_size)
{
    int core_id = get_current_core_id();
    std::cout << "Process started on Core ID: " << core_id << "\n";
    if(!pin_to_core(core_id)) return false;

    l1_cache_size = get_L1_cache_size(core_id);
    associativity = get_L1_cache_associativity(core_id);
    cache_line_size = get_L1_cache_line_size(core_id); 

    if (l1_cache_size == -1 || associativity == -1 || cache_line_size == -1) return false;
    
    std::cout << "L1 Cache Size for Core " << core_id << ": " << l1_cache_size / 1024 << " KB\n";
    std::cout << "Cache-line size: " << cache_line_size << "\n";
    std::cout << "L1 Cache Associativity for Core " << core_id << ": " << associativity << "-way\n";
    
    return true;
}

void transpose(std::span<int> matrix, int matrix_size, int block_size)
{
    for (int i = 0; i <= matrix_size / block_size; i++)
        for (int j = 0; j <= i; j++)
            for (int ii = i * block_size; ii < (i + 1) * block_size && ii < matrix_size; ii++)
                for (int jj = j * block_size; jj < (j + 1) * block_size && jj < ii; jj++)
                    std::swap(matrix[ii * matrix_size + jj], matrix[jj * matrix_size + ii]);
}

auto time(int matrix_size, int block_size, int iteration_count)
{
    std::vector<int> A(matrix_size * matrix_size);
    std::generate(A.begin(), A.end(), [](){ return generateRandomNumber(); });

    auto start = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < iteration_count; i++) transpose(A, matrix_size, block_size);

    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start);
}

int main(int argc, char* argv[]) {
    
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <matrix_size> <iteration_count>";
        return 1;
    }

    int SIZE = std::stoi(argv[1]);
    int ITERATION_COUNT = std::stoi(argv[2]);
    
    int l1_cache_size, associativity, cache_line_size;
    if (!setup(l1_cache_size, associativity, cache_line_size)) return 1;

    int rows_without_eviction = l1_cache_size / std::gcd(l1_cache_size / associativity, SIZE * sizeof(int));
    int elements_in_one_cache_line = cache_line_size / sizeof(int);
    
    int BLOCK_SIZE = std::max(elements_in_one_cache_line, rows_without_eviction / elements_in_one_cache_line * elements_in_one_cache_line);

    std::cout << "Calculated optimal block size: " << BLOCK_SIZE << '\n';

    std::cout << std::format("Transposition with    blocks: {:<5}\n", time(SIZE, BLOCK_SIZE, ITERATION_COUNT));
    std::cout << std::format("Transposition without blocks: {:<5}\n", time(SIZE,       SIZE, ITERATION_COUNT));
    
    return 0;
}
