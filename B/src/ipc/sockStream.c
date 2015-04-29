#include "../../inc/ipc/sockStream.h"
#include "../../inc/actions.h"



ipc_t* ipc_listen(int pid){

}


ipc_t *ipc_connect(int pid){

	int s, len;
	struct sockaddr_un remote;
	ipc_t* sock = malloc(sizeof(ipc_t));
	char path[4]; 

	sprintf(path, "%d", pid);

	if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		fprintf(stderr, "Ocurrio el error %s en ipc_connect\n",strerror(errno));
		exit(1);
	}
	remote.sun_family = AF_UNIX;
	strcpy(remote.sun_path, path);

	len = strlen(remote.sun_path) + sizeof(remote.sun_family);

	if (connect(s, (struct sockaddr *)&remote, len) == -1) {
		perror("connect");
		fprintf(stderr, "Ocurrio el error %s en ipc_connect\n",strerror(errno));
		exit(1);
	}


	//sock->server_id=get_server_pid();
	//sock->id=pid;

        sock->sock = s;	
	
	return sock;	
}

ipc_t* ipc_open(int pid){

	int s, len;
	struct sockaddr_un local;
	ipc_t* sock = malloc(sizeof(ipc_t));  	
	char path[4]; 

	sprintf(path, "%d", pid);


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
	
	//sock->server_id=get_server_pid();
	//sock->id=pid;

        sock->sock = s;	
	return sock;

}

void ipc_close(ipc_t *ipc){
	close(ipc->sock);
	free(ipc);
}

void ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t len){

	message_t* msg = malloc(sizeof(message_t)+len);
	
	msg->sender = getpid();
	msg->content_len = len;
	memcpy(&(msg->content), message, len);
	printf("message en ipc_send: %d\n", *((uint8_t*)message));
	printf("msg.content en ipc_send: %d\n", (uint8_t)(msg->content));
	
	if (send(ipc->sock, msg, sizeof(*msg), 0) == -1){
		perror("send");
		fprintf(stderr, "Ocurrio el error %s en ipc_send\n",strerror(errno));
		exit(1);
	}
}

message_t* ipc_receive(ipc_t *ipc){
	
	struct sockaddr_un remote;
	int t,s2,nbytes;
	static char buf[MSG_SIZE];
	
	t = sizeof(remote);
	
	if ((s2 = accept(ipc->sock, (struct sockaddr *)&remote, &t)) == -1) {
		perror("accept");
		fprintf(stderr, "Ocurrio el error %s en ipc_receive\n",strerror(errno));				
		exit(1);
	}
	
	if((nbytes = recv(s2, &buf, MSG_SIZE, 0))==-1){
		perror("recv");
		fprintf(stderr, "Ocurrio el error %s en ipc_receive\n",strerror(errno));				
        	exit(1);
	}
	
	close(s2);

	message_t * msg = malloc(nbytes);
	memcpy(msg, buf, nbytes);

    return msg;
}


