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
#include <stdarg.h>
#include <string.h>
#include <opt.h>
#include <file.h>


/* local/static prototypes */
static char const *prefix_dir(char const *dir, char const *file);


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
		return F_FILE;
	}
	else if(stat.st_mode & S_IFDIR)
		return F_DIR;

	return F_UNKNOWN;


err_1:
	(void)close(fd);

err_0:
	return F_ERROR;
}

char const *file_ext(char const *file){
	char const *s;


	for(s=file+strlen(file); s!=file; s--){
		if(*s == '.')
			return s + 1;
	}

	return 0x0;
}

int dir_apply(char const *root, char const **exts, int (*op)(char const *file, va_list args), ...){
	int r;
	va_list lst;


	va_start(lst, op);
	r = dir_vapply(root, exts, op, lst);
	va_end(lst);

	return r;
}

int dir_vapply(char const *root, char const **exts, int (*op)(char const *file, va_list args), va_list op_args){
	int fd;
	int i;
	int r;
	DIR *dp;
	struct dirent *e;
	char const *e_name;
	char const *e_ext;
	va_list args;


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
			if(!opts.recursive || strcmp(e->d_name, ".") == 0 || strcmp(e->d_name, "..") == 0)
				break;

			va_copy(args, op_args);
			r = dir_vapply(e_name, exts, op, args);
			va_end(args);
			break;

		case F_FILE:
			e_ext = file_ext(e->d_name);

			for(i=0; e_ext!=0x0 && exts[i]!=0x0; i++){
				if(strcmp(e_ext, exts[i]) == 0){
					va_copy(args, op_args);
					r = op(e_name, args);
					va_end(args);
					break;
				}
			}
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
	return r;
}


/* local functions */
static char const *prefix_dir(char const *dir, char const *file){
	char *s;
	size_t dlen;


	dlen = strlen(dir);
	s = malloc(dlen + strlen(file) + 2);

	if(s != 0x0)
		sprintf(s, "%s%s%s", dir, (dir[dlen - 1] == '/' ? "" : "/"), file);

	return s;
}
