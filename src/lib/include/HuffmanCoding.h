#pragma once

#include <bitset>
#include <cstdint>
#include <fstream>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

class HuffmanNode {
public:
  char data;
  int freq;
  HuffmanNode *left;
  HuffmanNode *right;

  HuffmanNode(char data, int freq);
};

class Compare {
public:
  inline bool operator()(HuffmanNode *a, HuffmanNode *b) {
    return a->freq > b->freq;
  }
};

class HuffmanCoding {
private:
  std::unordered_map<char, std::string> codes;
  std::unordered_map<char, int> freqMap;
  HuffmanNode *root;

  void buildFrequencyMap(const std::string &text);
  void buildHuffmanTree();
  void generateCodes(HuffmanNode *node, std::string currentCode);
  void writeCompressedFile(const std::string &encodedText,
                           const std::string &outputFile);
  void writeCodesToFile(const std::string &file);
  std::string encodeText(const std::string &text);

  std::string readFile(const std::string &filePath);

public:
  HuffmanCoding() : root(nullptr) {}

  void compress(const std::string &inputText,
                const std::string &outputBinaryFile,
                const std::string &codesFile);

  void compressFile(const std::string &inputFile,
                    const std::string &outputBinaryFile,
                    const std::string &codesFile);

  // New helpers for decompression
  void loadCodesFromFile(const std::string &file);
  void decompressFile(const std::string &inputBinaryFile,
                      const std::string &codesFile,
                      const std::string &outputFile);
};
