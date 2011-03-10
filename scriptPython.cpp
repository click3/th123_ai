
#include "scriptPython.h"
#include "main.h"

#define METH_VARARGS 0

scriptPython *scriptPython::instance = NULL;

//public
	scriptPython *scriptPython::getCurrentInstance(){
		if(instance == NULL){
			instance = new scriptPython();
		}
		return instance;
	}
/*
	int scriptPython::getScriptValue(const char *name)const{
		lua_Number ret;
		lua_getglobal(state, name);
		ret = lua_tonumber(state, -1);
		lua_pop(state, 1);
		return ret;
	}
	void scriptPython::setScriptValue(const char *name, double value){
		lua_pushnumber(state, value);
		lua_setglobal(state, name);
	}
	void scriptPython::setScriptValueBool(const char *name, bool value){
		lua_pushboolean(state, value);
		lua_setglobal(state, name);
	}
	void scriptPython::call(const char *name){
		lua_getglobal(state, name);
		lua_call(state, 0, 0);
	}

//protected
	int scriptPython::panic_reload(PyObject* self){
		int n = lua_gettop(L);
		int i;

		MyWndActive(TRUE);
		i = 0;
	        FILE *fp = fopen("error_tmp.tmp147258369","a");
		while(i < n){
			fprintf(fp,"PANIC: %s\n",lua_tostring(L,i+1));
			i++;
		}
		fclose(fp);

	        fp = fopen("error_tmp.tmp147258369","rb");//改行コードを\r\nにしたいのであえてrbで読む
		char str[4096];
		int size = fread(str,1,sizeof(str),fp);
		fclose(fp);
		str[size] = '\0';
		DeleteFile("error_tmp.tmp147258369");
		printf("%s",str);

		printf("----------\n");
		printf("ENTERで再読み込みします\n");
		if(strlen(aimanager_signature)==13){
			PostScriptError(URL_SCRIPT_ERROR_CGI,aimanager_signature,str);
		}
		while(GetKeyState(VK_RETURN)>=0 || my_wh!=GetForegroundWindow()){
			reload_check();
			Sleep(50);
		}
		while(GetKeyState(VK_RETURN)<0){Sleep(50);}
		scriptPython::getCurrentInstance()->reload();
		return 0;
	}

	bool scriptPython::initScript(){
		// Lua の言語エンジンを初期化
		state = luaL_newstate();
		// Luaのライブラリを使えるようにする
		luaL_openlibs(state);

		lua_atpanic(state, &scriptPython::panic_reload);

		set_weather_delay(300);
		set_key_delay(0);
		set_data_delay(0);

		lua_register(state, "_yield",			&scriptPython::lua_yield);
		lua_register(state, "key_reset",		&scriptPython::lua_key_reset);
		lua_register(state, "key_off",			&scriptPython::lua_key_off);
		lua_register(state, "key_on",			&scriptPython::lua_key_on);
		lua_register(state, "require",			&scriptPython::lua_require);
		lua_register(state, "get_skill_lv",		&scriptPython::lua_get_skill_lv);
		lua_register(state, "get_card_id",		&scriptPython::lua_get_card_id);
		lua_register(state, "get_card_cost",		&scriptPython::lua_get_card_cost);
		lua_register(state, "get_card_cost2",		&scriptPython::lua_get_card_cost2);
		lua_register(state, "set_key_delay",		&scriptPython::lua_set_key_delay);
		lua_register(state, "set_data_delay",		&scriptPython::lua_set_data_delay);
		lua_register(state, "get_obj_data",		&scriptPython::lua_get_obj_data);
		lua_register(state, "set_weather_delay",	&scriptPython::lua_set_weather_delay);
		lua_register(state, "get_opt_xy",		&scriptPython::lua_get_opt_xy);
		lua_register(state, "get_deck_list",		&scriptPython::lua_get_deck_list);
		lua_register(state, "get_key_stat",		&scriptPython::lua_get_key_stat);
		lua_register(state, "get_key_map",		&scriptPython::lua_get_key_map);
		lua_register(state, "get_real_key_stat",	&scriptPython::lua_get_real_key_stat);
		lua_register(state, "get_special_data",		&scriptPython::lua_get_special_data);
		lua_register(state, "get_obj_attackarea",	&scriptPython::lua_get_obj_attackarea);
		lua_register(state, "get_attackarea",		&scriptPython::lua_get_attackarea);
		lua_register(state, "get_hitarea",		&scriptPython::lua_get_hitarea);
		lua_register(state, "get_correction",		&scriptPython::lua_get_correction);
		lua_register(state, "get_fflags",		&scriptPython::lua_get_fflags);
		lua_register(state, "get_aflags",		&scriptPython::lua_get_aflags);
		lua_register(state, "getfunc",			&scriptPython::lua_getfunc);
		lua_register(state, "get_ini_value",		&scriptPython::lua_get_ini_value);
		lua_register(state, "get_ini_int",		&scriptPython::lua_get_ini_int);
		lua_register(state, "get_ini_int2",		&scriptPython::lua_get_ini_int2);
		lua_register(state, "load_ini",			&scriptPython::lua_load_ini);
		lua_register(state, "get_version",		&scriptPython::lua_get_version);
		lua_register(state, "get_key_stat2",		&scriptPython::lua_get_key_stat2);
		lua_register(state, "get_key_map2",		&scriptPython::lua_get_key_map2);
		return loadResource(LUA_SCRIPT_RC);
	}
	void scriptPython::startScript(){
		call("api_main");
	}
	void scriptPython::endScript(){
		ffree();
		lua_close(state);
	}

	bool scriptPython::execBuffer(const char *buffer, int size, const char *name){
		int a = luaL_loadbuffer(state, buffer, size, name);
		if(a != 0){
		        FILE *fp = fopen("error_tmp.tmp147258369","a");
			fprintf(fp,"Error:require(%s)\n",name);
			if(a==LUA_ERRFILE){
				fprintf(fp,"fileread failed(ファイル読み取りに失敗しました)\n");
			} else if(a==LUA_ERRSYNTAX){
				fprintf(fp,"syntax error(構文エラーが発生しました)\n");
			} else if(a==LUA_ERRMEM ){
				fprintf(fp,"memory malloc failed(メモリー割り当てに失敗しました)\n");
			} else {
				fprintf(fp,"unknown error %d(想定外のエラーに遭遇しました)\n",a);
			}
			fclose(fp);
			return false;
		}
		a = lua_pcall(state,0,0,0);
		if(a != 0){
		        FILE *fp = fopen("error_tmp.tmp147258369","a");
			fprintf(fp,"Error:call(%s)\n",name);
			if(a==LUA_ERRRUN){
				fprintf(fp,"run error(実行時エラー)\n");
			} else if(a==LUA_ERRERR){
				fprintf(fp,"errorhandler error(エラーハンドラ実行中にエラーしました)\n");
			} else if(a==LUA_ERRMEM){
				fprintf(fp,"memory malloc failed(メモリー割り当てに失敗しました)\n");
			} else {
				fprintf(fp,"unknown error %d(想定外のエラーに遭遇しました)\n",a);
			}
			fclose(fp);
			return false;
		}
		return true;
	}



	int scriptPython::lua_yield(PyObject* self){
		int n = lua_gettop(L);

		if(n!=0){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		yield();
		return 0;
	}
	int scriptPython::lua_key_reset(PyObject* self){
		int n = lua_gettop(L);

		if(n!=0){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		key_reset();
		return 0;
	}
	int scriptPython::lua_key_off(PyObject* self){
		int n = lua_gettop(L);

		if(n!=1 || !lua_isnumber(L,1)){
			lua_pushstring(L,"引数が正しくありません\n");
			lua_error(L);
		}
		key_off(static_cast<int>(lua_tonumber(L,1)));
		return 0;
	}
	int scriptPython::lua_key_on(PyObject* self){
		int n = lua_gettop(L);

		if(n!=1 || !lua_isnumber(L,1)){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		key_on(static_cast<int>(lua_tonumber(L,1)));
		return 0;
	}
	int scriptPython::lua_require(PyObject* self){
		int n = lua_gettop(L);

		if(n!=1 || !lua_isstring(L,1)){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		scriptPython::getCurrentInstance()->loadFile(lua_tostring(L,1));
		return 0;
	}
	int scriptPython::lua_get_skill_lv(PyObject* self){
		int n = lua_gettop(L);
		lua_Number a;

		if(n!=2 || !lua_isnumber(L,1) || !lua_isnumber(L,2)){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		a = GetSkillLv(static_cast<int>(lua_tonumber(L,1)), static_cast<int>(lua_tonumber(L,2)));
		lua_pushnumber(L,a);
		return 1;
	}
	int scriptPython::lua_get_card_id(PyObject* self){
		int n = lua_gettop(L);
		lua_Number a;

		if(n!=2 || !lua_isnumber(L,1) || !lua_isnumber(L,2)){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		a = GetCardId(static_cast<int>(lua_tonumber(L,1)), static_cast<int>(lua_tonumber(L,2)));
		lua_pushnumber(L,a);
		return 1;
	}
	int scriptPython::lua_get_card_cost(PyObject* self){
		int n = lua_gettop(L);
		lua_Number a;

		if(n!=2 || !lua_isnumber(L,1) || !lua_isnumber(L,2)){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		a = GetCardCost(static_cast<int>(lua_tonumber(L,1)), static_cast<int>(lua_tonumber(L,2)));
		lua_pushnumber(L,a);
		return 1;
	}
	int scriptPython::lua_get_card_cost2(PyObject* self){
		int n = lua_gettop(L);
		lua_Number a;

		if(n!=2 || !lua_isnumber(L,1) || !lua_isnumber(L,2)){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		a = GetCardCost2(static_cast<int>(lua_tonumber(L,1)), static_cast<int>(lua_tonumber(L,2)));
		lua_pushnumber(L,a);
		return 1;
	}
	int scriptPython::lua_set_key_delay(PyObject* self){
		int n = lua_gettop(L);

		if(n!=1 || !lua_isnumber(L,1)){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		set_key_delay(static_cast<int>(lua_tonumber(L,1)));
		return 0;
	}
	int scriptPython::lua_set_data_delay(PyObject* self){
		int n = lua_gettop(L);

		if(n!=1 || !lua_isnumber(L,1)){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		set_data_delay(static_cast<int>(lua_tonumber(L,1)));
		return 0;
	}
	int scriptPython::lua_get_obj_data(PyObject* self){
		int n = lua_gettop(L);

		if(n!=2 || !lua_isnumber(L,1) || !lua_isnumber(L,2) || lua_tointeger(L,1)<0 || lua_tointeger(L,1)>1){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		int p = lua_tointeger(L,1);
		obj *a;
		if(p==0){
			a = my_data.GetObject(lua_tointeger(L,2));
		} else {
			a = enemy_data.GetObject(lua_tointeger(L,2));
		}
		if(a==NULL){
			return 0;
		}
		lua_pushnumber(L,(lua_Number)a->action);
		lua_pushnumber(L,(lua_Number)a->x);
		lua_pushnumber(L,(lua_Number)a->y);
		lua_pushnumber(L,(lua_Number)a->hp);
		lua_pushnumber(L,(lua_Number)a->img_no);
		lua_pushnumber(L,(lua_Number)a->frame);
		lua_pushnumber(L,(lua_Number)a->speed.x);
		lua_pushnumber(L,(lua_Number)a->speed.y);
		lua_pushnumber(L,(lua_Number)a->base_addr);
		lua_pushnumber(L,(lua_Number)a->attackarea_n);
		lua_pushnumber(L,(lua_Number)a->hitarea_n);
		return 11;
	}
	int scriptPython::lua_get_obj_attackarea(PyObject* self){
		int n = lua_gettop(L);
		if(n!=3 || !lua_isnumber(L,1) || !lua_isnumber(L,2) || !lua_isnumber(L,3) || lua_tointeger(L,1)<0 || lua_tointeger(L,1)>1){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		int p = lua_tointeger(L,1);
		obj *a;
		if(p==0){
			a = my_data.GetObject(lua_tointeger(L,2));
		} else {
			a = enemy_data.GetObject(lua_tointeger(L,2));
		}
		if(a==NULL){
			return 0;
		}
		int i = lua_tointeger(L,3);
		BOX *b;
		if(a->attackarea_n>i && i>=0){
			b = &a->attackarea[i];
			lua_pushnumber(L,(lua_Number)b->left);
			lua_pushnumber(L,(lua_Number)b->top);
			lua_pushnumber(L,(lua_Number)b->right);
			lua_pushnumber(L,(lua_Number)b->bottom);
			return 4;
		}
		return 0;
	}
	int scriptPython::lua_get_obj_hitarea(PyObject* self){
		int n = lua_gettop(L);

		if(n!=3 || !lua_isnumber(L,1) || !lua_isnumber(L,2) || !lua_isnumber(L,3) || lua_tointeger(L,1)<0 || lua_tointeger(L,1)>1){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		int p = lua_tointeger(L,1);
		obj *a;
		if(p==0){
			a = my_data.GetObject(lua_tointeger(L,2));
		} else {
			a = enemy_data.GetObject(lua_tointeger(L,2));
		}
		if(a==NULL){
			return 0;
		}
		int i = lua_tointeger(L,3);
		BOX *b;
		if(a->hitarea_n>i && i>=0){
			b = &a->hitarea[i];
			lua_pushnumber(L,(lua_Number)b->left);
			lua_pushnumber(L,(lua_Number)b->top);
			lua_pushnumber(L,(lua_Number)b->right);
			lua_pushnumber(L,(lua_Number)b->bottom);
			return 4;
		}
		return 0;
	}
	int scriptPython::lua_get_correction(PyObject* self){
		int n = lua_gettop(L);

		if(n!=2 || !lua_isnumber(L,1) || !lua_isnumber(L,2)){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		lua_pushboolean(L,get_correction(static_cast<int>(lua_tonumber(L,1)), static_cast<int>(lua_tonumber(L,2))));
		return 1;
	}
	int scriptPython::lua_set_weather_delay(PyObject* self){
		int n = lua_gettop(L);

		if(n!=1 || !lua_isnumber(L,1)){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		set_weather_delay(static_cast<int>(lua_tonumber(L,1)));
		return 0;
	}
	int scriptPython::lua_get_opt_xy(PyObject* self){
		int n = lua_gettop(L);

		if(n!=2 || !lua_isnumber(L,1) || !lua_isnumber(L,2) || lua_tointeger(L,1)<0 || lua_tointeger(L,1)>1){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		int p = lua_tointeger(L,1);
		obj *a;
		if(p==0){
			a = my_data.GetOptionObject(lua_tointeger(L,2));
		} else {
			a = enemy_data.GetOptionObject(lua_tointeger(L,2));
		}
		if(a==NULL){
			return 0;
		}
		lua_pushnumber(L,(lua_Number)a->x);
		lua_pushnumber(L,(lua_Number)a->y);
		return 2;
	}
	int scriptPython::lua_get_deck_list(PyObject* self){
		int n = lua_gettop(L);
		short *a;
		int i;
		if(n!=1 || !lua_isnumber(L,1)){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		a = GetDeckList(0);
		if(a==NULL)return 0;
		i = 0;
		while(*a != -1){
			lua_pushnumber(L,*a);
			i++;
			a++;
		}
		return i;
	}
	int scriptPython::lua_get_key_stat(PyObject* self){
		int n = lua_gettop(L);
		lua_Number a;

		if(n!=1 || !lua_isnumber(L,1)){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		a = get_key_stat(lua_tonumber(L,1));
		lua_pushnumber(L,a);
		return 1;
	}
	int scriptPython::lua_get_key_map(PyObject* self){
		int n = lua_gettop(L);
		int i;
		lua_Number a;

		if(n!=0){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		i = 0;
		while(i < 10){
			a = get_key_stat(i);
			lua_pushnumber(L,a);
			i++;
		}
		return 10;
	}
	int scriptPython::lua_get_real_key_stat(PyObject* self){
		int n = lua_gettop(L);
		lua_Number a;

		if(n!=1 || !lua_isnumber(L,1)){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		a = GetKeyState(static_cast<int>(lua_tonumber(L,1)));
		lua_pushnumber(L,a);
		return 1;
	}
	int scriptPython::lua_get_special_data(PyObject* self){
		int n = lua_gettop(L);
		lua_Number a;

		if(n!=2 || !lua_isnumber(L,1) || !lua_isnumber(L,2)){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		a = GetSpecialData(static_cast<int>(lua_tonumber(L,1)), static_cast<int>(lua_tonumber(L,2)));
		lua_pushnumber(L,a);
		return 1;
	}
	int scriptPython::lua_get_attackarea(PyObject* self){
		int n = lua_gettop(L);
		BOX *a;

		if(n!=2 || !lua_isnumber(L,1) || !lua_isnumber(L,2)){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		a = get_attackarea(static_cast<int>(lua_tonumber(L,1)), static_cast<int>(lua_tonumber(L,2)));
		if(a==NULL){
			return 0;
		}
		lua_pushnumber(L,(lua_Number)a->left);
		lua_pushnumber(L,(lua_Number)a->top);
		lua_pushnumber(L,(lua_Number)a->right);
		lua_pushnumber(L,(lua_Number)a->bottom);
		return 4;
	}
	int scriptPython::lua_get_hitarea(PyObject* self){
		int n = lua_gettop(L);
		BOX *a;

		if(n!=2 || !lua_isnumber(L,1) || !lua_isnumber(L,2)){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		a = get_hitarea(static_cast<int>(lua_tonumber(L,1)), static_cast<int>(lua_tonumber(L,2)));
		if(a==NULL){
			return 0;
		}
		lua_pushnumber(L,(lua_Number)a->left);
		lua_pushnumber(L,(lua_Number)a->top);
		lua_pushnumber(L,(lua_Number)a->right);
		lua_pushnumber(L,(lua_Number)a->bottom);
		return 4;
	}
	int scriptPython::lua_get_fflags(PyObject* self){
		int n = lua_gettop(L);

		if(n!=2 || !lua_isnumber(L,1) || !lua_isnumber(L,2) || lua_tointeger(L,1)<0 || lua_tointeger(L,1)>1){
			lua_pushstring(L,"引数が正しくありません");
			return lua_error(L);
		}
		bool val;
		if(lua_tointeger(L,1)==0){
			val = my_data.CheckFFlags(lua_tointeger(L,2));
		} else {
			val = enemy_data.CheckFFlags(lua_tointeger(L,2));
		}
		lua_pushboolean(L,val);
		return 1;
	}
	int scriptPython::lua_get_aflags(PyObject* self){
		int n = lua_gettop(L);

		if(n!=2 || !lua_isnumber(L,1) || !lua_isnumber(L,2) || lua_tointeger(L,1)<0 || lua_tointeger(L,1)>1){
			lua_pushstring(L,"引数が正しくありません");
			return lua_error(L);
		}
		bool val;
		if(lua_tointeger(L,1)==0){
			val = my_data.CheckAFlags(lua_tointeger(L,2));
		} else {
			val = enemy_data.CheckAFlags(lua_tointeger(L,2));
		}
		lua_pushboolean(L,val);
		return 1;
	}

	int scriptPython::lua_getfunc(PyObject* self){
		int n = lua_gettop(L);

		if(n!=1 || (!lua_isstring(L,1) && !lua_isfunction(L,1))){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		if(lua_isstring(L,1)){
			lua_getglobal(L,lua_tostring(L,1));
		} else {
			lua_insert(L,1);
		}
		return 1;
	}
	int scriptPython::lua_get_ini_value(PyObject* self){
		int n = lua_gettop(L);
		char *s;

		if(n!=1 || !lua_isstring(L,1)){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		s = ini_value(lua_tostring(L,1));
		if(s==NULL){
			return 0;
		} else {
			lua_pushstring(L,s);
			return 1;
		}
	}

	int scriptPython::lua_get_ini_int(PyObject* self){
		int n = lua_gettop(L);

		if(n!=1 || !lua_isstring(L,1)){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		lua_pushnumber(L,(lua_Number)ini_int(lua_tostring(L,1)));
		return 1;
	}

	int scriptPython::lua_get_ini_int2(PyObject* self){
		int n = lua_gettop(L);

		if(n!=2 || !lua_isstring(L,1) || !lua_isnumber(L,2)){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		lua_pushnumber(L,(lua_Number)ini_int2(lua_tostring(L,1), static_cast<int>(lua_tonumber(L,2))));
		return 1;
	}

	int scriptPython::lua_load_ini(PyObject* self){
		int n = lua_gettop(L);

		if(n!=1 || !lua_isstring(L,1)){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		lua_pushnumber(L,(lua_Number)load_ini(lua_tostring(L,1)));
		return 1;
	}

	int scriptPython::lua_get_version(PyObject* self){
		int n = lua_gettop(L);

		if(n!=0){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		lua_pushstring(L,OPEN_VERSION);
		return 1;
	}
	int scriptPython::lua_get_key_stat2(PyObject* self){
		int n = lua_gettop(L);
		player *p;

		if(n!=2 || !lua_isnumber(L,1) || !lua_isnumber(L,2)){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		if(lua_tonumber(L,1) == 0){
			p = &my_data;
		} else {
			p = &enemy_data;
		}
		lua_pushnumber(L, p->GetKeyState(lua_tonumber(L,2)));
		return 1;
	}
	int scriptPython::lua_get_key_map2(PyObject* self){
		int n = lua_gettop(L);
		int i;
		player *p;

		if(n!=1 || !lua_isnumber(L,1)){
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		if(lua_tonumber(L,1) == 0){
			p = &my_data;
		} else {
			p = &enemy_data;
		}
		i = 0;
		while(i < 10){
			lua_pushnumber(L, p->GetKeyState(i));
			i++;
		}
		return 10;
	}
*/
