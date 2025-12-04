#include "lib/include/HuffmanCoding.h"
#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

void showUsage(const std::string &programName) {
  std::cout << "Usage:\n";
  std::cout << "  Compression:   " << programName
            << " -c <input_file> <output_bin_path> <output_codes_path>\n";
  std::cout << "  Decompression: " << programName
            << " -d <input_bin_path> <input_codes_path> <output_file_path>\n";
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    showUsage(argv[0]);
    return 1;
  }

  std::string mode = argv[1];
  HuffmanCoding coder;

  if (mode == "-c") {
    if (argc < 5) {
      std::cerr << "Error: Compression requires input file, output binary "
                   "path, and output codes path.\n";
      showUsage(argv[0]);
      return 1;
    }

    std::string inputFile = argv[2];
    std::string outputBin = argv[3];
    std::string outputCodes = argv[4];

    if (!fs::exists(inputFile)) {
      std::cerr << "Error: Input file not found: " << inputFile << std::endl;
      return 1;
    }

    std::cout << "Target Input: " << inputFile << "\n";
    std::cout << "Output Bin:   " << outputBin << "\n";
    std::cout << "Output Codes: " << outputCodes << "\n";

    try {
      coder.compressFile(inputFile, outputBin, outputCodes);
    } catch (const std::exception &e) {
      std::cerr << "\nError during compression: " << e.what() << std::endl;
      return 1;
    }

  } else if (mode == "-d") {
    if (argc < 5) {
      std::cerr << "Error: Decompression requires input binary path, input "
                   "codes path, and output file path.\n";
      showUsage(argv[0]);
      return 1;
    }

    std::string inputBin = argv[2];
    std::string inputCodes = argv[3];
    std::string outputFile = argv[4];

    if (!fs::exists(inputBin)) {
      std::cerr << "Error: Input binary file not found: " << inputBin
                << std::endl;
      return 1;
    }
    if (!fs::exists(inputCodes)) {
      std::cerr << "Error: Input codes file not found: " << inputCodes
                << std::endl;
      return 1;
    }

    std::cout << "Input Binary: " << inputBin << "\n";
    std::cout << "Input Codes:  " << inputCodes << "\n";
    std::cout << "Output File:  " << outputFile << "\n";

    try {
      coder.decompressFile(inputBin, inputCodes, outputFile);
    } catch (const std::exception &e) {
      std::cerr << "\nError during decompression: " << e.what() << std::endl;
      return 1;
    }

  } else {
    std::cerr << "Unknown mode: " << mode << "\n";
    showUsage(argv[0]);
    return 1;
  }

  return 0;
}
