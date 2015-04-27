#include "../../inc/ipc/sockStream.h"

ipc_t *ipc_listen(int pid){
		

}

ipc_t *ipc_connect(int pid,int server_id,char* path){
	
}

ipc_t* ipc_open(int pid, char* path){

	int s, s2, t, len;
	struct sockaddr_un local, remote;
	char str[100];
	ipc_t* sock = malloc(sizeof(ipc_t));  	

	if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		fprintf(stderr, "Ocurrio el error %s en ipc_open\n",strerror(errno));		
		exit(1);
	}

	local.sun_family = AF_UNIX;
	strcpy(local.sun_path, path);
	unlink(local.sun_path);
	len = strlen(local.sun_path) + sizeof(local.sun_family);

	if (bind(s, (struct sockaddr *)&local, len) == -1) {
		perror("bind");
		fprintf(stderr, "Ocurrio el error %s en ipc_open\n",strerror(errno));		
		exit(1);
	}

	if (listen(s, 5) == -1) {
		perror("listen");
		fprintf(stderr, "Ocurrio el error %s en ipc_open\n",strerror(errno));		
		exit(1);
	}
	
        sock->sock = s;	
	
	return sock;

}

void ipc_close(ipc_t *ipc){
	
}

void ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t len){

}

message_t* ipc_receive(ipc_t *ipc){
	
	struct sockaddr_un remote;
	int t,s2,nbytes;
	
	t = sizeof(remote);
	
	if ((s2 = accept(s, (struct sockaddr *)&remote, &t)) == -1) {
		perror("accept");
		fprintf(stderr, "Ocurrio el error %s en ipc_receive\n",strerror(errno));				
		exit(1);
	}
	
	if((nbytes = recv(s2, str, 100, 0))==-1){
		perror("recv");
		fprintf(stderr, "Ocurrio el error %s en ipc_receive\n",strerror(errno));				
        exit(1);
	}
	
	close(s2);	
}

static char buf[MSG_SIZE];
		int nbytes;

	    if ((nbytes = msgrcv(ipc->queue, &buf, MSG_SIZE, ipc->id, 0)) == -1) {
            perror("msgrcv");
			fprintf(stderr, "Ocurrio el error %s en ipc_receive\n",strerror(errno));				
            exit(1);
        }
		
		message_t * msg = malloc(nbytes);
		memcpy(msg, buf + sizeof(long), nbytes);

        return msg;



