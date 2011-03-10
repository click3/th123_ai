
#include "main.h"

scriptEngine *engine;


void reload(void){
	MyWndActive(FALSE);
	engine->reload();
	timeEndPeriod(1);
	exit(0);
}
void reload2(void){
	engine->end();
	engine->init();
	engine->loadDialog();
	engine->start();
}

int OpenAI(const char *s){
	SetAppDir();
	protect_flag = 1;//luaをプロテクトモードにする
	if(ini_int2("th105_start",0)==1){
		ph = GetProcessHandle();
	}
	char *fn;
	if(s == NULL || strlen(s)==0 || strcmp(s,"dialog")==0){
		engine->loadDialog();
	} else if(strcmp(s, "auto")==0){
		char fn[256];
		snprintf(fn, sizeof(fn), "script/%s.ai",id2char2(my_data.char_id));
		engine->loadFile(fn);
	} else {
		engine->loadFile(s);
	}
	protect_flag = 0;
	SetAppDir();

	engine->start();
	engine->end();
	return 0;
}


