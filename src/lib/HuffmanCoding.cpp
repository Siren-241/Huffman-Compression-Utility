#include "include/HuffmanCoding.h"
#include <iostream>
#include <iterator>
#include <limits>

// Constructor
HuffmanNode::HuffmanNode(char data, int freq) {
  this->data = data;
  this->freq = freq;
  this->left = nullptr;
  this->right = nullptr;
}

// Build frequency map from text
void HuffmanCoding::buildFrequencyMap(const std::string &text) {
  freqMap.clear();
  for (unsigned char uch : text) {
    char ch = static_cast<char>(uch);
    freqMap[ch]++;
  }
}

// Build Huffman Tree
void HuffmanCoding::buildHuffmanTree() {
  // clean existing tree pointer (memory leak avoidance omitted for brevity)
  std::priority_queue<HuffmanNode *, std::vector<HuffmanNode *>, Compare> pq;

  for (auto &pair : freqMap) {
    pq.push(new HuffmanNode(pair.first, pair.second));
  }

  if (pq.empty()) {
    root = nullptr;
    return;
  }

  // If only one distinct character, ensure tree produces at least one bit code.
  if (pq.size() == 1) {
    HuffmanNode *only = pq.top();
    pq.pop();
    // Create a dummy parent so we have left/right structure
    HuffmanNode *parent = new HuffmanNode('\0', only->freq);
    parent->left = only;
    parent->right = new HuffmanNode('\0', 0); // empty leaf
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

// Generate Huffman Codes (DFS)
void HuffmanCoding::generateCodes(HuffmanNode *node, std::string currentCode) {
  if (!node)
    return;

  // Leaf node
  if (!node->left && !node->right) {
    // Edge case: single-character input -> currentCode may be empty
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

// Encode text using Huffman codes
std::string HuffmanCoding::encodeText(const std::string &text) {
  std::string encoded;
  encoded.reserve(text.size()); // rough reserve
  for (unsigned char uch : text) {
    char ch = static_cast<char>(uch);
    auto it = codes.find(ch);
    if (it != codes.end()) {
      encoded += it->second;
    } else {
      // Shouldn't happen if codes built from same text, but guard anyway
      std::cerr << "Warning: character with no code found during encoding\n";
    }
  }
  return encoded;
}

// Write encoded binary output file
// Format: [8 bytes uint64_t = number of valid bits][raw bytes...]
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

// Save codes to file
void HuffmanCoding::writeCodesToFile(const std::string &file) {
  std::ofstream out(file);
  if (!out) {
    throw std::runtime_error("Unable to open codes file for writing");
  }
  for (auto &pair : codes) {
    // write numeric value of char to avoid whitespace issues (use int)
    out << static_cast<int>(static_cast<unsigned char>(pair.first)) << " "
        << pair.second << "\n";
  }
  out.close();
}

// Main compression function
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
  generateCodes(root, "");

  std::string encoded = encodeText(inputText);
  writeCompressedFile(encoded, outputBinaryFile);
  writeCodesToFile(codesFile);
}

// Read entire file as string
std::string HuffmanCoding::readFile(const std::string &filePath) {
  std::ifstream in(filePath, std::ios::binary);
  if (!in) {
    throw std::runtime_error("Unable to open input file for reading");
  }
  return std::string((std::istreambuf_iterator<char>(in)),
                     std::istreambuf_iterator<char>());
}

// Compress a text file directly
void HuffmanCoding::compressFile(const std::string &inputFile,
                                 const std::string &outputBinaryFile,
                                 const std::string &codesFile) {
  std::string text = readFile(inputFile);
  compress(text, outputBinaryFile, codesFile);
}

// Load codes back from codes.txt
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

// Decompress binary file to text
void HuffmanCoding::decompressFile(const std::string &inputBinaryFile,
                                   const std::string &codesFile,
                                   const std::string &outputFile) {

  loadCodesFromFile(codesFile);

  // Rebuild Huffman Tree from codes
  root = new HuffmanNode('\0', 0);
  for (auto &pair : codes) {
    HuffmanNode *curr = root;
    for (char bit : pair.second) {
      if (bit == '0') {
        if (!curr->left)
          curr->left = new HuffmanNode('\0', 0);
        curr = curr->left;
      } else { // '1'
        if (!curr->right)
          curr->right = new HuffmanNode('\0', 0);
        curr = curr->right;
      }
    }
    curr->data = pair.first;
  }

  // Read compressed binary file and the valid bit count
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

  // Truncate to the number of valid bits (drop padding)
  if (validBitCount > bitString.size()) {
    throw std::runtime_error("Invalid bit count in compressed file header");
  }
  bitString.resize(static_cast<size_t>(validBitCount));

  // Decode bit string
  std::ofstream out(outputFile, std::ios::binary);
  if (!out) {
    throw std::runtime_error(
        "Unable to open output file for writing during decompression");
  }

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
  }

  out.close();
}
