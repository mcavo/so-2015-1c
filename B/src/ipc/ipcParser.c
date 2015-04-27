#include "../inc/ipc_parser.h"

static void reverse(char s[], int size);

char *get_fname(pid_t ipc_pid) {
	char *pid = malloc(sizeof(char*)*10);
    char * path;
    char * p = "../system_files/";
    itoa(ipc_pid, pid);

    path = (char*)malloc((strlen(pid) + 1 + strlen(p))*sizeof(char));
    if(path==NULL)
        return path;
    memcpy(path, p, strlen(p) + 1);

    path = strcat(path, pid);
    path = strcat(path,".txt");
    free(pid);
    return path;
}

/* only for positive ints */
void itoa ( int value, char * str ) {
    int base = 10;
    int i = 0;  
    do {
        str[i++] = value % base + '0'; 
    } while ((value /= base) > 0);
    
    str[i] = '\0';
    reverse(str, i);
    return;
}

static void reverse(char s[], int size) {
    int i;
    for(i = 0; i < size/2; i++) {
        char aux = s[i];
        s[i] = s[size - i - 1];
        s[size - i - 1] = aux;
    }
    s[size] = '\0';
}

char *parse_line_string(char *msg) {
    int n = strlen(msg);
    char *str = calloc(LINE_LENGTH + 1 , sizeof(char));
    strcat(str, msg);
    printf("%s\n", str);
    for(; n < LINE_LENGTH - 1; n++) {
        str[n] = SEPARATOR_CHARACTER; 
    }
    str[n] = '\n';
    printf("line string:%s", str);
    return str;
}

char *parse_line_int(int int_msg) {
    printf("parse to line(int): %d\n", int_msg);
    char * str_aux = calloc(LINE_LENGTH + 1, sizeof(char));
    char * str_msg;
    itoa(int_msg, str_aux);
    str_msg = parse_line_string(str_aux);
    free(str_aux);
    return str_msg;
}

void parse_string(char* line, char *str) {
    //TODO: hay que garantizar que la información del método no tenga ni espacios ni \n
    int state = START;
    int i = 0;
    while(state != END){
        switch (line[i]) {
            case 0:
            case '\n':
                    if(state == SPACE) {
                        str[i - 1] = 0;
                    }
                    state = END;
                    break;
            case SEPARATOR_CHARACTER:
            case ' ':
                    if(state == SPACE) {
                        state = END;
                        str[i - 1] = 0;
                    } else {
                        state = SPACE;
                        str[i] = line[i];
                    }
                    break; 
            default:
                    str[i]=line[i];
                    state = START;
        }
        //printf("%c\n", buffer[i]);
        i++;
    }
}



