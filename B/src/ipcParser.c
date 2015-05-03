#include "../inc/ipcParser.h"

static void reverse(char s[], int size);

char *get_fname(pid_t ipc_pid) {
	char *pid = malloc(sizeof(char*)*10);
    char * path;
    char * p = "";
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
