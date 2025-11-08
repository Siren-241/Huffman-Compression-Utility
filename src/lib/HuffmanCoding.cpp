#include "include/HuffmanCoding.h"
#include <iostream>

HuffmanNode::HuffmanNode(char data, int freq) {
  this->data = data;
  this->freq = freq;
  this->left = nullptr;
  this->right = nullptr;
}

void HuffmanCoding::buildFrequencyMap(const std::string &text) {
  for (char ch : text) {
    freqMap[ch]++;
  }
}

void HuffmanCoding::buildHuffmanTree() {
  std::priority_queue<HuffmanNode *, std::vector<HuffmanNode *>, Compare> pq;
  for (auto pair : freqMap) {
    pq.push(new HuffmanNode(pair.first, pair.second));
  }

  while (pq.size() > 1) {
    HuffmanNode *left = pq.top();
    pq.pop();
    HuffmanNode *right = pq.top();
    pq.pop();

    HuffmanNode *merged = new HuffmanNode('\0', left->freq + right->freq);
    merged->left = left;
    merged->right = right;

    pq.push(merged);
  }

  root = pq.top();
}

void HuffmanCoding::generateCodes(HuffmanNode *node, std::string currentCode) {
  if (!node)
    return;

  if (!node->left && !node->right) {
    codes[node->data] = currentCode;
  }

  generateCodes(node->left, currentCode + "0");
  generateCodes(node->right, currentCode + "1");
}

std::string HuffmanCoding::encodeText(const std::string &text) {
  std::string encoded = "";
  for (char ch : text) {
    encoded += codes[ch];
  }
  return encoded;
}

void HuffmanCoding::writeCompressedFile(const std::string &encodedText,
                                        const std::string &outputFile) {
  std::ofstream out(outputFile, std::ios::binary);
  std::string byte = "";

  for (char bit : encodedText) {
    byte += bit;
    if (byte.size() == 8) {
      std::bitset<8> b(byte);
      out.put(static_cast<char>(b.to_ulong()));
      byte = "";
    }
  }

  if (!byte.empty()) {
    while (byte.size() < 8)
      byte += "0";
    std::bitset<8> b(byte);
    out.put(static_cast<char>(b.to_ulong()));
  }

  out.close();
}

void HuffmanCoding::writeCodesToFile(const std::string &file) {
  std::ofstream out(file);
  for (auto pair : codes) {
    out << pair.first << " " << pair.second << "\n";
  }
  out.close();
}

void HuffmanCoding::compress(const std::string &inputText,
                             const std::string &outputBinaryFile,
                             const std::string &codesFile) {
  buildFrequencyMap(inputText);
  buildHuffmanTree();
  generateCodes(root, "");

  std::string encoded = encodeText(inputText);
  writeCompressedFile(encoded, outputBinaryFile);
  writeCodesToFile(codesFile);
}

std::string HuffmanCoding::readFile(const std::string &filePath) {
  std::ifstream in(filePath, std::ios::binary);
  return std::string((std::istreambuf_iterator<char>(in)),
                     std::istreambuf_iterator<char>());
}

void HuffmanCoding::compressFile(const std::string &inputFile,
                                 const std::string &outputBinaryFile,
                                 const std::string &codesFile) {
  std::string text = readFile(inputFile);
  compress(text, outputBinaryFile, codesFile);
}
