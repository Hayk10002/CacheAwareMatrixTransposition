cmake_minimum_required(VERSION 3.10)

project(CacheAwareMergeSort)

set(CMAKE_CXX_STANDARD 23)
add_executable(main main.cpp)

# Add the main executable as a test
enable_testing()
add_test(NAME MainTest1 COMMAND main 10 100000)
add_test(NAME MainTest2 COMMAND main 20 10000)
add_test(NAME MainTest3 COMMAND main 32 10000)
add_test(NAME MainTest4 COMMAND main 50 10000)
add_test(NAME MainTest5 COMMAND main 64 10000)
add_test(NAME MainTest6 COMMAND main 100 1000)
add_test(NAME MainTest7 COMMAND main 128 1000)
add_test(NAME MainTest8 COMMAND main 200 1000)
add_test(NAME MainTest9 COMMAND main 256 1000)
add_test(NAME MainTest10 COMMAND main 500 1000)
add_test(NAME MainTest11 COMMAND main 512 1000)
add_test(NAME MainTest12 COMMAND main 1000 100)
add_test(NAME MainTest13 COMMAND main 1024 100)
add_test(NAME MainTest14 COMMAND main 2000 100)
add_test(NAME MainTest15 COMMAND main 2048 100)