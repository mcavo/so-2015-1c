#include "../inc/utils.h"

int get_server_pid(){
	FILE *fp;
	char str[60];
	int pid;
	fp = fopen("serverPid.txt" , "r");
	if(fp == NULL)
	{
	perror("Error opening file");
	return -1;
	}
	if( fgets (str, 60, fp)==NULL )
	return -1;
	pid= atoi(str);
	fclose(fp);
	return pid;
}