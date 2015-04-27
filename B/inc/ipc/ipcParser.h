#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define BOOL int
#define TRUE 1
#define FALSE 0

#define ERROR -1

#define LINE_LENGTH 100 /* including '/n' */
#define MAX_MESSAGES 5
#define MESSAGE_SIZE LINE_LENGTH * 3 /* 3 is the message line number */

#define START 0
#define SPACE 1
#define END 2

#define SEPARATOR_CHARACTER '-'
#define SERVER "../system_files/server-data.txt"

typedef unsigned short uint16_t;

typedef struct {
	uint16_t length;
    char *content;
    pid_t from;
} message_t;

void itoa ( int value, char * str );
char *get_fname(pid_t ipc_pid);
char *parse_line_string(char *msg);
void parse_string(char* line, char *str);
char *parse_line_int(int int_msg);