#pragma once

#include <memory>
#include <string>

struct DecodingPath
{
  uint16_t destination;
  uint8_t outputLength;
  uint8_t output[4];
};

class HuffmanDecoder
{
  public:
	HuffmanDecoder(const char* inputName);
	~HuffmanDecoder();
	void Decode(const char* outputName);
	size_t Count(const uint8_t* pattern, uint32_t patternLength);
  private:
	void MakeDecodingTable(const std::array<uint32_t, 256>& charCounts);
	size_t DecodeChunk();

	std::string _inputName;
	std::unique_ptr<DecodingPath[]> _decodingTable;
	std::unique_ptr<uint8_t[]> _inputBuffer;
	std::unique_ptr<uint8_t[]> _outputBuffer;
	const DecodingPath* _position;
	size_t _uncompressedSize;
	// Number of bytes remaining to be decompressed.
	size_t _remainingSize;
	size_t _inputBufferSize;
	size_t _outputBufferSize;
	size_t _shortestCodeLength;
	int _fd;
};
