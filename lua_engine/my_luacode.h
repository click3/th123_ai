#include <stdio.h>

struct fopen_struct{
	FILE *fp;
	struct fopen_struct *next;
	struct fopen_struct *prev;
};

FILE *fopen2(const char *fn,char *flag);
int fclose2(FILE *fp);
void ffree(void);

extern int protect_flag;
