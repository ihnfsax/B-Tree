cmake_minimum_required(VERSION 3.0.0)
project(BPlusTree VERSION 0.1.0)

include(CTest)
enable_testing()

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ..)
set(CMAKE_CXX_STANDARD 17)
add_compile_options(-g)
aux_source_directory(. DIRSRCS)
add_executable(BPlusTree ${DIRSRCS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
