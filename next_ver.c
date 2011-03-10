#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

char list[][256] = {
	"th123_ai.exe",	"document.txt", "th123_ai.ini",
	"History.txt",	"motion.txt",	"Readme.txt",
	"skill.txt",		"spell.txt",
	"SWRSAddr.ini",	"SwrAddr.ini",	"card.txt",
	""};
char src[][256] = {
	"main.cpp",	"script.cpp",		"keyboard.cpp",
	"lib.cpp",	"network.cpp",	"consts.h",
	"address.cpp","scriptBase.cpp",	"scriptLua.cpp",
	"main.h",	"script.h",		"keyboard.h",
	"lib.h",	"network.h",
	"address.h",	"dik_list.h",		"version.h",
	"resource.h",	"scriptLua.h",	"scriptBase.h",
	"makefile",	"include",
	"lua_src",	"api.ai",
	"th105_ai.rc",	"th105_ai.ico",	"th105_ai2.ico",
	"th105_ai2.ico",
	""};

char AppDir[256];
void SetAppDir(void){
	int i;

	GetModuleFileName(NULL,AppDir,256);
	i = strlen(AppDir)-1;
	while(AppDir[i] != '\\' && AppDir[i] != '/' && i > 0)i--;
	AppDir[i] = '\0';
	SetCurrentDirectory(AppDir);
}

char *enumFile(char *str,int i){
	static char s[256];
	HANDLE hSearch;
	WIN32_FIND_DATA fd;

	hSearch = FindFirstFile(str, &fd );
	if( hSearch == INVALID_HANDLE_VALUE )return NULL;
	while(i > 0){
		if(!FindNextFile( hSearch, &fd )){
			if( GetLastError() == ERROR_NO_MORE_FILES ){
				return NULL;
			}
		}
		i--;
	}
	FindClose( hSearch );
	strcpy(s,fd.cFileName);
	return s;
}

int CopyFiles(char *src,char *dest,int flag){
	int i;
	char *s,a[256],fn[256],fn2[256];

	if(!CopyFile(src,dest,flag)){
		CreateDirectory(dest,NULL);
		sprintf(a,"%s\\*",src);
		i = 0;
		while(1){
			s = enumFile(a,i);
			if(s==NULL)break;
			if(s[0] != '.'){
				sprintf(fn,"%s\\%s",src,s);
				sprintf(fn2,"%s\\%s",dest,s);
				CopyFile(fn,fn2,flag);
			}
			i++;
		}
	}
	return TRUE;
}

main(){
	FILE *fp;
	char str[256];
	char s[2048],temp[256],fn[256];
	int i;
	char ver[64];

	SetAppDir();

	fp = fopen("history.txt","r");
	if(fp==NULL)return 1;
	fgets(str,256,fp);
	fclose(fp);
	if(str[strlen(str)-1] == '\n'){
		str[strlen(str)-1] = '\0';
	}
	i = 0;
	while(str[i] != '\t' && str[i] != '\0')i++;
	strcpy(ver,&str[i+1]);

	sprintf(temp,"%s\\%s",AppDir,ver);
	if(SetCurrentDirectory(temp)){
		printf("ä˘Ç…äYìñÇ∑ÇÈVersionÇ™ë∂ç›ÇµÇ‹Ç∑\n");
		return 1;
	}
	CreateDirectory(temp,NULL);
	strcpy(s,"F:\\Lhaplus\\Lhaplus.exe /c:zip");

	i = 0;
	while(list[i][0] != '\0'){
		sprintf(fn,"%s\\%s",AppDir,list[i]);
		sprintf(temp," \"%s\"",fn);
		strcat(s,temp);
		sprintf(temp,"%s\\%s\\%s",AppDir,ver,list[i]);
		CopyFiles(fn,temp,TRUE);
		i++;
	}
	system(s);
	i = 0;
	while(src[i][0] != '\0'){
		sprintf(fn,"%s\\%s",AppDir,src[i]);
		sprintf(temp,"%s\\%s\\%s",AppDir,ver,src[i]);
		CopyFiles(fn,temp,TRUE);
		i++;
	}
	return 0;
}
