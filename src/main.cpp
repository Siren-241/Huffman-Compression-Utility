#include "lib/include/HuffmanCoding.h"
#include <filesystem>
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0]
              << " <input_file> [compressed_file] [codes_file]\n";
    return 1;
  }

  std::string inputFile = argv[1];
  std::string outputBinary = (argc >= 3) ? argv[2] : "../output/compressed.bin";
  std::string outputCodes = (argc >= 4) ? argv[3] : "../output/codes.txt";

  std::filesystem::create_directories(
      std::filesystem::path(outputBinary).parent_path());
  std::filesystem::create_directories(
      std::filesystem::path(outputCodes).parent_path());

  std::ifstream inFile(inputFile, std::ios::binary);
  if (!inFile) {
    std::cerr << "Unable to open file: " << inputFile << "\n";
    return 1;
  }

  std::string text((std::istreambuf_iterator<char>(inFile)),
                   std::istreambuf_iterator<char>());
  inFile.close();

  HuffmanCoding compressor;
  compressor.compress(text, outputBinary, outputCodes);

  std::cout << "Compression complete.\n";
  std::cout << "Compressed file: " << outputBinary << "\n";
  std::cout << "Huffman codes: " << outputCodes << "\n";
  return 0;
}
