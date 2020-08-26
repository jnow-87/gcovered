/**
 * Copyright (C) 2020 Jan Nowotsch
 * Author Jan Nowotsch	<jan.nowotsch@gmail.com>
 *
 * Released under the terms of the GNU GPL v2.0
 */



#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <file.h>


#include <errno.h>
#include <string.h>


/* local/static prototypes */
static char const *prefix_dir(char const *s0, char const *s1);


/* global functions */
file_type_t file_type(char const *file){
	int fd;
	char const *s;
	struct stat stat;


	/* get file stat */
	fd = open(file, O_RDONLY);

	if(fd < 0)
		goto err_0;

	if(fstat(fd, &stat) != 0)
		goto err_1;

	(void)close(fd);

	/* check file type and extension */
	if(stat.st_mode & (S_IFREG | S_IFLNK)){
		for(s=file+strlen(file); s!=file; s--){
			if(*s == '.'){
				if(strcmp(s, ".gcov") == 0)
					return F_GCOV;
			}
		}
	}
	else if(stat.st_mode & S_IFDIR)
		return F_DIR;

	return F_UNKNOWN;


err_1:
	(void)close(fd);

err_0:
	return F_ERROR;
}

int dir_process(char const *root, int (*op)(char const *file)){
	int fd;
	int r;
	DIR *dp;
	struct dirent *e;
	char const *e_name;


	r = -1;

	dp = opendir(root);

	if(dp == 0x0)
		goto end_0;

	fd = dirfd(dp);

	if(fd < 0)
		goto end_1;

	r = 0;

	while(r == 0 && (e = readdir(dp)) != 0x0){
		e_name = prefix_dir(root, e->d_name);

		switch(file_type(e_name)){
		case F_DIR:
			if(strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0)
				break;

			r = dir_process(e_name, op);
			break;

		case F_GCOV:
			r = op(e_name);
			break;

		default:
			// skip file
			break;
		}

		free((void*)e_name);
	}


end_1:
	(void)closedir(dp);

end_0:
	if(r != 0)
		printf("err: %s: %s\n", root, strerror(errno));

	return r;
}


/* local functions */
static char const *prefix_dir(char const *dir, char const *file){
	char *s;


	s = malloc(strlen(dir) + strlen(file) + 1);

	if(s != 0x0)
		sprintf(s, "%s/%s", dir, file);

	return s;
}
