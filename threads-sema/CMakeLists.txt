cmake_minimum_required(VERSION 3.27)

project(ostep-homework-solutions-semaphores)

set(CMAKE_C_STANDARD 23)

add_executable(fork-join fork-join.c)
add_executable(rendezvous rendezvous.c)
add_executable(barier barrier.c)
add_executable(reader-writer reader-writer.c)
add_executable(reader-writer-nostarve reader-writer-nostarve.c)
add_executable(reader-writer-nostarve-modern reader-writer-nostarve-modern.c)
add_executable(mutex-nostarve mutex-nostarve.c)
