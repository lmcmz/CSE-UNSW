#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ftw.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

int global_str_len = 0;
char delimiter;
char *global_string = 0;
char *global_outputPath = 0;
char *index_filename;

int *delimiter_arr;
int delimiter_num = 0;

/************************************************************************************
				Bucket
************************************************************************************/

int binarySearch(int arr[], int l, int r, int x) 
{ 
	if (r >= l) 
	{
		int mid = l + (r - l)/2; 
		
		if (arr[mid] == x) {
			return mid; 
		}
			
		if (arr[mid] > x)  {
			return binarySearch(arr, l, mid-1, x); 	
		}
		return binarySearch(arr, mid+1, r, x); 
		} 
	return -1; 
} 

char *read_whole_file(char *filename)
{
	char * buffer = 0;
	long length;
	FILE * f = fopen (filename, "rb");
	if (f)
	{
		fseek (f, 0, SEEK_END);
		length = ftell(f);
		global_str_len = length;
		fseek (f, 0, SEEK_SET);
		buffer = malloc (length);
		if (buffer)
		{
			fread (buffer, 1, length, f);
		}
		fclose (f);
	} else {
		printf("OPEN FILE ERROR!\n");
	}
	buffer[length] = '\0';
	return buffer;
}

/************************************************************************************
				Sort
************************************************************************************/

void swap(int *a, int *b)  
{
	int temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

int bwt_strcmp(char *s1, char *s2)
{	
	while (*s1 == *s2)
	{
		
		if (*s1 == 0) {
//			printf("aaaaaaa\n");
			s1 = &global_string[0];
		}
		if (*s2 == 0) {
//			printf("bbbbbbbbb\n");
			s2 = &global_string[0];
		}
//		printf("[ %c - %c ]\n", *s1, *s2);
//		if (*s1 == delimiter) {
//			return s1 - s2;
//		}
		s1++;
		s2++;
		
		if (*s1 == 0) {
//			printf("aaaaaaa\n");
			s1 = &global_string[0];
		}
		if (*s2 == 0) {
//			printf("bbbbbbbbb\n");
			s2 = &global_string[0];
		}
	}
	
	if (*s1 == delimiter) {
		return -1;
	}else if (*s2 == delimiter) {
		return 1;
	}
	
	return *s1 - *s2;
}


void quicksort(int *array, int begin, int end)
{	
	int i, j;
	if(begin < end)
	{
		i = begin + 1;  
		j = end;
		while(i < j)
		{
			char *str_1 = &global_string[array[i]];
			char *str_2 = &global_string[array[begin]];
			if(bwt_strcmp(str_1, str_2) > 0) 
			{
				swap(&array[i], &array[j]); 
				j--;
			}
			else
			{
				i++;  
			}
		}
		char *str_1 = &global_string[array[i]];
		char *str_2 = &global_string[array[begin]];
		if(bwt_strcmp(str_1, str_2) >= 0)  		
		{
			i--;
		}
		swap(&array[begin], &array[i]); 
		quicksort(array, begin, i);
		quicksort(array, j, end);
	}
}

int comp( const void * p, const void * q) 
{ 
	return (*(int*)p - *(int*)q) ; 
}

void handle_index()
{	
	int i,j;
	
	int *index_arr = malloc(sizeof(int) * delimiter_num);
	memcpy(index_arr, delimiter_arr, sizeof(int) * delimiter_num);
	qsort(index_arr, delimiter_num, sizeof(int), comp);
	for (i=0;i<delimiter_num;i++) {
		int index = binarySearch(index_arr, 0, delimiter_num -1, delimiter_arr[i]);
		delimiter_arr[i] = index+1;
	}
	free(index_arr);
}

void bwtencode(int ascii_code, char *outputPath, int len, int *bucket_array)
{	
	FILE *file_w = fopen(outputPath,"a");
	int str_len = global_str_len;
	int i,k;

	char *suffix_array = malloc(sizeof(char)*global_str_len);
	
	quicksort(bucket_array, 0, len-1);
	
	if (ascii_code == delimiter) {
		for (i=0;i<len;i++) {
			if (bucket_array[i] == 0) {
				bucket_array[i] = global_str_len;
			}
		}
	}
//	
//	printf("\n-------------%c-------------\n", ascii_code);
//	
//	for (i=0;i<len;i++) {
//		printf("%c - %d\n", global_string[bucket_array[i]], bucket_array[i]);
//	}
//	printf("\n----------------------------\n");
	
	int j = 0;
	for (k=0;k<len;k++) {
		int index = bucket_array[k]-1;
		if (index == 0) {
			index = global_str_len;
		}
		suffix_array[j] = global_string[index-1];
		j++;
//		printf("%c", global_string[index-1]);
	}
	
	if (ascii_code == delimiter) {
		FILE *file = fopen(index_filename,"w");
		delimiter_arr = bucket_array;
		delimiter_num = len;
		handle_index();
		fwrite(delimiter_arr, sizeof(int), len, file);
		fclose(file);
	}
	
	
	fwrite(suffix_array, sizeof(char), len, file_w);
	free(suffix_array);
	fclose(file_w);
}

void encode_bucket(int ascii_code, int *arr)
{
	int j;
	int k = 0;
	for (j=0;j<global_str_len;j++) {
		if (global_string[j] == ascii_code) {
			arr[k] = j+1;
//			arr[k] = j;
			k++;
		}
	}
	if (k == 0) {
		return;
	}
	
	if (ascii_code == delimiter) {
		arr[k-1] = 0;
		
//		for (j=0;j<k;j++) {
//			printf("%d ", arr[j]);
//		}
//		printf("\n");
	}
	
	bwtencode(ascii_code, global_outputPath, k, arr);
}

void bwt_bucket()
{
	int i;
	int *bucket_arr = malloc(sizeof(int)*global_str_len);
	
	encode_bucket(delimiter, bucket_arr);
	
	for (i = 0; i<128; i++) {
		if (i == delimiter) {
			continue;
		}
		encode_bucket(i, bucket_arr);
	}
	free(bucket_arr);
}

int main(int argc, char *argv[]) 
{
	if (argc != 5) {
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
	
	char *tempPath = argv[2];
	char *inputPath = argv[3];
	char *outputPath = argv[4];
	
	asprintf(&index_filename,"%s%s",outputPath,"_index");
	
	global_outputPath = outputPath;
	remove(outputPath);
	remove(index_filename);
	
	/*---------------------------------*/
	char *string = read_whole_file(inputPath);
	global_string = string;
//	printf("%d", global_string[global_str_len]);
//	global_string[global_str_len+1] = '\0';
	bwt_bucket();
	
	return 0;
}