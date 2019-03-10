/*
 * src/tutorial/intset.c
 *
 ******************************************************************************
  This file contains routines that can be bound to a Postgres backend and
  called by the backend in the process of processing queries.  The calling
  format for these routines is dictated by Postgres architecture.
******************************************************************************/

#include "postgres.h"
#include <string.h>
#include <regex.h>
#include "fmgr.h"
#include "libpq/pqformat.h"		/* needed for send/recv functions */

PG_MODULE_MAGIC;

char* string_append(const char *str1, const char *str2);
int check_comma(char *arr, int len);
bool check_elements_array(int val, int *arr, int len);
int match(const char *string, const char *pattern);
void remove_spaces(char *str);
void remove_head_tail(char *str);


typedef struct Intset
{
	int32		len;
	int			arr[FLEXIBLE_ARRAY_MEMBER];
}Intset;

Intset * cstring_to_intset(char *str);

int match(const char *string, const char *pattern)
{
	regex_t re;
	if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) return 0;
	int status = regexec(&re, string, 0, NULL, 0);
	regfree(&re);
	if (status != 0) return 0;
	return 1;
}

void remove_spaces(char *str)
{
	int count = 0;
	int i;
	for (i = 0; str[i]; i++)
		if (str[i] != ' ' && str[i] != '\t' && str[i] != '\n')
			str[count++] = str[i];
	str[count] = '\0';
}


char* string_append( const char *str1, const char *str2)
{
	int len_1 = strlen(str1);
	int len_2 = strlen(str2);
	char *result = palloc(len_1 + len_2 + 1);
	strcpy(result, str1);
	strcat(result, str2);
	return result;
}

bool check_elements_array(int val, int *arr, int len)
{
    int i;
    for (i=0; i < len; i++) {
        if (arr[i] == val)
			return true;
    }
    return false;
}

int check_comma(char *arr, int len)
{
	int i;
	int count=0;
	for(i=0; i<len; i++) {
		if (arr[i] == ',')
			count++;
	}
	return count;
}

void remove_head_tail(char *str) 
{
    size_t len = strlen(str);
    // assert(len >= 2);
    memmove(str, str+1, len-2);
    str[len-2] = 0;
}


Intset * cstring_to_intset(char *str)
{
	const char* re_1 = "[0-9]+[ \t]+[0-9]+";
	const char* re_2 = "[^0-9,-]+";
	const char* re_3 = "[0-9]+,[,]+[0-9]+";
	const char* re_4 = "^-?[0-9]+$";
	const char* re_5 = "^{.*}$";

	if (match(str, re_1))
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for intset 1: \"%s\"",
						str)));

	remove_spaces(str);

	if ((match(str, re_5)))
	ereport(ERROR,
			(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
		 errmsg("invalid input syntax for intset 2: \"%s\"",
					str)));

	int len = strlen(str);

	if (len <= 2) {
		Intset *intset = (Intset *) palloc(VARHDRSZ);
		SET_VARSIZE(intset, VARHDRSZ);
		// memcpy(intset->arr, [], VARHDRSZ);
		return intset;
	}

	char new_str[len-2];
	// sscanf(str, " { %[^}]}", new_str);
	remove_head_tail(str);
	strcpy(new_str, str);

	if ((match(new_str, re_2)))
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for intset 3: \"%s\"",
						new_str)));


	int count_of_comma = check_comma(new_str, len-2);
	int length_new_str = count_of_comma + 1;
	int arr[length_new_str];

	int lala;
	for (lala=0; lala<length_new_str; lala++) {
		arr[lala] = INT32_MAX;
	}
	// printf("%s\n",arr);

	if (match(new_str, re_3))
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for intset 4: \"%s\"",
						new_str)));

	char *result;
	result = strtok(new_str, ",");
	int j = 0;

	while (result != NULL) {
		if (!(match(result, re_4)))
		ereport(ERROR,
				(errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
				 errmsg("invalid input syntax for intset 5: \"%s\"",
						result)));

		int i = atoi(result);
		if(!check_elements_array(i, arr, length_new_str)){
			arr[j] = i;
			j++;
		}
		result = strtok(NULL, ",");
	}

	Intset *intset = (Intset *) palloc(VARHDRSZ + sizeof(int)*j);
	SET_VARSIZE(intset, VARHDRSZ + sizeof(int)*j);
	memcpy(intset->arr, arr, VARHDRSZ + sizeof(int)*j);
	return intset;
}

/*****************************************************************************
 * Input/Output functions
 *****************************************************************************/

PG_FUNCTION_INFO_V1(intset_in);

Datum
intset_in(PG_FUNCTION_ARGS)
{
	char	   *str = PG_GETARG_CSTRING(0);
	Intset *intset = cstring_to_intset(str);
	PG_RETURN_POINTER(intset);
}

PG_FUNCTION_INFO_V1(intset_out);

Datum
intset_out(PG_FUNCTION_ARGS)
{
	Intset    *intset = (Intset *) PG_GETARG_POINTER(0);
	char *result = "{";
	char *suffix = "}";

	int i;
	int len = (VARSIZE(intset)-VARHDRSZ)/sizeof(int);
	for(i=0; i<len; i++) {
		char *value = psprintf("%d", intset->arr[i]);
		result = string_append(result, value);
		if(i != len-1)
		{
			value = ",";
			result = string_append(result, value);
		}
	}

	result = string_append(result, suffix);
	PG_RETURN_CSTRING(result);
}


/*****************************************************************************
 * New Operators
 *
 * A practical intset datatype would provide much more than this, of course.
 *****************************************************************************/

PG_FUNCTION_INFO_V1(intset_include);

Datum
intset_include(PG_FUNCTION_ARGS)
{
	int a = PG_GETARG_INT32(0);
	Intset    *intset = (Intset *) PG_GETARG_POINTER(1);
	int len = (VARSIZE(intset)-VARHDRSZ)/sizeof(int);
	bool is_include = check_elements_array(a, intset->arr, len);
	PG_RETURN_BOOL(is_include);
}


PG_FUNCTION_INFO_V1(intset_cadinality);

Datum
intset_cadinality(PG_FUNCTION_ARGS)
{
	Intset    *intset = (Intset *) PG_GETARG_POINTER(0);
	int len = (VARSIZE(intset)-VARHDRSZ)/sizeof(int);
	PG_RETURN_INT32(len);
}


PG_FUNCTION_INFO_V1(intset_subset);

Datum
intset_subset(PG_FUNCTION_ARGS)
{
	Intset    *a = (Intset *) PG_GETARG_POINTER(0);
	Intset    *b = (Intset *) PG_GETARG_POINTER(1);

	int len_a = (VARSIZE(a)-VARHDRSZ)/sizeof(int);
	int len_b = (VARSIZE(b)-VARHDRSZ)/sizeof(int);

	int count = 0;
	int i;
	for (i=0; i<len_a; i++)
		if (check_elements_array(a->arr[i], b->arr, len_b)) 
			count ++;
	PG_RETURN_BOOL(count==len_a);
}


PG_FUNCTION_INFO_V1(intset_equal);

Datum
intset_equal(PG_FUNCTION_ARGS)
{
	Intset    *a = (Intset *) PG_GETARG_POINTER(0);
	Intset    *b = (Intset *) PG_GETARG_POINTER(1);

	bool bool_equal = true;

	int len_a = (VARSIZE(a)-VARHDRSZ)/sizeof(int);
	int len_b = (VARSIZE(b)-VARHDRSZ)/sizeof(int);

	if (len_a != len_b)
		bool_equal = false;
	int i;
	for (i=0; i<len_a; i++)
		if (a->arr[i] != b->arr[i])
			bool_equal = false;
	PG_RETURN_BOOL(bool_equal);
}


PG_FUNCTION_INFO_V1(intset_intersection);

Datum
intset_intersection(PG_FUNCTION_ARGS)
{
	Intset    *a = (Intset *) PG_GETARG_POINTER(0);
	Intset    *b = (Intset *) PG_GETARG_POINTER(1);

	int len_a = (VARSIZE(a)-VARHDRSZ)/sizeof(int);
	int len_b = (VARSIZE(b)-VARHDRSZ)/sizeof(int);

	int array[len_a];
	int i;
	int j = 0;
	for (i=0; i<len_a; i++) {
		if (check_elements_array(a->arr[i],b->arr,len_b))
		{
			array[j] = a->arr[i];
			j++;
		}
	}

	Intset *intset = (Intset *) palloc(VARHDRSZ + sizeof(int)*j);
	SET_VARSIZE(intset, VARHDRSZ + sizeof(int)*j);
	memcpy(intset->arr, array, VARHDRSZ + sizeof(int)*j);
	PG_RETURN_POINTER(intset);
}


PG_FUNCTION_INFO_V1(intset_union);

Datum
intset_union(PG_FUNCTION_ARGS)
{
	Intset    *a = (Intset *) PG_GETARG_POINTER(0);
	char    *b_str = PG_GETARG_CSTRING(1);
	Intset *b = cstring_to_intset(b_str);

	int len_a = (VARSIZE(a)-VARHDRSZ)/sizeof(int);
	int len_b = (VARSIZE(b)-VARHDRSZ)/sizeof(int);
	
	int array[len_a+len_b];
	int i;
	int j;
	int k=0;
	for (i=0; i<len_a; i++) {
		array[i] = a->arr[i];
		k++;
	}
	for (j=0; j<len_b; j++) {
		if (!(check_elements_array(b->arr[j],array,len_a)))
		{
			array[k] = b->arr[j];
			k++;
		}
	}

	Intset *intset = (Intset *) palloc(VARHDRSZ + sizeof(int)*k);
	SET_VARSIZE(intset, VARHDRSZ + sizeof(int)*k);
	memcpy(intset->arr, array, VARHDRSZ + sizeof(int)*k);
	PG_RETURN_POINTER(intset);
}


PG_FUNCTION_INFO_V1(intset_disjunction);

Datum
intset_disjunction(PG_FUNCTION_ARGS)
{
	Intset    *a = (Intset *) PG_GETARG_POINTER(0);
	Intset    *b = (Intset *) PG_GETARG_POINTER(1);

	int len_a = (VARSIZE(a)-VARHDRSZ)/sizeof(int);
	int len_b = (VARSIZE(b)-VARHDRSZ)/sizeof(int);
	
	int array[len_a+len_b];
	int i;
	int j;
	int ii = 0;
	int jj = 0;

	for (i=0;i<len_a;i++) {
		if (!(check_elements_array(a->arr[i],b->arr,len_b))) {
			array[ii] = a->arr[i];
			ii++;
			jj++;
		}
	}

	for (j=0;j<len_b;j++) {
		if (!(check_elements_array(b->arr[j],a->arr,len_a))) {
			array[jj] = b->arr[j];
			jj++;
		}
	}

	Intset *intset = (Intset *) palloc(VARHDRSZ + sizeof(int)*jj);
	SET_VARSIZE(intset, VARHDRSZ + sizeof(int)*jj);
	memcpy(intset->arr, array, VARHDRSZ + sizeof(int)*jj);
	PG_RETURN_POINTER(intset);
}


PG_FUNCTION_INFO_V1(intset_difference);

Datum
intset_difference(PG_FUNCTION_ARGS)
{
	Intset    *a = (Intset *) PG_GETARG_POINTER(0);
	Intset    *b = (Intset *) PG_GETARG_POINTER(1);

	int len_a = (VARSIZE(a)-VARHDRSZ)/sizeof(int);
	int len_b = (VARSIZE(b)-VARHDRSZ)/sizeof(int);
	
	int array[len_a];

	int i;
	int j=0;

	for (i=0; i<len_a; i++) {
		if (!(check_elements_array(a->arr[i],b->arr,len_b))) {
			array[j] = a->arr[i];
			j++;
		}
	}
	Intset *intset = (Intset *) palloc(VARHDRSZ + sizeof(int)*j);
	SET_VARSIZE(intset, VARHDRSZ + sizeof(int)*j);
	memcpy(intset->arr, array, VARHDRSZ + sizeof(int)*j);
	PG_RETURN_POINTER(intset);
}
