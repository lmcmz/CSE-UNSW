#include <iostream>
#include <stdlib.h>
#include <string.h>
#include "Decoder.h"
#include "Encoder.h"

static void Usage(const char* argv[])
{
  std::cerr << "Usage:" << std::endl
	<< '\t' << argv[0] << " -e input_file encoded_file" << std::endl
	<< '\t' << argv[0] << " -d encoded_file output_file" << std::endl
	<< '\t' << argv[0] << " -s search_string encoded_file" << std::endl;
  exit(1);
}

int main(int argc, const char* argv[])
{
  if (argc < 4)
	Usage(argv);

  try
  {
	if (strcmp(argv[1], "-s") == 0)
	{
	  HuffmanDecoder decoder(argv[argc - 1]);
	  // Allow multiple search strings. This is not required by the assignment
	  // specification but is useful for testing.
	  for (int i = 2; i < argc - 1; ++i)
	  {
		size_t count = decoder.Count(reinterpret_cast<const uint8_t*>(argv[i]), strlen(argv[i]));
		std::cout << count << std::endl;
	  }
	}
	else
	{
	  if (argc != 4)
		Usage(argv);

	  if (strcmp(argv[1], "-e") == 0)
	  {
		HuffmanEncode(argv[2], argv[3]);
	  }
	  else if (strcmp(argv[1], "-d") == 0)
	  {
		HuffmanDecoder decoder(argv[2]);
		decoder.Decode(argv[3]);
	  }
	}
  }
  catch (const std::exception& e)
  {
	std::cerr << e.what() << std::endl;
	return 1;
  }
  return 0;
}
