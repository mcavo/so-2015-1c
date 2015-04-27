#ifndef __DBLOADER_H__
#define __DBLOADER_H__

#include <stdio.h>
#include "dbconnector.h"

#define FAIL 0
#define SUCCESS 1

int load_fixture(char *db_path, char *fixt_path);

#endif
