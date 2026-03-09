cmake_minimum_required(VERSION 3.10)
project(MFUAlgorithmSimulator)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Archivos fuente
set(SOURCES
    main.cpp
    MMU.cpp
    PageTable.cpp
    TLB.cpp
    MFUAlgorithm.cpp
)

# Crear ejecutable
add_executable(MFUSimulator ${SOURCES})

# Opciones de compilación
if(MSVC)
    target_compile_options(MFUSimulator PRIVATE /W4)
else()
    target_compile_options(MFUSimulator PRIVATE -Wall -Wextra -Wpedantic)
endif()
