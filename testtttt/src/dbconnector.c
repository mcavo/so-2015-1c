#include "../inc/dbconnector.h"

static void db_rlock(database_t *database, uint16_t movie_id, int first, int last);
static void db_wlock(database_t *database,uint16_t movie_id, int first, int last);
static void db_unlock(database_t *database,uint16_t movie_id, int first, int last);
static movie_t* db_find(database_t *database, char* name);

void db_movie_init(movie_t* movie, char* name) {
	strncpy(movie->name, name, MOVIE_NAME_LENGTH);
	memset(movie->tickets, 0, sizeof(movie->tickets));
}

database_t *db_open(char *path) {
	int fd = open(path, O_RDWR);
	struct stat st;
	fstat(fd, &st);
	void * movies = mmap(NULL,st.st_size,PROT_READ | PROT_WRITE, MAP_SHARED,fd,0);
	if (movies == MAP_FAILED) {
		printf("Map Failed %d\n", errno);
		return NULL;
	}
	database_t *db = (database_t*) malloc(sizeof(*db));
	db->fd = fd;
	db->st = st;
	db->count = st.st_size / sizeof(movie_t);
	db->movies = (movie_t*) movies;
	return db;
}

void db_close(database_t *db) {
	munmap(db->movies, db->st.st_size);
	close(db->fd);
	free(db);
}

static void db_lock(database_t * db, int start, int len, int type) {
	struct flock fl;
	fl.l_type = type;
	fl.l_whence = SEEK_SET;
	fl.l_start = start;
	fl.l_len = len;
	fl.l_pid = getpid();
	fcntl(db->fd, F_SETLKW, &fl);
}

static void db_rlock(database_t *db, uint16_t movie_id, int first, int last) {
	int start = movie_id * sizeof(movie_t) + sizeof(char)*(MOVIE_NAME_LENGTH + 1) + first*sizeof(ticket_t);
	len = (last-first+1)*sizeof(ticket_t);
	db_lock(db,start,len,F_RDLCK);
}
static void db_wlock(database_t *db,uint16_t movie_id, int first, int last) {
	int start = movie_id * sizeof(movie_t) + sizeof(char)*(MOVIE_NAME_LENGTH + 1) + first*sizeof(ticket_t);
	len = (last-first+1)*sizeof(ticket_t);
	db_lock(db,start,len,F_WRLCK);
}
static void db_unlock(database_t *db,uint16_t movie_id, int first, int last) {
	int start = movie_id * sizeof(movie_t) + sizeof(char)*(MOVIE_NAME_LENGTH + 1) + first*sizeof(ticket_t);
	len = (last-first+1)*sizeof(ticket_t);
	db_lock(db,start,len,F_UNLCK);
}

int db_buy_tickets (database_t *db, uint16_t movie_id, int first, int last) {
	int i;
	if (movie_i >= db->count)
		return ERR_NO_SUCH_MOVIE;
	if (last>=MOVIE_MAX_PLACES || first<0)
		return ERR_INVALID_TICKETS;
	if (last < first)
		return ERR_INVALID_RANGE;
	movie_t *movie = &(db->movies[movie_i]);
	db_wlock(db,movie_id,first,last);
	for (i=first; i<=last; i++) {
		if (movie->tickets[i] == 1) {
			db_unlock(db,movie_id,first,last);
			return ERR_OCCUPIED_SEATS;
		}
	}
	for (i=first; i<=last; i++) {
		movie->tickets[i] = 1;
	}
	db_unlock(db,movie_id,first,last);
	return 0;
}

//ticket_t* db_get_tickets(database_t *database, uint16_t movie_id);