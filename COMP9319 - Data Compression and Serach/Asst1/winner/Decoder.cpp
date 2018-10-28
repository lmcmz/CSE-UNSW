#include "Decoder.h"
#include "HuffmanTree.h"
#include <fcntl.h>
#include <iostream>
#include <queue>
#include <sstream>
#include <unistd.h>

std::unique_ptr<uint32_t[]> KMPFailureFunction(const uint8_t* pattern, uint32_t length)
{
  auto failure = std::make_unique<uint32_t[]>(length);
  uint32_t i;
  for (i = 0; i < length; ++i)
	failure[i] = 0;

  i = 1;
  uint32_t j = 0;
  while (i < length)
  {
	if (pattern[i] == pattern[j])
	{
	  ++j;
	  failure[i++] = j;
	}
	else if (j > 0)
	{
	  j = failure[j - 1];
	}
	else
	{
	  ++i;
	}
  }
#ifdef _DEBUG
  std::cout << "KMP Failure Function:" << std::endl;
  for (i = 0; i < length; ++i)
	std::cout << '\t' << (char)pattern[i] << ": " << failure[i] << std::endl;
#endif
  return failure;
}

HuffmanDecoder::HuffmanDecoder(const char* inputName)
  : _inputName(inputName)
{
  _fd = open(inputName, O_RDONLY);
  if (_fd < 0)
  {
	std::stringstream ss;
	ss << "Could not open file " << inputName;
	throw std::runtime_error(ss.str());
  }
  // The first 1024 bytes of the compressed file are a table containing the
  // frequency of each of the 256 possible byte values in the original file.
  std::array<uint32_t, 256> charCounts;
  int64_t dataSize = 0;
  if (read(_fd, charCounts.data(), 1024) == 1024)
	dataSize = lseek(_fd, 0, SEEK_END) - 1024;
  if (dataSize <= 0)
  {
	std::stringstream ss;
	ss << "Input file " << inputName << " has an incomplete header.";
	throw std::runtime_error(ss.str());
  }
  // Calculate the total size of the uncompressed file by adding count for each
  // byte value.
  _uncompressedSize = 0;
  for (size_t count : charCounts)
	_uncompressedSize += count;

  _inputBufferSize = std::min<size_t>(dataSize, INPUT_BUFFER_SIZE);
  _inputBuffer = std::make_unique<uint8_t[]>(_inputBufferSize);
  MakeDecodingTable(charCounts);
  // Size the output buffer to have enough room even if the input consists entirely of the
  // shortest Huffman code. This means that we can decode the entire input buffer without
  // having to check for a full output buffer.
  _outputBufferSize = std::min(_uncompressedSize, (_inputBufferSize * 8) / _shortestCodeLength) + 8;
  _outputBuffer = std::make_unique<uint8_t[]>(_outputBufferSize);
}

HuffmanDecoder::~HuffmanDecoder()
{
  if (_fd > 0)
	close(_fd);
}

void HuffmanDecoder::Decode(const char* outputName)
{
  int outputFile = open(outputName, O_WRONLY|O_CREAT|O_TRUNC, 0666);
  if (outputFile < 0)
  {
	std::stringstream ss;
	ss << "Could not open file " << outputName;
	throw std::runtime_error(ss.str());
  }

  DecodingPath dummy;
  dummy.destination = 0;
  _position = &dummy;

  lseek(_fd, 1024, SEEK_SET);
  // DecodeChunk updates _remainingSize.
  _remainingSize = _uncompressedSize;
  while (_remainingSize > 0)
  {
	size_t bytesDecoded = DecodeChunk();
	write(outputFile, _outputBuffer.get(), bytesDecoded);
  }

  close(outputFile);
}

// Count the number of times that the given patten occurs in the encoded file.

size_t HuffmanDecoder::Count(const uint8_t* pattern, uint32_t patternLength)
{
  DecodingPath dummy;
  dummy.destination = 0;
  _position = &dummy;

  lseek(_fd, 1024, SEEK_SET);
  std::unique_ptr<uint32_t[]> failure = KMPFailureFunction(pattern, patternLength);
  size_t matchCount = 0;
  uint32_t patternPos = 0;
  uint32_t patternLengthMinusOne = patternLength - 1;
  _remainingSize = _uncompressedSize;
  // DecodeChunk updates _remainingSize.
  while (_remainingSize > 0)
  {
	// Simply decoded the file a chunk at a time and search each chunk.
	size_t bytesDecoded = DecodeChunk();
	// Search using the KMP algorithm. As well as having the optimimum worst-case
	// performance, it has the advantage that it never has to go backwards in the
	// string. This makes it easier to use when processing the file in chunks.
	const uint8_t* currentChar = _outputBuffer.get();
	const uint8_t* outputEnd = currentChar + bytesDecoded;
	while (currentChar != outputEnd)
	{
	  if (*currentChar == pattern[patternPos])
	  {
		if (patternPos == patternLengthMinusOne)
		{
		  ++matchCount;
		  patternPos = failure[patternPos];
		}
		else
		{
		  ++patternPos;
		}
		++currentChar;
	  }
	  else if (patternPos > 0)
	  {
		patternPos = failure[patternPos - 1];
	  }
	  else
	  {
		++currentChar;
	  }
	}
  }

  // Leave the file open so that we can search again. The HuffmanDecoder
  // destructor will close the file.
  return matchCount;
}

// The simplest way to decode a Huffman file is by walking through the Huffman
// tree. Starting at the root node of the tree, you read the bits from the
// encoded file one by one. You move to the node's left or right child depending
// on whether the bit is a 0 or 1, continuing recursively until you reach a leaf
// node. You then output the leaf's character and return to the root. Because
// this method only processes a bit at a time, it is slow.

// This decoder uses a 2-dimensional lookup table to simulate walking a Huffman
// tree while processing the encoded data in 4-bit "nibbles". The idea is that
// from any given node in the tree, the decoder can follow 16 possible paths,
// depending on the 16 possible values of the nibble. The path may reach leaf
// nodes and return to the root multiple times, outputting a character each
// time.

// Because the tree can contain at most 511 nodes, and there are 16 possible
// nibbles, a 511 x 16 lookup table is sufficient to handle all possible states
// in the largest possible tree. The table entries are represented by the
// DecodingPath struct:

//		struct DecodingPath
//		{
//		  uint16_t destination;
//		  uint8_t outputLength;
//		  uint8_t output[4];
//		};

// The DecodingPath struct is padded to 8 bytes, so the table occupies at most
// 65,400 bytes. I experimented with an implementation that decoded a byte at
// a time, but it was slower. This was presumably because it required a much
// larger lookup table, which didn't fit in the processor's level 1 or level 2
// caches.

// The decoder keeps track of its current position in the tree, which is
// initially the root. Every time it reads a nibble, it uses its position
// and the nibble as indices to look up the DecodingPath in the table.
// This tells the decoder which byte(s), if any, to output, and its new
// position. It continues this process until the entire file has been decoded.

void HuffmanDecoder::MakeDecodingTable(const std::array<uint32_t, 256>& charCounts)
{
  std::array<HuffmanNode, 511> nodes;
  uint32_t nodeCount = MakeHuffmanTree(nodes, charCounts);
  // Conceptually the decoding table is a 2-dimensional array, with one
  // dimension being the starting node, and the other being the input
  // nibble. However, implementing it as a 1-dimensional array improved
  // performance slightly.
  _decodingTable = std::make_unique<DecodingPath[]>(nodeCount * 16);
  // Iterate over every possible starting position in the tree.
  for (uint32_t startingPos = 0; startingPos < nodeCount; ++startingPos)
  {
	// Iterate through each of the 16 possible nibble values.
	for (uint32_t nibble = 0; nibble < 16; ++nibble)
	{
	  const HuffmanNode* node = &nodes[startingPos];
	  // Trace the path that the decoder would follow from this starting node
	  // given this nibble of input.
	  DecodingPath& path = _decodingTable[(startingPos * 16) + nibble];
	  path.outputLength = 0;
	  uint8_t bitPattern = nibble;
	  for (uint32_t bit = 0; bit < 4; ++bit)
	  {
		if (bitPattern & 1)
		  node = &nodes[node->rightChild];
		else
		  node = &nodes[node->leftChild];
		if (node->index == 0)
		{
		  // Index 0 is the root node. If we get here, it means that the current
		  // bit pattern is not a valid Huffman code.
		  break;
		}
		if (node->leaf)
		{
		  // We need to output a character every time we reach a leaf.
		  path.output[path.outputLength] = node->character;
		  ++path.outputLength;
		  // Loop back to the root.
		  node = &nodes[0];
		}
		bitPattern >>= 1;
	  }
	  // Multiply the index by 16 to simulate a 2-dimensional table.
	  path.destination = node->index * 16;
	}
  }

  // Find the length of the shortest Huffman code.
  // Node 0 is always the root of the tree.
  nodes[0].codeLength = 0;
  std::queue<uint16_t> q;
  q.push(0);
  for (;;)
  {
	const HuffmanNode& node = nodes[q.front()];
	// The first leaf node that we find will be the closest one to the root,
	// and will therefore have the shortest Huffman code.
	if (node.leaf)
	{
	  _shortestCodeLength = node.codeLength;
	  break;
	}
	q.pop();
	if (node.leftChild != 0)
	{
	  nodes[node.leftChild].codeLength = node.codeLength + 1;
	  q.push(node.leftChild);
	}
	if (node.rightChild != 0)
	{
	  nodes[node.rightChild].codeLength = node.codeLength + 1;
	  q.push(node.rightChild);
	}
  }
#ifdef _DEBUG
  std::cout << "Shortest code length = " << _shortestCodeLength << std::endl;
#endif
}

size_t HuffmanDecoder::DecodeChunk()
{
  int bytesRead = read(_fd, _inputBuffer.get(), _inputBufferSize);
  if (bytesRead < 1)
  {
	std::stringstream ss;
	ss << "Input file " << _inputName << " is incomplete.";
	throw std::runtime_error(ss.str());
  }
  const uint8_t* inputPos = _inputBuffer.get();
  const uint8_t* inputEnd = inputPos + bytesRead;
  uint8_t* outputPos = _outputBuffer.get();
  const DecodingPath* table = _decodingTable.get();
  const DecodingPath* path = _position;
  // The maximum number of bytes that we can possibly decode while processing a
  // single nibble is either 1, 2, or 4 depending on the length of the shortest
  // Huffman code. We use separate inner loops for each case to avoid
  // unnecessary comparisons in the 1 and 2 byte cases and improve performance
  // slightly.
  if (_shortestCodeLength < 4)
  {
	if (_shortestCodeLength == 1)
	{
	  // Each nibble may produce up to 4 bytes of output.
	  do
	  {
		auto input = *inputPos;
		// nibble 0
		path = &table[path->destination + (input & 0x0F)];
		uint8_t length = path->outputLength;
		if (length > 0)
		{
		  *outputPos = path->output[0];
		  ++outputPos;
		  if (length > 1)
		  {
			*outputPos = path->output[1];
			++outputPos;
			if (length > 2)
			{
			  *outputPos = path->output[2];
			  ++outputPos;
			  if (length > 3)
			  {
				*outputPos = path->output[3];
				++outputPos;
			  }
			}
		  }
		}
		// nibble 1
		input >>= 4;
		path = &table[path->destination + input];
		length = path->outputLength;
		if (length > 0)
		{
		  *outputPos = path->output[0];
		  ++outputPos;
		  if (length > 1)
		  {
			*outputPos = path->output[1];
			++outputPos;
			if (length > 2)
			{
			  *outputPos = path->output[2];
			  ++outputPos;
			  if (length > 3)
			  {
				*outputPos = path->output[3];
				++outputPos;
			  }
			}
		  }
		}
	  } while (++inputPos != inputEnd);
	}
	else
	{
	  // Each nibble may produce up to 2 bytes of output.
	  do
	  {
		auto input = *inputPos;
		// nibble 0
		path = &table[path->destination + (input & 0x0F)];
		uint8_t length = path->outputLength;
		if (length)
		{
		  *outputPos = path->output[0];
		  ++outputPos;
		  if (length > 1)
		  {
			*outputPos = path->output[1];
			++outputPos;
		  }
		}
		// nibble 1
		input >>= 4;
		path = &table[path->destination + input];
		length = path->outputLength;
		if (length)
		{
		  *outputPos = path->output[0];
		  ++outputPos;
		  if (length > 1)
		  {
			*outputPos = path->output[1];
			++outputPos;
		  }
		}
	  } while (++inputPos != inputEnd);
	}
  }
  else
  {
	// Each nibble can produce at most 1 byte of output.
	do
	{
	  auto input = *inputPos;
	  // nibble 0
	  path = &table[path->destination + (input & 0x0F)];
	  if (path->outputLength)
	  {
		*outputPos = path->output[0];
		++outputPos;
	  }
	  // nibble 1
	  input >>= 4;
	  path = &table[path->destination + input];
	  if (path->outputLength)
	  {
		*outputPos = path->output[0];
		++outputPos;
	  }
	} while (++inputPos != inputEnd);
  }
  _position = path;
  // Because the file has to store whole bytes, as many as 7 bits at the end
  // may not be real data. The decoder may therefore produce a small amount of
  // spurious output at the end of the file. We can simply ignore any bytes in
  // excess of what we were expecting.
  size_t bytesDecoded = std::min<size_t>(outputPos - _outputBuffer.get(), _remainingSize);
  _remainingSize -= bytesDecoded;
  return bytesDecoded;
}
