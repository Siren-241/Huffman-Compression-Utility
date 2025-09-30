cmake_minimum_required(VERSION 3.10)

project(HuffmanCompressor VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(Compressor 
                "src/main.cpp"
                "src/lib/HuffmanCoding.cpp"
)
