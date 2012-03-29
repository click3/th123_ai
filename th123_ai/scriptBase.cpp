
#include "stdafx.h"

#pragma comment (lib,"Comdlg32.lib")

#define SCRIPT_NAME_EMPTY	("memory")

//private
void scriptEngine::convertFilePath(char *path) const {
	if(!loadable) {
		yield();
	}
	while(true) {
		char ss[256], *s = ::strstr(path,"%");
		if(s == NULL) {
			break;
		}
		if(strlen(s) < 4) {
			s[0] = '\0';
			break;
		} else if((s[1]!='c' && s[1]!='C') || (s[2]!='1' && s[2]!='2')) {
			::strmove(s, &s[3]);
			continue;
		}
		const unsigned int char_id_temp = getScriptValue(s[2]=='1' ? "my_char" : "enemy_char");
		const CHAR_ID char_id = static_cast<CHAR_ID>(char_id_temp);
		const char * const char_name = (s[1]=='c' ? ::ID2Char2(char_id) : ::ID2Char(char_id));
		char temp[256];
		SPRINTF(temp, "%s%s%s", path, char_name, s);
		::strcpy(path, ss);
	}
}


//public
bool scriptEngine::loadFile(const char *fileName) {
	if(fileName==NULL || fileName[0]=='\0' || strlen(fileName)>255) {
		return false;
	}
	char fullPath[256];
	strncpy(fullPath, fileName, sizeof(fullPath));
	convertFilePath(fullPath);
	char prevDir[256];
	GetCurrentDirectory(sizeof(prevDir), prevDir);
	char fn[256];
	{
		char dir[256],*s;
		strncpy(dir, fullPath, sizeof(dir));
		s = &dir[strlen(dir)];
		while(s>=dir && s[0]!='\\' && s[0]!='/') {
			s--;
		}
		if(s >= dir) {
			s[0] = '\0';
			SetCurrentDirectoryA(dir);
		}
		strncpy(fn,&s[1],sizeof(fn));
	}
	FILE *fp = fopen(fn, "rb");
	if(fp == NULL) {
		SetCurrentDirectoryA(prevDir);
		return false;
	}
  strncpy(scriptName, fullPath, _countof(scriptName));
	bool ret = loadFile(fp, fullPath);
	fclose(fp);
	SetCurrentDirectoryA(prevDir);
	return ret;
}
bool scriptEngine::loadFile(FILE *fp, const char *name) {
	if(fp == NULL) {
		return false;
	}
	int start = ftell(fp);
	fseek(fp,0,SEEK_END);
	int size = ftell(fp) - start;
	fseek(fp,start,SEEK_SET);
	char *data = (char *)malloc(size);
	if(data == NULL) {
		return false;
	}
	fread(data,1,size,fp);
	fseek(fp,start,SEEK_SET);
	bool ret = loadBuffer(data, size, name);
	free(data);
	return ret;
}
bool scriptEngine::loadFile(FILE *fp) {
	return loadFile(fp, SCRIPT_NAME_EMPTY);
}
bool scriptEngine::loadDialog() {
	OPENFILENAME ofn;
	char fileName[256] = "";
	std::string dir_str;
	org::click3::Utility::GetAppDir(dir_str);
	const boost::filesystem::path dir = boost::filesystem::path(dir_str) / "script";
	memset(&ofn,0,sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter = "AIファイル(*.ai)\0*.ai\0All files(*.*)\0*.*\0\0";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = sizeof(fileName);
	ofn.lpstrInitialDir = dir.string().c_str();
	ofn.Flags = OFN_FILEMUSTEXIST;
	ofn.lpstrTitle = "ファイルを開く";
	ofn.lpstrDefExt = "ai";

	if(!GetOpenFileName(&ofn)) {
		return false;
	}
	return loadFile(fileName);
}
bool scriptEngine::loadResource(int id) {
	char *buffer;
	int size;
	{
		HMODULE hm = GetModuleHandle(NULL);
		char name[64];
		snprintf(name, sizeof(name), "#%d",id);
		HRSRC hr = FindResource(hm, name, "RT_RCDATA");
		size = SizeofResource(hm, hr);
		buffer = static_cast<char*>(LockResource(LoadResource(hm, hr)));
		if(hm==NULL || hr==NULL || size==0 || buffer==NULL) {
			return false;
		}
	}
	return loadBuffer(buffer, size, "Resource");
}
bool scriptEngine::loadBuffer(const char *buffer, int size, const char *name) {
  if(buffer == NULL) {
    return false;
  }
  if(!loadable) {
    yield();
  }
  bool ret = execBuffer(buffer, size, name);
  return ret;
}
bool scriptEngine::loadBuffer(const char *buffer, int size) {
	return loadBuffer(buffer, size, SCRIPT_NAME_EMPTY);
}
bool scriptEngine::loadBuffer(const char *buffer, const char *name) {
	return loadBuffer(buffer, strlen(buffer),name);
}
bool scriptEngine::loadBuffer(const char *buffer) {
	return loadBuffer(buffer, strlen(buffer));
}
bool scriptEngine::reload() {
	if(scriptName[0] == '\0') {
		return false;
	}
	char fileName[256];
	strncpy(fileName, scriptName, _countof(fileName));
	end();
	init();
	bool ret = loadFile(fileName);
	if(ret) {
		start();
	}
	return ret;
}

void scriptEngine::init() {
	th105_active();
	if(! initScript()) {
		printf("ERROR:scriptEngine init error(initScript return is false)\n");
		getchar();
		exit(0);
	}
}
void scriptEngine::start() {
	if(! loadable) {
		printf("ERROR:scriptEngine start error(non load script)\n");
		getchar();
		exit(0);
	}
	th105_active();
	startScript();
}
void scriptEngine::end() {
	if(! loadable) {
		return;
	}
	endScript();
	loadable = false;
	scriptName[0] = '\0';
}
