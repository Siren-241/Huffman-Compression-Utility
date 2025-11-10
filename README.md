# Huffman Coding File Compression and Decompression

## Project Structure

```
project/
│
├── testFiles/
│   └── input.txt                   # Place the text file you want to compress here
│
├── output/
│   ├── compressed.bin              # Generated compressed binary file
│   ├── codes.txt                   # Huffman codes used for compression
│   └── decompressed.txt            # Output after decompression
│
├── src/
|   ├── main.cpp                     # Entry point with CLI + default behavior
|   └── lib/
│       ├── HuffmanCoding.cpp        # Method implementations
│       └── include/
│           └── HuffmanCoding.h      # Class definitions and method declarations
```

---

## Dependencies

- **C++17 or above**
- Standard Library (no external libraries required)

---

## Build Instructions

1. Make a folder called "build" at root of project
2. Open it in terminal
3. Run the following commands:
   `cmake ../`

   `cmake --build .`

   `./Compressor --help`

**One after the other**

_If it is showing error like `cmake not installed` or something similar, give the error to chatgpt and follow installation instructions_
