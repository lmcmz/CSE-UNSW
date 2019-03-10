#include "HuffmanTree.h"
#include <queue>

class CountGreater
{
  public:
	bool operator()(const HuffmanNode* a, const HuffmanNode* b) const
	{
	  return a->count > b->count;
	}
};

uint32_t MakeHuffmanTree(std::array<HuffmanNode, 511>& nodes, const std::array<uint32_t, 256>& charCounts)
{
  std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, CountGreater> trees;
  // Reserve the first node for the root.
  uint16_t nodeIndex = 1;
  // Initialize the leaf nodes representing all the characters in the file.
  // Nodes for characters that are not in the file don't need to be initialized
  // because they will not be referenced.
  for (uint32_t i = 0; i < 256; ++i)
  {
	if (charCounts[i] > 0)
	{
	  HuffmanNode& node = nodes[nodeIndex];
	  node.count = charCounts[i];
	  node.index = nodeIndex;
	  // leftChild and rightChild are indicies into the nodes array. On leaf
	  // nodes they point back to the root.
	  node.leftChild = 0;
	  node.rightChild = 0;
	  node.character = static_cast<uint8_t>(i);
	  node.leaf = true;
	  trees.push(&node);
	  ++nodeIndex;
	}
  }

  // Repeatedly remove the two nodes with the lowest counts, combine them into a
  // subtree, and add the subtree to the priority queue.
  while (trees.size() > 2)
  {
	HuffmanNode& subRoot = nodes[nodeIndex];
	subRoot.count = trees.top()->count;
	subRoot.leftChild = trees.top()->index;
	trees.pop();
	subRoot.count += trees.top()->count;
	subRoot.rightChild = trees.top()->index;
	trees.pop();
	subRoot.index = nodeIndex;
	subRoot.leaf = false;
	++nodeIndex;
	trees.push(&subRoot);
  }

  // We want the root of the tree to be the first node in the array, so combine
  // the last 2 nodes here. Some extra logic is also necessary for files that
  // contain only a single byte value, and therefore have only the root and one
  // other node.
  HuffmanNode& root = nodes[0];
  root.leftChild = trees.top()->index;
  root.count = trees.top()->count;
  trees.pop();
  root.index = 0;
  root.leaf = false;
  if (!trees.empty())
  {
	root.rightChild = trees.top()->index;
	root.count += trees.top()->count;
  }
  else
  {
	root.rightChild = 0;
  }
  // Return the number of nodes in the tree.
  return nodeIndex;
}
