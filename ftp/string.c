#include<stdlib.h>
#include<string.h>
#include<stdio.h>
char **strsplit(char **str1,char *str2,char **array)
{
	int loop,len,count = 0;
	char *tmp=NULL;
	for(loop = 0; loop < 50; loop++) {
		tmp = strsep(str1," \n");
		if((tmp != NULL) && (strcmp(tmp,"")!=0)) {
			*(array + count) = malloc(strlen(tmp));
			strcpy(*(array+count),tmp);
			count++;
		}
		if(tmp == NULL)
			break;
	}
	return array;
}
