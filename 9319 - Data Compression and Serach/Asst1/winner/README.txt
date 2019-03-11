*** File Header ***

The header simply stores the occurrence count for each of the 256 possible
byte values as a 32-bit integer. Since we were told that our file header has
to be exactly 1024 bytes, there was no point in devising a more efficient
implementation.

*** Encoding Algorithm ***

The encoder first counts the occurrences of each byte value in the file. It
then uses the counts to construct a tree using the standard Huffman algorithm.

Encoding by using a Huffman tree directly is slow, so the encoder instead uses
the tree to construct a lookup table containing the Huffman code and code
length for every byte value that occurs in the file. The Huffman codes are
stored as 64 bit integers, since with a large file and an extremely skewed
distribution of byte values, the Huffman codes can exceed 32 bits. To obtain
the Huffman code for an input byte, the encoder uses the byte as an index into
the lookup table.

The encoder simply reads the file in chunks, looks up the Huffman code for
each character, and concatenates the codes into a bit stream. When the bit
stream fills the output buffer, it is written to disk.

The encoder is capable of correctly encoding files larger than 4GB, as long
as no character count is too large to fit in a 32-bit integer.

Decoding Algorithm

The decoder uses the counts in the file header to construct a Huffman tree
that is identical to the one that was used to encode the file.

The simplest way to decode a Huffman file is by walking through the Huffman
tree. Starting at the root node of the tree, you read the bits from the
encoded file one by one. You move to the node's left or right child depending
on whether the bit is a 0 or 1, continuing recursively until you reach a leaf
node. You then output the leaf's character and return to the root. Because
this method only processes a bit at a time, it is slow.

Rather than use the Huffman tree directly for decoding, my algorithm uses
the tree to construct a 2-dimensional lookup table. The table is used to
simulate walking a Huffman tree while processing the encoded data in 4-bit
"nibbles". The idea is that from any given node in the tree, the decoder
can follow 16 possible paths, depending on the 16 possible values of the
nibble. The path may reach leaf nodes and return to the root multiple times,
outputting a character each time.

Because the tree can contain at most 511 nodes, and there are 16 possible
nibbles, a 511 x 16 lookup table is sufficient to handle all possible
states in the largest possible tree. The table entries are represented by
the DecodingPath struct:

struct DecodingPath
{
  uint16_t destination;
  uint8_t outputLength;
  uint8_t output[4];
};

The DecodingPath struct is padded to 8 bytes, so the table occupies at most
65,400 bytes for the largest possible tree. I experimented with an
implementation that decoded a byte at a time, but it was slower. This was
presumably because it required a lookup table which was too large to fit in the
processor's level 1 or level 2 caches.

The decoder keeps track of its current position in the tree, which is initially
the root. Every time it reads a nibble, it uses its position and the nibble
as indices to look up the DecodingPath in the table.  This tells the decoder
which byte(s), if any, to output, and which position to move to. It continues
this process until the entire file has been decoded.

*** Searching ***

The search function works simply by decoding the file in chunks, and searching
within each chunk using the KMP algorithm. The code is designed to match
patterns which span chunk boundaries.

Because decoding and searching use the same decoding algorithm, most of
the code is in separate functions which are used by both the decoding and
search functions.

The code contains extensive comments which provide more detail on how it works.
