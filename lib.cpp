#include <cstdio>
#include <cstdarg>
#include <windows.h>
#include "main.h"
#include "lib.h"
#include "version.h"
#include "resource.h"
using namespace std;

void GetAppDir(char *str){
	int i;

	GetModuleFileName(NULL,str,256);
	i = strlen(str)-1;
	while(str[i] != '\\' && str[i] != '/' && i > 0)i--;
	str[i] = '\0';
}
//デフォルト指定可能、16進数なども可能版atoi
int atoi2(const char *str,int def){
	int i;
	char *s = NULL;

	if(str == NULL || !isdigit(str[0]))return def;
	i = strtol(str,&s,0);
	if(i == 0 && s != NULL && str[0]!='0')return def;

	return i;
}
//デフォルト指定可能版atof
float atof2(const char *str,float def){
	float i;

	if(str == NULL || !isdigit(str[0]))return def;
	i = atof(str);
	if(i == 0 && str[0] != '0')return def;

	return i;
}
//IEEE754形式小数点演算だと仮定して、不正なfloatではないかチェックする
//コンパイラおよびCPUに強く依存する。
int check_float(float f){
	int a;
	a = ((*(int *)&f)>>23)&0xFF;
	if(a > 254 || (*(int *)&f!=0 && a<108))return FALSE;
	return TRUE;
}
BOOL ReadProcessMemoryFloat(HANDLE hProcess,LPCVOID lpBaseAddress,LPVOID lpBuffer,DWORD nSize,LPDWORD lpNumberOfBytesRead){
	BOOL ret;
	float f;
	if(nSize != 4)return FALSE;
	ret = ReadProcessMemory(hProcess,lpBaseAddress,&f,4,lpNumberOfBytesRead);
	if(check_float(f)){
		*(float *)lpBuffer = f;
		return ret;
	} else {
		return FALSE;
	}
}

void SetAppDir(void){
	int i;
	char AppDir[256];

	GetModuleFileName(NULL,AppDir,256);
	i = strlen(AppDir)-1;
	while(AppDir[i] != '\\' && AppDir[i] != '/' && i > 0)i--;
	AppDir[i] = '\0';
	SetCurrentDirectory(AppDir);
}

void set_clipboard(char *s){
	HGLOBAL mem;
	char *str;

	mem = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, strlen(s)+1);
	str = static_cast<char*>(GlobalLock(mem));
	strcpy(str, s);
	GlobalUnlock(mem);

	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_TEXT,mem);
	CloseClipboard();
	return;
}

void to_lowstring(char *s){
	while(*s != '\0'){
		*s = tolower(*s);
		s++;
	}
}

void SetConsoleTitle2(char *str){
	static char *s = NULL;

	if(s != NULL){
		if(strncmp(s,str,strlen(str)) == 0)return;
		free(s);
	}
	s = static_cast<char*>(malloc(strlen(str)+1+strlen(OPEN_VERSION)+3));
	strcpy(s,str);
	strcat(s," - ");
	strcat(s,OPEN_VERSION);
	SetConsoleTitle(s);
}

void change_icon(int id){
	static int id_list[10] = {0};
	static long icon_list[10][2] = {0};
	int i;

	if(id_list[0] == 0){
		memset(id_list,0,sizeof(int)*10);
	}
	i = 0;
	while(i < 10){
		if(id_list[i] == id){
			break;
		}
		i++;
	}
	if(i==10){
		i = 0;
		while(id_list[i]!=0)i++;
		id_list[i] = id;
		icon_list[i][0] = (long)LoadImage(hInst,MAKEINTRESOURCE(id),IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
		icon_list[i][1] = (long)LoadImage(hInst,MAKEINTRESOURCE(id),IMAGE_ICON,32,32,LR_DEFAULTCOLOR);
	}
	SendMessage(my_wh,WM_SETICON,0,icon_list[i][0]);
	SendMessage(my_wh,WM_SETICON,1,icon_list[i][1]);
}

th_ini g_ini;


char *ini_value(const char *name){
	return g_ini.GetValue(name);
}
int ini_int(const char *name){
	return g_ini.GetInt(name);
}
int ini_int2(const char *name, int def){
	return g_ini.GetInt(name, def);
}
float ini_float(const char *name){
	return g_ini.GetFloat(name);
}
int load_ini(const char *fn){
	return g_ini.LoadFile(fn);
}
void ini_add(const char *section,const char *name,const char *value){
	g_ini.Add(section, name, value);
}
void create_ini(void){
}
