cmake_minimum_required(VERSION 3.27)

project(ostep-homework-solutions)

set(CMAKE_C_STANDARD 23)

add_executable(cpu-api_ex1 cpu-api/solutions/code/ex1.c)
add_executable(cpu-api_ex2 cpu-api/solutions/code/ex2.c)
add_executable(cpu-api_ex3 cpu-api/solutions/code/ex3.c)
add_executable(cpu-api_ex4 cpu-api/solutions/code/ex4.c)
add_executable(cpu-api_ex7 cpu-api/solutions/code/ex7.c)
add_executable(cpu-api_ex8 cpu-api/solutions/code/ex8.c)

add_executable(cpu-mechanisms-measurement-syscall cpu-mechanisms-measurement/SysCallMeasurement.c)

add_executable(cpu-mechanisms-measurement-context-switch cpu-mechanisms-measurement/ContextSwitchMeasurement.c)

add_executable(vm-intro-memory-user vm-intro/solutions/code/memory-user.c)

add_executable(vm-api-null vm-api/solutions/code/null.c)
add_executable(vm-api-ex4 vm-api/solutions/code/ex4.c)
add_executable(vm-api-ex5 vm-api/solutions/code/ex5.c)
add_executable(vm-api-ex6 vm-api/solutions/code/ex6.c)
add_executable(vm-api-ex7 vm-api/solutions/code/ex7.c)
add_executable(vm-api-vector vm-api/solutions/code/vector.c)
target_link_libraries(vm-api-vector m)

add_executable(vm-tlbs-tlb vm-tlbs/code/tlb.c)
target_compile_options(vm-tlbs-tlb PRIVATE -O0)
