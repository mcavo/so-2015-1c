#include "../inc/dbloader.h"
#include <stdio.h>
static movie_t load_movie(char *line);

int load_fixture(char *db_path, char *fix_path) {

	FILE *db = fopen(db_path, "w");
	FILE *fix = fopen(fix_path, "r");

	if (fix == NULL) {
		return FAIL;
	}

	size_t size = 0;
	char *line = NULL;

	while (getline(&line, &size, fix) != -1) {
		movie_t movie = load_movie(line);
		fwrite(&movie, sizeof(movie), 1, db);
	}

	if (line)
		free(line);

	fclose(db);
	fclose(fix);
	
	return SUCCESS;
}

static movie_t load_movie(char *line) {
	movie_t movie;
	size_t line_len = strlen(line);
	if (line[line_len - 1] == '\n')
		line[line_len - 1] = 0;
	char *m_name = line;
	db_movie_init(&movie, m_name);
	return movie;
}

