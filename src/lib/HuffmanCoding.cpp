#include "include/HuffmanCoding.h"
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>

namespace fs = std::filesystem;

HuffmanNode::HuffmanNode(char data, int freq) {
  this->data = data;
  this->freq = freq;
  this->left = nullptr;
  this->right = nullptr;
}

void HuffmanCoding::printProgressBar(size_t current, size_t total) {
  float progress = static_cast<float>(current) / total;
  int barWidth = 50;
  std::cout << "[";
  int pos = barWidth * progress;
  for (int i = 0; i < barWidth; ++i) {
    if (i < pos)
      std::cout << "=";
    else if (i == pos)
      std::cout << ">";
    else
      std::cout << " ";
  }
  std::cout << "] " << int(progress * 100.0) << " %\r";
  std::cout.flush();
}

void HuffmanCoding::buildFrequencyMap(const std::string &text) {
  freqMap.clear();
  for (unsigned char uch : text) {
    char ch = static_cast<char>(uch);
    freqMap[ch]++;
  }
}

void HuffmanCoding::buildHuffmanTree() {
  std::priority_queue<HuffmanNode *, std::vector<HuffmanNode *>, Compare> pq;

  for (auto &pair : freqMap) {
    pq.push(new HuffmanNode(pair.first, pair.second));
  }

  if (pq.empty()) {
    root = nullptr;
    return;
  }

  if (pq.size() == 1) {
    HuffmanNode *only = pq.top();
    pq.pop();
    HuffmanNode *parent = new HuffmanNode('\0', only->freq);
    parent->left = only;
    parent->right = new HuffmanNode('\0', 0);
    pq.push(parent);
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

void HuffmanCoding::writeDot(HuffmanNode *node, std::ostream &out) {
  if (!node)
    return;

  unsigned long long id = reinterpret_cast<unsigned long long>(node);

  if (node->left || node->right) {
    out << "  node" << id << " [label=\"" << node->freq
        << "\", shape=circle, style=filled, fillcolor=\"#e0e0e0\"];\n";
  } else {
    std::string charDisplay;
    if (node->data == '\n')
      charDisplay = "\\\\n";
    else if (node->data == '\r')
      charDisplay = "\\\\r";
    else if (node->data == '\t')
      charDisplay = "\\\\t";
    else if (node->data == ' ')
      charDisplay = "SPACE";
    else if (node->data == '\"')
      charDisplay = "\\\"";
    else
      charDisplay = std::string(1, node->data);

    out << "  node" << id << " [label=\"{" << charDisplay << " | " << node->freq
        << "}\", shape=record, style=filled, fillcolor=\"#b3e6ff\"];\n";
  }

  if (node->left) {
    unsigned long long leftId =
        reinterpret_cast<unsigned long long>(node->left);
    out << "  node" << id << " -> node" << leftId
        << " [label=\"0\", color=\"#ff3333\", penwidth=2.0];\n";
    writeDot(node->left, out);
  }
  if (node->right) {
    unsigned long long rightId =
        reinterpret_cast<unsigned long long>(node->right);
    out << "  node" << id << " -> node" << rightId
        << " [label=\"1\", color=\"#0066cc\", penwidth=2.0];\n";
    writeDot(node->right, out);
  }
}

void HuffmanCoding::saveTreeToDot(const std::string &filename) {
  if (!root)
    return;
  std::ofstream out(filename);
  out << "digraph HuffmanTree {\n";
  out << "  rankdir=TB;\n";
  out << "  node [fontname=\"Arial\"];\n";
  out << "  edge [fontname=\"Arial\"];\n";
  writeDot(root, out);
  out << "}\n";
  out.close();
  std::cout << "Tree Visualization saved to: " << filename << "\n";
}

void HuffmanCoding::generateCodes(HuffmanNode *node, std::string currentCode) {
  if (!node)
    return;

  if (!node->left && !node->right) {
    if (currentCode.empty()) {
      codes[node->data] = "0";
    } else {
      codes[node->data] = currentCode;
    }
    return;
  }

  if (node->left)
    generateCodes(node->left, currentCode + "0");
  if (node->right)
    generateCodes(node->right, currentCode + "1");
}

std::string HuffmanCoding::encodeText(const std::string &text) {
  std::string encoded;
  encoded.reserve(text.size());
  size_t total = text.size();
  size_t current = 0;

  std::cout << "Encoding:\n";

  for (unsigned char uch : text) {
    char ch = static_cast<char>(uch);
    auto it = codes.find(ch);
    if (it != codes.end()) {
      encoded += it->second;
    } else {
      std::cerr << "Warning: character with no code found during encoding\n";
    }

    current++;
    if (current % (total / 50 + 1) == 0) {
      printProgressBar(current, total);
    }
  }
  printProgressBar(total, total);
  std::cout << std::endl;
  return encoded;
}

void HuffmanCoding::writeCompressedFile(const std::string &encodedText,
                                        const std::string &outputFile) {
  std::ofstream out(outputFile, std::ios::binary);
  if (!out) {
    throw std::runtime_error("Unable to open output binary file for writing");
  }

  uint64_t bitCount = encodedText.size();
  out.write(reinterpret_cast<const char *>(&bitCount), sizeof(bitCount));

  std::string byte = "";
  for (char bit : encodedText) {
    byte += bit;
    if (byte.size() == 8) {
      std::bitset<8> b(byte);
      unsigned long val = b.to_ulong();
      unsigned char c = static_cast<unsigned char>(val & 0xFF);
      out.put(static_cast<char>(c));
      byte.clear();
    }
  }

  if (!byte.empty()) {
    while (byte.size() < 8)
      byte += "0";
    std::bitset<8> b(byte);
    unsigned long val = b.to_ulong();
    unsigned char c = static_cast<unsigned char>(val & 0xFF);
    out.put(static_cast<char>(c));
  }

  out.close();
}

void HuffmanCoding::writeCodesToFile(const std::string &file) {
  std::ofstream out(file);
  if (!out) {
    throw std::runtime_error("Unable to open codes file for writing");
  }
  for (auto &pair : codes) {
    out << static_cast<int>(static_cast<unsigned char>(pair.first)) << " "
        << pair.second << "\n";
  }
  out.close();
}

void HuffmanCoding::compress(const std::string &inputText,
                             const std::string &outputBinaryFile,
                             const std::string &codesFile) {
  codes.clear();
  freqMap.clear();
  root = nullptr;

  buildFrequencyMap(inputText);
  buildHuffmanTree();
  if (!root) {
    throw std::runtime_error("No data to compress (empty input).");
  }

  saveTreeToDot(outputBinaryFile + ".dot");

  generateCodes(root, "");

  std::string encoded = encodeText(inputText);
  std::cout << "Writing to File...\n";
  writeCompressedFile(encoded, outputBinaryFile);
  writeCodesToFile(codesFile);
}

std::string HuffmanCoding::readFile(const std::string &filePath) {
  std::ifstream in(filePath, std::ios::binary);
  if (!in) {
    throw std::runtime_error("Unable to open input file for reading");
  }
  return std::string((std::istreambuf_iterator<char>(in)),
                     std::istreambuf_iterator<char>());
}

void HuffmanCoding::compressFile(const std::string &inputFile,
                                 const std::string &outputBinaryFile,
                                 const std::string &codesFile) {
  std::cout << "\n--- Starting Compression Benchmark ---\n";
  auto start = std::chrono::high_resolution_clock::now();

  std::string text = readFile(inputFile);
  compress(text, outputBinaryFile, codesFile);

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;

  size_t originalSize = 0;
  size_t compressedSize = 0;

  try {
    originalSize = fs::file_size(inputFile);
    compressedSize = fs::file_size(outputBinaryFile);
  } catch (...) {
  }

  double savings = 0.0;
  if (originalSize > 0) {
    savings =
        (1.0 - (static_cast<double>(compressedSize) / originalSize)) * 100.0;
  }

  std::cout << "--------------------------------------\n";
  std::cout << "       COMPRESSION REPORT             \n";
  std::cout << "--------------------------------------\n";
  std::cout << "Time Taken      : " << std::fixed << std::setprecision(4)
            << elapsed.count() << " s\n";
  std::cout << "Original Size   : " << originalSize << " bytes\n";
  std::cout << "Compressed Size : " << compressedSize << " bytes\n";
  std::cout << "Space Saved     : " << std::fixed << std::setprecision(2)
            << savings << " %\n";
  std::cout << "--------------------------------------\n";
}

void HuffmanCoding::loadCodesFromFile(const std::string &file) {
  std::ifstream in(file);
  if (!in) {
    throw std::runtime_error("Unable to open codes file for reading");
  }

  codes.clear();
  int charVal;
  std::string code;
  while (in >> charVal >> code) {
    char ch = static_cast<char>(charVal);
    codes[ch] = code;
  }
  in.close();
}

void HuffmanCoding::decompress(const std::string &inputBinaryFile,
                               const std::string &codesFile,
                               const std::string &outputFile) {
  loadCodesFromFile(codesFile);

  root = new HuffmanNode('\0', 0);
  for (auto &pair : codes) {
    HuffmanNode *curr = root;
    for (char bit : pair.second) {
      if (bit == '0') {
        if (!curr->left)
          curr->left = new HuffmanNode('\0', 0);
        curr = curr->left;
      } else {
        if (!curr->right)
          curr->right = new HuffmanNode('\0', 0);
        curr = curr->right;
      }
    }
    curr->data = pair.first;
  }

  std::ifstream in(inputBinaryFile, std::ios::binary);
  if (!in) {
    throw std::runtime_error(
        "Unable to open compressed binary file for reading");
  }

  uint64_t validBitCount = 0;
  in.read(reinterpret_cast<char *>(&validBitCount), sizeof(validBitCount));
  if (!in) {
    throw std::runtime_error(
        "Failed to read bit-length header from compressed file");
  }

  std::string bitString;
  char byte;
  while (in.get(byte)) {
    std::bitset<8> bits(static_cast<unsigned char>(byte));
    bitString += bits.to_string();
  }
  in.close();

  if (validBitCount > bitString.size()) {
    throw std::runtime_error("Invalid bit count in compressed file header");
  }
  bitString.resize(static_cast<size_t>(validBitCount));

  std::ofstream out(outputFile, std::ios::binary);
  if (!out) {
    throw std::runtime_error(
        "Unable to open output file for writing during decompression");
  }

  std::cout << "Decoding:\n";
  size_t totalBits = bitString.size();
  size_t currentBit = 0;

  HuffmanNode *curr = root;
  for (char bit : bitString) {
    if (!curr) {
      throw std::runtime_error(
          "Tree traversal encountered null node during decompression");
    }
    curr = (bit == '0') ? curr->left : curr->right;
    if (!curr) {
      throw std::runtime_error("Invalid traversal (null child) while decoding "
                               "— codes or compressed data mismatch");
    }
    if (!curr->left && !curr->right) {
      out.put(curr->data);
      curr = root;
    }

    currentBit++;
    if (currentBit % (totalBits / 50 + 1) == 0) {
      printProgressBar(currentBit, totalBits);
    }
  }
  printProgressBar(totalBits, totalBits);
  std::cout << std::endl;

  out.close();
}

void HuffmanCoding::decompressFile(const std::string &inputBinaryFile,
                                   const std::string &codesFile,
                                   const std::string &outputFile) {
  std::cout << "\n--- Starting Decompression Benchmark ---\n";

  auto start = std::chrono::high_resolution_clock::now();

  decompress(inputBinaryFile, codesFile, outputFile);

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;

  std::cout << "--------------------------------------\n";
  std::cout << "      DECOMPRESSION REPORT            \n";
  std::cout << "--------------------------------------\n";
  std::cout << "Time Taken      : " << std::fixed << std::setprecision(4)
            << elapsed.count() << " s\n";
  std::cout << "--------------------------------------\n";
}
