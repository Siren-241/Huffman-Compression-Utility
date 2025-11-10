#include "lib/include/HuffmanCoding.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {

  if (argc > 1) {
    if (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help")
      std::cout << "Usage:\n1. Compression: ./Compressor [input_file] "
                   "[output_file_path] "
                   "[output_codes_path]\n2. Decompression: ./Compressor -d "
                   "[compressed_file_path] "
                   "[codes_file_path] [target_decompressed_path]\n";
    return 0;
  }

  std::string inputFile = "../testFiles/input.txt";
  std::string compressedFile = "../output/compressed.bin";
  std::string codesFile = "../output/codes.txt";
  std::string decompressedFile = "../output/decompressed.txt";

  // Ensure directories exist
  std::filesystem::create_directories("testFiles");
  std::filesystem::create_directories("output");

  HuffmanCoding huffman;

  // Case 1: Decompression Mode => ./a.out -d compressed.bin codes.txt
  // decompressed.txt
  if (argc > 1 && std::string(argv[1]) == "-d") {
    if (argc >= 3)
      compressedFile = argv[2];
    if (argc >= 4)
      codesFile = argv[3];
    if (argc >= 5)
      decompressedFile = argv[4];

    std::cout << "Decompressing...\n";
    huffman.decompressFile(compressedFile, codesFile, decompressedFile);
    std::cout << "Decompression complete.\n";
    std::cout << "Output written to: " << decompressedFile << "\n";
    return 0;
  }

  // Case 2: Compression Mode with args => ./a.out input.txt compressed.bin
  // codes.txt
  if (argc > 1) {
    inputFile = argv[1];
    if (argc >= 3)
      compressedFile = argv[2];
    if (argc >= 4)
      codesFile = argv[3];
  }

  // Case 3: Default compression when no arguments
  std::ifstream inFile(inputFile, std::ios::binary);
  if (!inFile) {
    std::cerr << "Error: Could not open input file: " << inputFile << "\n";
    return 1;
  }

  std::string text((std::istreambuf_iterator<char>(inFile)),
                   std::istreambuf_iterator<char>());
  inFile.close();

  std::cout << "Compressing...\n";
  huffman.compress(text, compressedFile, codesFile);

  std::cout << "Compression Complete.\n";
  std::cout << "Compressed File: " << compressedFile << "\n";
  std::cout << "Codes File: " << codesFile << "\n";

  return 0;
}
