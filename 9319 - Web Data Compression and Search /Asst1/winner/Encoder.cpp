#include "HuffmanTree.h"
#include <memory>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <stack>
#include <unistd.h>

// Encoding represents a Huffman code and the number of bits in the code.

struct Encoding
{
  uint64_t code;
  // There's no point making this smaller since the
  // struct will be padded to 8 bytes anyway.
  uint32_t codeLength;
};

void MakeCodeTable(std::array<Encoding, 256>& codes, std::array<uint32_t, 256>& charCounts)
{
  // Construct a Huffman tree from the input file's character counts.
  std::array<HuffmanNode, 511> nodes;
  MakeHuffmanTree(nodes, charCounts);
  // Visit every node in the tree, and store the Huffman code for the path from
  // the root to the node. When we get to a leaf node, store its Huffman code
  // and code length in the lookup table.
  std::stack<uint16_t> stack;
  stack.push(0);
  nodes[0].code = 0;
  nodes[0].codeLength = 0;
#ifdef _DEBUG
  int maxLen = 0;
#endif
  while (!stack.empty())
  {
	const HuffmanNode& node = nodes[stack.top()];
	stack.pop();
	if (node.leaf)
	{
	  // This is a leaf node, so we need to store its encoding in the lookup
	  // table. The byte value represented by the node is used as a direct
	  // index into the table.
	  Encoding& encoding = codes[static_cast<uint32_t>(node.character)];
	  encoding.code = node.code;
	  encoding.codeLength = node.codeLength;
#ifdef _DEBUG
	  if (node.codeLength > maxLen)
		maxLen = node.codeLength;
	  std::string s;
	  uint64_t bits = node.code;
	  for (uint32_t i = 0; i < node.codeLength; ++i)
	  {
		if (bits & 1)
		  s += "1";
		else
		  s += "0";
		bits >>= 1;
	  }
	  std::cout << node.character << " = " << s << std::endl;
#endif
	}
	else
	{
	  // Set the Huffman codes and code lengths on this node's children, if any,
	  // and then add them to the stack so that we will visit them later.
	  if (node.leftChild != 0)
	  {
		HuffmanNode& left = nodes[node.leftChild];
		left.code = node.code;
		left.codeLength = node.codeLength + 1;
		stack.push(node.leftChild);
	  }
	  if (node.rightChild != 0)
	  {
		HuffmanNode& right = nodes[node.rightChild];
		right.code = node.code | ((uint64_t)1 << node.codeLength);
		right.codeLength = node.codeLength + 1;
		stack.push(node.rightChild);
	  }
	}
  }
#ifdef _DEBUG
  std::cout << "size of HuffmanNode = " << sizeof(HuffmanNode) << std::endl;
  std::cout << "size of Encoding = " << sizeof(Encoding) << std::endl;
  std::cout << "Max code length = " << maxLen << std::endl;
#endif
}

void HuffmanEncode(const char* inputName, const char* outputName)
{
  int inputFile = open(inputName, O_RDONLY);
  if (inputFile < 0)
  {
	std::stringstream ss;
	ss << "Could not open file " << inputName;
	throw std::runtime_error(ss.str());
  }
  // Determine the size of the file.
  size_t fileSize = lseek(inputFile, 0, SEEK_END);
  if (fileSize == 0)
	throw std::runtime_error("Input file is empty.");
  lseek(inputFile, 0, SEEK_SET);
  size_t bufferSize = std::min(fileSize, INPUT_BUFFER_SIZE);
  // Read the file and count the number of times that each character appears.
  // If the file is too big to fit in our buffer then we need multiple reads.
  std::array<uint32_t, 256> charCounts;
  charCounts.fill(0);
  size_t totalBytesRead = 0;
  auto inputBuffer = std::make_unique<uint8_t[]>(bufferSize);
  ssize_t bytesRead = 0;
  do
  {
	bytesRead = read(inputFile, inputBuffer.get(), bufferSize);
	totalBytesRead += bytesRead;
	const uint8_t* end = inputBuffer.get() + bytesRead;
	for (const uint8_t* c = inputBuffer.get(); c < end; ++c)
	  ++charCounts[static_cast<size_t>(*c)];

  } while (totalBytesRead < fileSize);
  // Construct a lookup table with the Huffman code for each character.
  std::array<Encoding, 256> codes;
  MakeCodeTable(codes, charCounts);
  // If the file was too big to fit in our buffer then we have to read it again.
  if (fileSize > bufferSize)
  {
	lseek(inputFile, 0, SEEK_SET);
	bytesRead = read(inputFile, inputBuffer.get(), bufferSize);
  }
  // Open the output file and write the character counts.
  int outputFile = open(outputName, O_WRONLY|O_CREAT|O_TRUNC, 0666);
  if (outputFile < 0)
  {
	std::stringstream ss;
	ss << "Could not open file " << outputName;
	throw std::runtime_error(ss.str());
  }
  write(outputFile, charCounts.data(), 1024);
  // Marshall the Huffman codes into 64-bit integers. This should be the most
  // efficient size on modern processors, which have 64-bit registers.
  uint32_t outputBufferEntries = (bufferSize + 7) / 8;
  auto outputBuffer = std::make_unique<uint64_t[]>(outputBufferEntries);
  size_t bytesProcessed = 0;
  uint64_t output = 0;
  // outputPos keeps track of the position in the output buffer. It can be
  // 32 bits, because we'll never use a buffer with more than 2^32 entries.
  uint32_t outputPos = 0;
  uint32_t availableBits = 64;
  for (;;)
  {
	const uint8_t* end = inputBuffer.get() + bytesRead;
	// Process the contents of the file a byte at a time.
	for (const uint8_t* c = inputBuffer.get(); c < end; ++c)
	{
	  // Lookup the Huffman code for the current byte and append it to the
	  // output bit stream.
	  const Encoding& encoding = codes[static_cast<size_t>(*c)];
	  uint64_t code = encoding.code;
	  uint32_t codeLength = encoding.codeLength;
	  for (;;)
	  {
		output |= (code << (64 - availableBits));
		if (codeLength < availableBits)
		{
		  availableBits -= codeLength;
		  break;
		}
		else // codeLength >= availableBits
		{
		  outputBuffer[outputPos] = output;
		  output = 0;
		  if (++outputPos == outputBufferEntries)
		  {
			// The output buffer is full, so write the contents to the file.
			write(outputFile, outputBuffer.get(), outputPos * 8);
			outputPos = 0;
		  }
		  if (codeLength == availableBits)
		  {
			availableBits = 64;
			break;
		  }
		  codeLength -= availableBits;
		  code >>= availableBits;
		  availableBits = 64;
		}
	  }
	}
	bytesProcessed += bytesRead;
	if (bytesProcessed >= fileSize)
	  break;
	bytesRead = read(inputFile, inputBuffer.get(), outputBufferEntries);
  }

  close(inputFile);
  uint32_t remainingBytes = outputPos * 8;
  if (availableBits < 64)
  {
	outputBuffer[outputPos] = output;
	// Calculate how many bytes of the final word contain encoded data and
	// update remainingBytes accordingly. Note that this code assumes that the
	// processor is little-endian. It would have to be modified to work on
	// big-endian machines.
	remainingBytes += (64 + 7 - availableBits) / 8;
  }
  if (remainingBytes > 0)
	write(outputFile, outputBuffer.get(), remainingBytes);
  close(outputFile);
}
