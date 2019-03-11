#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define BUFFER_SIZE 6000
//1048576 
//(1024*1024)

int freq_table[128];
int skip_maxtrix[128][8739];

int* c_array;
char* c_str_array;
int unique_count = 0;

char *bwtPath;
char *indexPath;

char delimiter;

int *delimiter_pos;
int global_index_len = 0;
int match_section = -1;
int index_file_len;

FILE *bwt_fp;

int in_array(int val, int *arr, int size)
{
	int i;
	for (i=0; i < size; i++) {
		if (arr[i] == val)
			return i;
	}
	return -1;
}

int char_in_str(char target, char *str, int len)
{
	int count = 0;
	int i;
	for (i=0; i < len; i++) {
		if (str[i] == target) {
			count++;
		}
	}
	return count;
}

int comp( const void * p, const void * q) 
{ 
	return (*(int*)p - *(int*)q) ; 
}

int get_file_len(char *filename, size_t size)
{
	FILE * f = fopen (filename, "rb");
	fseek (f, 0, SEEK_END);
	int length = ftell(f) / size;
	fclose(f);
	return length;
}

// Read file content
void frequency_count(char filename[]) 
{
	// END of file with ASCII 255  
//	FILE *file = fopen(filename,"r");
	char c;
	int i;
	int pos = 0;
	
	while (!feof(bwt_fp)) {
		c = fgetc(bwt_fp);
		freq_table[c] ++;
		pos++;
		int k = pos % (BUFFER_SIZE); // 1MB
		if (k == 0) {
			int k_c = (pos / (BUFFER_SIZE)) - 1;
			for (i=0;i<128;i++) {
				skip_maxtrix[i][k_c] = freq_table[i];
			}
			k++;
		}
	}
//	fclose(file);
	
//	printf("%d\n", skip_maxtrix['r'][8]);
//	printf("%d\n", skip_maxtrix['r'][9]);
}


int *read_index_file(char *filename)
{
	int * buffer = 0;
	long length;
	FILE * f = fopen (filename, "rb");
	if (f)
	{
		fseek (f, 0, SEEK_END);
		length = ftell(f) / sizeof(int);
		global_index_len = length;
		fseek (f, 0, SEEK_SET);
		buffer = malloc (length * sizeof(int));
		if (buffer)
		{
			fread (buffer, sizeof(int), length, f);
		}
		fclose (f);
	} else {
		printf("OPEN FILE ERROR!\n");
	}
	buffer[length] = '\0';
	return buffer;
}

int get_index_pos(int index, FILE *file) 
{
	fseek(file, 0, SEEK_SET);
	int file_len = index_file_len;
	int buffer_size = 1024;
	
	int k = file_len / buffer_size;
	int remind = file_len % buffer_size;
	
	if (remind != 0) {
		k++;
	}
	
	int i = 0;
	int* buffer = malloc(sizeof(int) * buffer_size);
	int load_size = buffer_size;
	
	for (i=0;i<k;i++) {
		if (i == k-1) {
			load_size = remind;
		}
		
		fread(buffer, sizeof(int), load_size, file);
		int result = in_array(index, buffer, load_size);
		if( result != -1){
			free(buffer);
			return (i * buffer_size)+result;
		}	
	}
	free(buffer);
	printf("ERROR.  05\n");
	return -1;
}

// TODO: improve efficient
int occr(char target, int index) 
{
//	FILE *file = fopen(bwtPath,"rb");
	int num = 0;
	int k = 0;
	
	if (index > BUFFER_SIZE) {
		k = (index / BUFFER_SIZE);
		num = skip_maxtrix[target][k-1];
	}
	
//	printf("num: %c, %d, %d\n", target, index, k);
	fseek(bwt_fp, sizeof(char)*(BUFFER_SIZE * k), SEEK_SET);
	
	int buffer_re = index % BUFFER_SIZE;
	char *buffer = malloc(sizeof(char) * buffer_re);
	fread(buffer, sizeof(char), buffer_re, bwt_fp);
	
//	printf("num: %c, %d, %d, %d\n", target, index, k, buffer_re);
	
//	
//	int i;
//	for (i=0;i<buffer_re;i++) {
//		printf("%c", buffer[i]);
//	}
//	printf("\n");
	
	num = num + char_in_str(target, buffer, buffer_re);
	free(buffer);
//	fclose(file);
	return num;
}

char get_char(int index, FILE *file)
{
	fseek(file, index-1, SEEK_SET);
	char target = fgetc(file);
	return target;
}

int get_index(int index, FILE *file)
{
	fseek(file, sizeof(int)*(index-1), SEEK_SET );
	int target;
	fread(&target, sizeof(int), 1, file);
	return target;
}

void generate_c_array()
{
	int count = 0;
	int i;
	for (i=0;i<128;i++) {
		if (freq_table[i] == 0) {
			continue;
		}
		count++;
//		printf("%c - %d\n", i, freq_table[i]);
	}
	
	unique_count = count;
	c_array = malloc(sizeof(int) * unique_count);
	c_str_array = malloc(sizeof(char) * unique_count);
	
	int k = 0;
	c_array[k] = k;
	c_str_array[k] = delimiter;
	int pos = 0;
	pos += freq_table[delimiter];
	k++;
	
	for (i=0;i<128;i++) {
		if (freq_table[i] == 0 || i == delimiter) {
			continue;
		}
		c_array[k] = pos;
		c_str_array[k] = i;
		k++;
		pos += freq_table[i];
	}
	
//	for (i=0;i<unique_count;i++) {
//		printf("%d - %c - %d\n", i,c_str_array[i], c_array[i]);
//	}
}

int find_c_index(char target, bool isNext)
{
	int i;
	for (i=0;i<unique_count;i++) {
		if (c_str_array[i] == target) {
			if (isNext) {
				if (i == unique_count -1) {
					return c_array[i] + freq_table[target];
				}
				return c_array[i+1];
			}
//			printf("TTT: %d - %d\n", i, c_array[i]);
			return c_array[i];
		}
	}
	printf("ERROR!   01 \n");
	return -1;
}

int* backward_search(int index, char *test_str)
{
//	printf("\n--------------\n");
	int i = index;
//	printf("i: %d\n", i);
	int c = test_str[i-1];
//	printf("C: %c\n", c);
	int first = find_c_index(c, false) + 1;
//	printf("F: %d\n", first);
	int last = find_c_index(c, true);
//	printf("L: %d\n", last);
//	printf("\n--------------\n");
	
	while ( (first <= last) && (i >= 2) ) {
//		printf("i: %d\n", i);
		c = test_str[i-2];
//		printf("C: %c\n", c);
		int t1 = find_c_index(c, false);
		int t2 = occr(c, first-1) ;
//		printf("F: %d %d %d\n",t1 , t2 ,first);
		first = t1 + t2 + 1;
//		printf("F: %d\n", first);
		
		t2 = occr(c, last);
//		printf("F: %d %d %d\n",t1 , t2 ,last);
		last = t1 + t2;
//		printf("L: %d\n", last);
		i--;
//		printf("\n--------------\n");
	}
	
	int * arr = malloc(sizeof(int) * 2);
	arr[0] = first;
	arr[1] = last;
	return arr;
}

void m_mode(char *test_str)
{
	int target_str_len = strlen(test_str);
	int* arr = backward_search(target_str_len, test_str);
	int first = arr[0];
	int last = arr[1];
	
//	printf("F: %d\nL: %d\n", arr[0], arr[1]);
	if (last < first) {
		return;
	}
//	printf("F: %d\nL: %d\n", arr[0], arr[1]);
	printf("%d\n", last-first+1);
}

int * n_mode(char *test_str)
{
//	FILE *file = fopen(bwtPath,"r");
	int target_str_len = strlen(test_str);
	int* arr = backward_search(target_str_len, test_str);
	int first = arr[0];
	int last = arr[1];
	
//	printf("F: %d\nL: %d\n", arr[0], arr[1]);
	
	if (last < first) {
		return NULL;
	}
	
	int k=0;
	int * delimiter_arr = malloc(sizeof(int)*(last-first+1));
	memset(delimiter_arr, -1, sizeof(int)* (last-first+1));
	
//	printf("F: %d\nL: %d\n", arr[0], arr[1]);
	
	int i;
	for (i=first;i<=last;i++) {
//		printf("%d\n", i);
		char c = get_char(i, bwt_fp);
		//printf("%d\n", c);
		
		int pos = i;
		while (c != delimiter) {
//			printf("%c", c);
			int offset = occr(c, pos);
			pos = find_c_index(c, false) + offset;
//			printf("%c - %d - %d\n", c, offset, pos);
			c = get_char(pos, bwt_fp);
		}
//		printf(" %c - %d - %d\n", c, pos, occr(delimiter, pos-1)+1);
//		printf("\n");
		delimiter_arr[k] = pos-1;
		k++;
	}
	
//	printf("bb\n");
	int result = 1;
	qsort(delimiter_arr, k, sizeof(int), comp);
	
	int * result_arr = malloc(sizeof(int)*(last-first+1));
	memset(result_arr, -1, sizeof(int)*(last-first+1));
	
	int q = 0;
	int temp = 0;
	result_arr[q] = delimiter_arr[0];
	q++;
	
	for (i=0;i<k-1;i++) {
		if (delimiter_arr[i] != delimiter_arr[i+1]) {
			result++;
			result_arr[q] = delimiter_arr[i+1];
			q++;
//			continue;
		}
	}
	
	free(delimiter_arr);
//	printf("aa\n");
	
	match_section = result;
//	fclose(file);
	return result_arr;
}

// TODO: Sort
void a_mode(char *test_str)
{
	FILE *file = fopen(indexPath,"r");
	int i;
	int* delimiter_arr = n_mode(test_str);
	if (!delimiter_arr) {
		fclose(file);
		return;
	}
	int* result_arr = malloc(sizeof(int) * match_section);
	int k = 0;
	int delimter_num = index_file_len;
//	get_file_len(indexPath, sizeof(int));
//	printf("D_len: %d\n", delimter_num);
		
	for (i=0;i<match_section;i++) {
		int offset = occr(delimiter, delimiter_arr[i]+1);
//		printf("D: %d - %d\n", delimiter_arr[i]+1, offset);
		int target = get_index(offset, file);
		target++;
//		
		if (target > delimter_num) {
//			printf("MAX: %d\n", target);
			target = 1;
		}
		result_arr[k] = target;
		k++;
//		printf("%d\n",target) ;
	}
//	printf("cccc\n");
	qsort(result_arr, match_section, sizeof(int) ,comp);
	
	for (i=0;i<match_section;i++) {
		printf("%d\n",result_arr[i]);
	}
	fclose(file);
}

void i_mode(char *test_str)
{
//	FILE *file = fopen(bwtPath,"r");
	FILE *file_i = fopen(indexPath,"r");
	int start = -1;
	int end = -1;
	sscanf(test_str, "%d %d", &start, &end);
	
	char str[5001];
	
	str[5000] = '\0';
	
	int i;
	for (i=start;i<=end;i++) 
	{
		int index = 4999;
		int pos = get_index_pos(i, file_i)+1;
		char c = get_char(pos, bwt_fp);
//		printf("%d - %c\n", pos, get_char(pos));
		
		while (c != delimiter) {
//			printf("%c", c);
			str[index] = c;
			index--;
			int offset = occr(c, pos);
			pos = find_c_index(c, false) + offset;
//			printf("%c - %d - %d\n", c, offset, pos);
			c = get_char(pos, bwt_fp);
		}
		printf("%s\n", str+index+1);
//		printf("\n");
	}
	fclose(file_i);
//	fclose(file);
}

int main(int argc, char *argv[]) 
{
	if (argc != 6) {
		printf("Argumens invalid!\n");
		return 1;
	}
	char *delimiter_argv = argv[1];
	delimiter = delimiter_argv[0];
	
	if (strcmp(delimiter_argv, "\\n") == 0) {
//		printf("YES");
		delimiter = 10;
	}
	
	//Coderunner Bug!!
	if (strcmp(delimiter_argv, "n") == 0) {
//		printf("YES");
		delimiter = 10;
	}
	
//	printf("%c\n", delimiter);
	
	bwtPath = argv[2];
	char *tempFolder = argv[3];
	
	asprintf(&indexPath,"%s%s",bwtPath,"_index");
	
	char *mode = argv[4];
	char *test_str = argv[5];
	
//	memset(skip_maxtrix, 0, sizeof(int)*128*10);
	
	bwt_fp = fopen(bwtPath,"rb");
	
	frequency_count(bwtPath);
	generate_c_array();
	
//	delimiter_pos = read_index_file(indexPath);
//	int i;
//	for (i=0;i<global_index_len;i++) {
//		printf("%d ", delimiter_pos[i]);
//	}
//	printf("\n");
	
//	int i;
//	for (i=0;i<128;i++) {
//		printf("%c - %d\n", i, skip_maxtrix[128][49]);
//	}
	
	index_file_len = get_file_len(indexPath, sizeof(int));
	
	if (strcmp(mode, "-m") == 0) {
		m_mode(test_str);
	}
	
	if (strcmp(mode, "-n") == 0) {
		int* result = n_mode(test_str);
		if (result) {
			printf("%d\n", match_section);
		}
	}
	
	if (strcmp(mode, "-a") == 0) {
		a_mode(test_str);
	}
	
	if (strcmp(mode, "-i") == 0) {
		i_mode(test_str);
	}
	
	fclose(bwt_fp);
	return 0;
}