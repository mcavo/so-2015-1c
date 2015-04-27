#include "../../inc/dbloader.h"

int main(int argc, char* argv[]) {

	char *db = argv[1];
	char *fix = argv[2];

	printf("Loading...\n");

	if(load_fixture(db, fix)==FAIL)
		printf("Fail to load from %s\n",fix);
	else
		printf("Ready!\n");
	
	return 0;

}
