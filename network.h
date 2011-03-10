#pragma once
#include <windows.h>

void ErrorPost(const char *url,const char *s);
char *get(const char *address,int *data_len=NULL);
bool AIZipPost(const char *url,const char *fn,int char_id,const char *signature,const char *name,const char *description,const char *root);
bool PostDeleteMethod(const char *url,int char_id,const char *signature,const char *name,const char *description,const char *root);
bool PostScriptError(const char *url,const char *signature, const char *s);
