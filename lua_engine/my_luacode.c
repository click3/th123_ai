#include <stdio.h>
#include <stdlib.h>
#include "my_luacode.h"

int protect_flag = 0;

struct fopen_struct *fopen_struct_head=NULL;
struct fopen_struct *fopen_struct_current=NULL;

FILE *fopen2(const char *fn,char *flag) {
	FILE *fp;
	struct fopen_struct *a;

	fp = fopen(fn,flag);
	if(fp==NULL)return NULL;

	a = (struct fopen_struct *)malloc(sizeof(struct fopen_struct));
	if(fopen_struct_head==NULL) {
		fopen_struct_head = a;
		a->prev = NULL;
	} else {
		fopen_struct_current->next = a;
		a->prev = fopen_struct_current;
	}
	fopen_struct_current = a;
	a->fp = fp;
	a->next = NULL;

	return fp;
}

int fclose2(FILE *fp) {
	struct fopen_struct *a;

	a = fopen_struct_head;
	while(a!=NULL) {
		if(a->fp==fp) {
			if(a->prev!=NULL) {
				a->prev->next = a->next;
			}
			if(a->next!=NULL) {
				a->next->prev = a->prev;
			}
			if(a==fopen_struct_head) {
				fopen_struct_head = a->next;
			}
			if(a==fopen_struct_current) {
				fopen_struct_current = a->prev;
			}
			free(a);
			break;
		}
		a = a->next;
	}
	return fclose(fp);
}

void ffree(void) {
	struct fopen_struct *a,*b;

	a = fopen_struct_head;
	while(a!=NULL) {
		fclose(a->fp);
		b = a;
		a = a->next;
		free(b);
	}
	fopen_struct_head = NULL;
	fopen_struct_current = NULL;
}

