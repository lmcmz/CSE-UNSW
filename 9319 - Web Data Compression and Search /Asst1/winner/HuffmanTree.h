#pragma once

#include <array>

const size_t INPUT_BUFFER_SIZE = 100000;

// Both the decoder and encoder construct a Huffman tree, which is then used to
// populate a lookup table.

struct HuffmanNode
{
  uint64_t code;
  uint32_t count;
  uint16_t index;
  int16_t leftChild;
  int16_t rightChild;
  uint8_t codeLength;
  uint8_t character;
  bool leaf;
};

extern uint32_t MakeHuffmanTree(std::array<HuffmanNode, 511>& nodes,
  const std::array<uint32_t, 256>& charCounts);
