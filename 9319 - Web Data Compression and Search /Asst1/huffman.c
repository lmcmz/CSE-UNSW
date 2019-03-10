#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>

/*-------- Node --------*/ 

typedef struct Node 
{
	bool islinked;
	char code;
	unsigned char word;
	int frequency;
	struct Node *left;
	struct Node *right;	
	struct Node *parent;	
} Node;

Node* new_node(unsigned char word, int frequency, Node *left, Node *right)
{
	Node* temp = (Node*)malloc(sizeof(Node));
	temp->left = left;
	temp->right = right;
	temp->word = word;
	temp->frequency = frequency;
	temp->islinked = false;
	temp->parent = NULL;
	temp->code = 2;
	return temp;
}

// Find min frequency unchecked node
Node* min_node(int node_num, struct Node * tree[])
{
	int min = INT_MAX;
	int target = 0;
	int i;
	for (i=0; i<node_num; i++)
	{
		if ((tree[i] -> islinked) == true) {
			continue;
		}
			
		if ((tree[i] -> frequency) < min) {
			min = tree[i] -> frequency;
			target = i;
		}
	}
	return tree[target];
}

/*-------- Bit Operation --------*/ 

char read_single_bit(unsigned char* buffer, unsigned int index)
{
	unsigned char c = buffer[index / 8]; 
	unsigned int bit_position = index % 8; 
	return ((c >> (7 - bit_position)) & 1);
}

/*-------- END --------*/ 

/*-------- Global Var --------*/ 
int dict[255] = {0}; 
Node* temp_node;
int match_num = 0;

int node_count() {
	int j = 0;
	int i;
	for (i=0; i<256; i++) {
		if (dict[i] != 0) {
			j++;
		}
	}
	return j;
}

// Read file content
void frequency_count(char filename[]) 
{
	// END of file with ASCII 255  
	FILE *file = fopen(filename,"r");
	int c;
	while (!feof(file)) {
		c = fgetc(file);
		dict[c] ++;
//		printf("%c", c);
	}
	fclose(file);
}

void link_node(Node *node)
{
	if (node == NULL) {
		return;
	}
	
	link_node(node->right);
	link_node(node->left);
}

void read_parent_code(Node *node, char code[],int loop)
{
	if (node->parent == NULL) {
		return;
	}

	code[loop] = node->code;
	loop++;
	read_parent_code(node->parent, code, loop);
}

void find_code(struct Node * tree[], char code[], int node_num, int count, FILE *out_file,unsigned char  word)
{
	int target = 0;
	int i;
	for (i=0;i<count;i++) {
//		printf("PARENT: %c\n", tree[i]->parent->word);
		if (tree[i]->word == word) {
			target = i;
//			printf("PARENT: %d\n", tree[target]->parent->code);
			break;
		}
	
	}
	
	int loop = 0;
	read_parent_code(tree[target], code, loop);
}

void encode_to_file(struct Node * tree[], int node_num, int count, char filename[], char out_filename[])
{
	FILE *out_file = fopen(out_filename, "wb");
	fwrite(dict, sizeof(int), 256, out_file);
	
	FILE *file = fopen(filename,"r");
	int c;
	
	int cur_buffer = 0;
	int j = 0;
	char write_buffer = 0;
	
//	printf("Count: %d\n", count);
	
	while (!feof(file)) {
		c = fgetc(file);
		
		int max_code_len = count - 2;
		if (max_code_len < 0) {
			break;
		}
		char code[max_code_len];
		memset(code, 2, count-1);
		find_code(tree, code, node_num, count, out_file, c);
	
//		printf("En: %c ", c);
		int i;
		for (i=count-2;i>=0;i--) {
			if (code[i] == 2) {
				continue;
			}
			// Reach the end of file
			if (cur_buffer > 0 && c == -1) {
				fwrite(&write_buffer, sizeof(char), 1, out_file);
//				printf("%d %c\n", j, c);
				break;
			}
	
			if (cur_buffer == 8) {
				fwrite(&write_buffer, sizeof(char), 1, out_file);
				cur_buffer = 0;
				write_buffer = 0;
//				printf("\n");
			}
			
			write_buffer |= (code[i] == 1) << (7 - cur_buffer);
			cur_buffer++;
//			printf("%d", code[i]);
		}
//		printf("\n");
		j++;
	}
	
	fclose(out_file);
}

void build_tree(struct Node * tree[], int node_num, int count)
{
	unsigned char character[count];
	int frequnency[count];
	
	int j = 0;
	int i;
	for (i=0; i<256; i++) 
	{
		if (dict[i] != 0) {
			character[j] = i;
			frequnency[j] = dict[i];
			j++;
		}
	}
		
	// Initial Tree
	for (i=0; i<=node_num; i++)
	{
		// Have character
		if (i < count)
		{
			tree[i] = new_node( character[i], frequnency[i], NULL, NULL);
//			printf(" %d  %c %d\n", i,tree[i]->word, tree[i]->frequency);
			continue;	
		}
		// Have no character
		tree[i] = new_node(0, INT_MAX, NULL, NULL);
	}
	
	for (i=count; i<=node_num; i++) 
	{
		Node* min_1 = min_node(node_num, tree);
		min_1 -> islinked = true;
		min_1 -> code = 0;
		min_1 -> parent = tree[i];
		
		Node* min_2 = min_node(node_num, tree);
		min_2 -> islinked = true;
		min_2 -> code = 1;
		min_2 -> parent = tree[i];
		
		tree[i]	-> left = min_1;
		tree[i] -> right = min_2;
		tree[i] -> frequency = (min_1 -> frequency) + (min_2 -> frequency);
	}
	
	link_node(tree[node_num]);
}

void encode_haffman(char filename[], char out_filename[]) 
{
	frequency_count(filename);
	int count = node_count();
	int node_num = (2*count -2); // less than 256
	struct Node * tree[node_num];
	build_tree(tree, node_num, count);
	encode_to_file(tree, node_num, count, filename,out_filename);
}


/*-------- Decode --------*/ 


void decode_haffman(char in_filename[], char out_filename[]) 
{
	FILE *file = fopen(in_filename, "rb");
	fread(dict, sizeof(int), 256, file);
	
	int count = node_count();
//	printf("count: %d\n", count);
	
	// Handle one character case
	int max_code_len = 2*count -2;
	if (max_code_len <= 0) {
		FILE *out_file = fopen("decode.txt", "wb");
		int i;
		for (i =0;i<256;i++) {
			if (dict[i] == 0) {
				continue;
			}
			int j;
			for (j=0; j<= dict[i]; j++) {
				fwrite(&i, sizeof(char), 1, out_file);
			}
		}
		fclose(out_file);
		return;
	}
	int node_num = (max_code_len); // less than 256
	struct Node * tree[node_num];
	
	build_tree(tree, node_num, count);
	
//	for (int i=0; i<node_num; i++)
//	{
//		printf("Code: %c %d - %d %d\n", tree[i] -> word ,tree[i] -> frequency, tree[i] -> code, tree[i]->parent->frequency);
//	}
	
	unsigned char buffer = fgetc(file);
	int k = 0;
	int read_buffer = 0;
	int decode_len = 0;
	
	// MAX code length is count - 1
	char code[count-2];
	memset(code, 2, count-2);
	
	temp_node = new_node(0, 0, NULL, NULL);
	
	FILE *out_file = fopen(out_filename, "wb");
	
	while (!feof(file)) {
		Node *node = tree[node_num];
		if (temp_node->left != NULL) {
			node = temp_node;
			temp_node = tree[node_num];
		}
		
		int i;
		for (i=0;i<8;i++) {
			unsigned char bit = read_single_bit(&buffer, i);
//			printf("%d", bit);
			
			if (bit == 1) {
				node = node->right;
			} else {
				node = node->left;
			}
			
			if (node->right==NULL && node->left==NULL) {
				fwrite(&node->word, sizeof(char), 1, out_file);
//				printf("%c", node->word);
				node = tree[node_num];
				k++;
				if (k==tree[node_num]->frequency) {
					break;
				}
			}
			if (i==7 && node->right!=NULL) {
				temp_node = node;
			}
		}
//		printf("\n");
		buffer = fgetc(file);
	}
	fclose(out_file);
	fclose(file);
}

/*-------- Search --------*/ 

void prefix_table(char pattern[], int prefix[], int n)
{
	prefix[0] = 0;
	int len = 0;
	int i = 1;
	while (i < n) {
		if (pattern[i] == pattern[len]) {
			len++;
			prefix[i] = len;
			i++;
		} else {
			if (len > 0) {
				len = prefix[len - 1];
			} else {
				prefix[i] = len;
				i++;
			}
		}
	}
	
	for (i = n - 1;i > 0;i--) {
		prefix[i] = prefix[i-1];
	}
	prefix[0] = -1;
}

int kmp_search(char text[], char pattern[])
{
	int n = strlen(pattern);
	int m = strlen(text);
	
//	printf(" n: %d  m:  %d\n",n ,m);
//	printf("\n");
	int *prefix = malloc(sizeof(int) * n);
	prefix_table(pattern, prefix, n);
	
//	printf("Prefix: ");
//	for (int i =0;i<n;i++) {
//		printf(" %d", prefix[i]);
//	}
//	printf("\n");
	
	int i = 0;
	int j = 0;
	int mark = -2;
	while (i<m) {
		if (j == n-1 && text[i]==pattern[j]) {
			match_num++;
//			printf("\nFOUND target: %d. %d %d\n", i-j, i, m);
			j = prefix[j];
//			mark = j;
//			printf("N:  %d  %d\n", i, mark);
			if (j== -1) {
				i++;
			   	j++;
			}
		}
		if (text[i]== pattern[j]) {
			i++; 
			j++;
			if (i==m) {
				mark = j-1;
//				printf("AAAAAAAA.   %d\n", j-1);
			}
		}else {
			j = prefix[j];
//			mark = j;
//			printf("N:  %d %d\n" , i, mark);
			if (j== -1) {
				i++;
			   	j++;
			}
		}
	}
	return mark;
}

int search_in_buffer(char search_buffer[], int k, char target[])
{
	char decode_text[k];
	int i;
	for (i=0;i<k;i++) {
		decode_text[i] = search_buffer[i];
//		printf("%c", decode_text[i]);
	}
//	printf("\n %d  %lu\n", k, strlen(decode_text));
	return kmp_search(decode_text, target);
}

void search_haffman(char target[], char filename[]) {
	//TODO
	
	FILE *file = fopen(filename, "rb");
	fread(dict, sizeof(int), 256, file);
	int count = node_count();
	int max_code_len = 2*count -2;
	int node_num = (max_code_len); // less than 256
	struct Node * tree[node_num];
	build_tree(tree, node_num, count);
	
//	for (int i=0; i<node_num; i++)
//	{
//		printf("Code: %c %d - %d %d\n", tree[i] -> word ,tree[i] -> frequency, tree[i] -> code, tree[i]->parent->frequency);
//	}
	
//	unsigned char search_buffer[1024 * 1024]={0};  // 1M Buffer
	
	int search_buffer_limit = 1024*1024;
	
	int s_k = 0;
	int target_len = strlen(target);
	char search_buffer[search_buffer_limit];  // 1M Buffer
	unsigned char buffer = fgetc(file);
	
	int k = 0;
	
	int read_buffer = 0;
	int decode_len = 0;
	
	// MAX code length is count - 1
	char code[count-2];
	memset(code, 2, count-2);
	
	temp_node = new_node(0, 0, NULL, NULL);
//	printf("%d\n", tree[node_num]->frequency);
	
	while (!feof(file)) {
		Node *node = tree[node_num];
		if (temp_node->left != NULL) {
			node = temp_node;
			temp_node = tree[node_num];
		}
		int i;
		for (i=0;i<8;i++) {
			unsigned char bit = read_single_bit(&buffer, i);
//			printf("%d", bit);
			
			if (bit == 1) {
				node = node->right;
			} else {
				node = node->left;
			}
			
			if (node->right==NULL && node->left==NULL) {
//				fwrite(&node->word, sizeof(char), 1, out_file);
			
//				fflush(stdout);  // DEBUG Segmentation Fault
//				printf("%c", node->word);
				search_buffer[s_k] = node->word;
				node = tree[node_num];
				k++;
				s_k++;
				
				
				if ((s_k % search_buffer_limit == 0 && s_k >0) || k==tree[node_num]->frequency) {
//					fflush(stdout);  // DEBUG Segmentation Fault
					int mark = search_in_buffer(search_buffer, s_k, target);
//					printf("mark: %d\n", mark);
					s_k = 0;
//					if (mark != -2 && mark != -1) {
					if (mark != -2) {
						
//						fflush(stdout);
//						printf("Before:\n");
//						for (int g=0;g<search_buffer_limit;g++) {
//							printf("%c", search_buffer[g]);
//						}
//						printf("\n%d \n", s_k);
						int g;
						for (g=0;g<=mark;g++) {
//							fflush(stdout);
//							printf("\n%d   %d\n",g, search_buffer_limit-mark+g-1);
//							char temp = 
							search_buffer[g] = search_buffer[search_buffer_limit-mark+g-1];
							s_k++;
						}
						
//						fflush(stdout);
//						printf("After:\n");
//						for (int g=0;g<search_buffer_limit;g++) {
//							printf("%c", search_buffer[g]);
//						}
//						printf("\n%d \n", s_k);

//						s_k = mark+1;
					}
				}
				
				if (k==tree[node_num]->frequency) {
					break;
				}
			}
			if (i==7 && node->right!=NULL) {
				temp_node = node;
			}
		}
//		printf("\n");
		buffer = fgetc(file);
	}
	printf("%d\n", match_num);
}

int main(int argc, char *argv[]) {
	
	if( argc == 4 ) {
		char mode_str[strlen(argv[1])+1];
		strcpy(mode_str, argv[1]);
		char input_path[strlen(argv[2])+1];
		strcpy(input_path, argv[2]);
		char output_path[strlen(argv[3])+1];
		strcpy(output_path, argv[3]);
		
		if (strcmp(mode_str, "-e") == 0) 
		{
			encode_haffman(input_path, output_path);
		} else if (strcmp(mode_str, "-d") == 0) 
		{
			decode_haffman(input_path, output_path); 
		} else if (strcmp(mode_str, "-s") == 0) 
		{
			search_haffman(input_path, output_path);
		} else {
			printf("Wrong argument.\n");
		}
	}
	else if( argc > 4 ) {
		printf("Too many arguments supplied.\n");
	}
	else {
		printf("Missing argument expected.\n");
	}
}
