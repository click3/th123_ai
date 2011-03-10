
#pragma once

#include <atlbase.h>
#include <stdio.h>

#define snprintf(str,size,format, ...)	sprintf(str,format,__VA_ARGS__);
void GetAppDir(char *str);
int atoi2(const char *str,int def);
float atof2(const char *str,float def);
int check_float(float f);
BOOL ReadProcessMemoryFloat(HANDLE hProcess,LPCVOID lpBaseAddress,LPVOID lpBuffer,DWORD nSize,LPDWORD lpNumberOfBytesRead);
void SetAppDir(void);
void set_clipboard(char *s);
void to_lowstring(char *s);
void SetConsoleTitle2(char *str);
void change_icon(int id);
#define strmove(s1,s2)	memmove(s1,s2,strlen(s2)+1)
int del_space(char *s);




class ini
{
public:
	ini(){
		head = NULL;
	}
	~ini(){
		LIST *a,*b;

		a = head;
		while(a != NULL){
			b = a;
			a = a->next;
			free(b);
		}
		head = NULL;
	}

	virtual char *GetValue(const char *name){
		LIST *a;
		a = get_list(name);
		if(a==NULL)return NULL;
		return a->value;
	}
	virtual int GetInt(const char *name,int def = 0){
		return atoi2(GetValue(name),def);
	}
	virtual float GetFloat(const char *name){
		return atof2(GetValue(name),0.0);
	}


	int LoadFile(const char *fn){
		FILE *fp;
		char section[256] = "root";
		char s[256],*value;
		int i,count;

		fp = fopen(fn,"r");
		if(fp == NULL)return 0;
		count = 0;
		while(fgets(s,256,fp) != NULL){
			if(s[strlen(s)-1] == '\n')s[strlen(s)-1] = '\0';
			if(strcmp(section,"SwrName") != 0 && strcmp(section,"SWRSName") != 0)del_space(s);//SwrNameÇÃèÍçáÇÕãÛîíçÌèúÇ»Çµ
			if(*s == ';' || *s == '\0' || strncmp(s,"//",2)==0 || *s == '#'){
				continue;
			} else if(*s == '['){
				i = 1;
				while(s[i] != ']'){
					if(s[i] == '\0'){
						fclose(fp);
						return -1;
					}
					i++;
				}
				s[i] = '\0';
				strcpy(section,&s[1]);
			} else {
				i = 0;
				while(s[i] != '='){
					if(s[i] == '\0'){
						fclose(fp);
						return -2;
					}
					i++;
				}
				s[i] = '\0';
				value = &s[i+1];
				Add(section,s,value);
				count++;
			}
		}
		fclose(fp);
		return count;
	}

	virtual void Add(const char *section,const char *name,const char *value){
		LIST *a;

		a = get_list(name);
		if(a == NULL){
			a = static_cast<LIST*>(malloc(sizeof(LIST)));
			if(head == NULL){
				head = a;
			} else {
				tail->next = a;
			}
			tail = a;

			strncpy(a->section, section, sizeof(a->section));
			strncpy(a->name, name, sizeof(a->name));
			a->next = NULL;
		}
		strncpy(a->value, value, sizeof(a->value));
		//printf("%s->%s = %s\n",a->section,a->name,a->value);
	}
private:
	typedef struct list{
		char section[256];
		char name[256];
		char value[256];
		struct list *next;
	}LIST;
	LIST *head;
	LIST *tail;

	int del_space(char *s){
		int count = 0;
		while(*s != '\0'){
			if(*s == ' ' || *s == '\t'){
				memmove(s, &s[1], strlen(&s[1])+1);
				count++;
			} else {
				s++;
			}
		}
		return count;
	}

	LIST *get_list(const char *name){
		LIST *a;

		a = head;
		while(a != NULL){
			if(strcmp(a->name,name)==0){
				return a;
			}
			a = a->next;
		}
		return NULL;
	}
};

class th_ini : public ini
{
public:
	enum TYPE{
		TYPE_NONE,
		TYPE_TH105_SWR,
		TYPE_TH123_SOKU
	};
	TYPE type;

	th_ini(){
		type = TYPE_NONE;
	}
	th_ini(TYPE a){
		type = a;
	}

	bool SetType(TYPE a){
		if(a!=TYPE_NONE && a!=TYPE_TH105_SWR && a!=TYPE_TH123_SOKU){
			AtlTrace("Error ini::SetType param error( type = %d)\n",a);
			ATLASSERT(false);
			return false;
		}
		type = a;
		return true;
	}

	char *GetValue(const char *name){
		if(strncmp(name, "ADDR_", 4) != 0){
			return ini::GetValue(name);
		}
		char temp[256] = "";
		if(type == TYPE_TH105_SWR){
			strcpy(temp, "SWR_");
		} else if(type == TYPE_TH123_SOKU){
			strcpy(temp, "SWRS_");
		} else {
			AtlTrace("Error ini::GetValue type error( type = %d)\n",type);
			ATLASSERT(false);
			return "";
		}
		strcat(temp, name);
		//printf("%d:%s\t%s\t%s\n",type,name,temp,ini::GetValue(temp));
		return ini::GetValue(temp);
	}
};

extern th_ini g_ini;

char *ini_value(const char *name);
int ini_int(const char *name);
int ini_int2(const char *name, int def);
float ini_float(const char *name);
int load_ini(const char *fn);
void ini_add(const char *section,const char *name,const char *value);
void create_ini(void);
