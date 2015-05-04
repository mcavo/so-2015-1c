#include "../../inc/ipc/sockStream.h"
#include "../../inc/actions.h"

ipc_t* ipc_listen(int pid){
	ipc_t* sock = ipc_open(pid);

	if (listen(sock->sock, 0) == -1) {
		perror("listen");
		fprintf(stderr, "Ocurrio el error %s en ipc_open\n",strerror(errno));		
		exit(1);
	}
	return sock;
}

ipc_t *ipc_connect(int pid){
	 ipc_t* sock = malloc(sizeof(ipc_t));
	sock->server_id=pid;
	sock->id=getpid();
	return sock;	
}

ipc_t* ipc_open(int pid){
	int s, len;
	struct sockaddr_un local;
	ipc_t* sock = malloc(sizeof(ipc_t));  	
	char * path = get_fname(pid); 

	//sprintf(path, "%d", pid);

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

	sock->server_id=pid;
	sock->id=getpid();
	sock->sock = s;
	free(path);
	return sock;
}

void ipc_close(ipc_t *ipc){
	close(ipc->sock);
	free(ipc);
}

void ipc_send(ipc_t *ipc, uint16_t recipient, void *message, uint16_t len){
	int s, length;
	struct sockaddr_un remote;
	char * path = get_fname(ipc->server_id); 

	//sprintf(path, "%d", ipc->server_id);
	if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		fprintf(stderr, "Ocurrio el error %s en ipc_connect\n",strerror(errno));
		exit(1);
	}
	remote.sun_family = AF_UNIX;
	strcpy(remote.sun_path, path);

	length = strlen(remote.sun_path) + sizeof(remote.sun_family);

	if (connect(s, (struct sockaddr *)&remote, length) == -1) {
		perror("connect");
		fprintf(stderr, "Ocurrio el error %s en ipc_connect\n",strerror(errno));
		exit(1);
	}
	ipc->sock = s;

	message_t *msg = calloc(MESSAGE_SIZE, sizeof(char));
	msg->sender = ipc->id;
	msg->content_len = len;
	memcpy(msg->content, message, len);
	int i = send(ipc->sock, msg, MESSAGE_SIZE, 0);
	if ( i <= 0){
		perror("send");
		fprintf(stderr, "Ocurrio el error %s en ipc_send\n",strerror(errno));
		exit(1);
	}
	free(path);
	close(s);
}

message_t* ipc_receive(ipc_t *ipc){	
	struct sockaddr_un remote;
	int t,s2,nbytes;
	char *buf = calloc(MESSAGE_SIZE, sizeof(char));	
	t = sizeof(remote);
	if ((s2 = accept(ipc->sock, (struct sockaddr *)(&remote), (socklen_t *)&t)) == -1) {
		perror("accept");
		fprintf(stderr, "Ocurrio el error %s en ipc_receive\n",strerror(errno));				
		exit(1);
	}
	if((nbytes = recv(s2, buf, MESSAGE_SIZE, 0))==-1){
		perror("recv");
		fprintf(stderr, "Ocurrio el error %s en ipc_receive\n",strerror(errno));				
        	exit(1);
	}

	close(s2);
	message_t * msg = calloc(MESSAGE_SIZE, sizeof(char));
	msg->content_len = ((uint16_t *)buf)[1];
	msg->sender = ((uint16_t *)buf)[0];
	msg = realloc(msg, sizeof(uint16_t)*2 + msg->content_len);
	memcpy(msg -> content, buf + sizeof(uint16_t)*2, msg->content_len);

	free(buf);
    return msg;
}


