
#include "stdafx.h"


scriptLua *scriptLua::instance = NULL;

//public
	scriptLua *scriptLua::getCurrentInstance() {
		if(instance == NULL) {
			instance = new scriptLua();
		}
		return instance;
	}

	int scriptLua::getScriptValue(const char *name)const{
		lua_Number ret;
		lua_getglobal(state, name);
		ret = lua_tonumber(state, -1);
		lua_pop(state, 1);
		return (int)ret;
	}
	void scriptLua::setScriptValue(const char *name, double value) {
		lua_pushnumber(state, value);
		lua_setglobal(state, name);
	}
	void scriptLua::setScriptValueBool(const char *name, bool value) {
		lua_pushboolean(state, value);
		lua_setglobal(state, name);
	}
	void scriptLua::call(const char *name) {
		lua_getglobal(state, name);
		lua_call(state, 0, 0);
	}

//protected
	int scriptLua::panic_reload(lua_State *L) {
		int n = lua_gettop(L);
		int i;

		MyWndActive(TRUE);
		i = 0;
	        FILE *fp = fopen("error_tmp.tmp147258369","a");
		while(i < n) {
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
		if(strlen(aimanager_signature)==13) {
			PostScriptError(URL_SCRIPT_ERROR_CGI,aimanager_signature,str);
		}
		while(GetKeyState(VK_RETURN)>=0 || my_wh!=GetForegroundWindow()) {
			reload_check();
			Sleep(50);
		}
		while(GetKeyState(VK_RETURN)<0) {Sleep(50);}
		scriptLua::getCurrentInstance()->reload();
		return 0;
	}

	bool scriptLua::initScript() {
		// Lua の言語エンジンを初期化
		state = luaL_newstate();
		// Luaのライブラリを使えるようにする
		luaL_openlibs(state);

		lua_atpanic(state, &scriptLua::panic_reload);

		set_weather_delay(300);
		set_key_delay(0);
		set_data_delay(0);

		lua_register(state, "_yield",			&scriptLua::lua_yield);
		lua_register(state, "key_reset",		&scriptLua::lua_key_reset);
		lua_register(state, "key_off",			&scriptLua::lua_key_off);
		lua_register(state, "key_on",			&scriptLua::lua_key_on);
		lua_register(state, "require",			&scriptLua::lua_require);
		lua_register(state, "get_skill_lv",		&scriptLua::lua_get_skill_lv);
		lua_register(state, "get_card_id",		&scriptLua::lua_get_card_id);
		lua_register(state, "get_card_cost",		&scriptLua::lua_get_card_cost);
		lua_register(state, "get_card_cost2",		&scriptLua::lua_get_card_cost2);
		lua_register(state, "set_key_delay",		&scriptLua::lua_set_key_delay);
		lua_register(state, "set_data_delay",		&scriptLua::lua_set_data_delay);
		lua_register(state, "get_obj_data",		&scriptLua::lua_get_obj_data);
		lua_register(state, "set_weather_delay",	&scriptLua::lua_set_weather_delay);
		lua_register(state, "get_opt_xy",		&scriptLua::lua_get_opt_xy);
		lua_register(state, "get_deck_list",		&scriptLua::lua_get_deck_list);
		lua_register(state, "get_key_stat",		&scriptLua::lua_get_key_stat);
		lua_register(state, "get_key_map",		&scriptLua::lua_get_key_map);
		lua_register(state, "get_real_key_stat",	&scriptLua::lua_get_real_key_stat);
		lua_register(state, "get_special_data",		&scriptLua::lua_get_special_data);
		lua_register(state, "get_obj_attackarea",	&scriptLua::lua_get_obj_attackarea);
		lua_register(state, "get_attackarea",		&scriptLua::lua_get_attackarea);
		lua_register(state, "get_hitarea",		&scriptLua::lua_get_hitarea);
		lua_register(state, "get_correction",		&scriptLua::lua_get_correction);
		lua_register(state, "get_fflags",		&scriptLua::lua_get_fflags);
		lua_register(state, "get_aflags",		&scriptLua::lua_get_aflags);
		lua_register(state, "getfunc",			&scriptLua::lua_getfunc);
		lua_register(state, "get_ini_value",		&scriptLua::lua_get_ini_value);
		lua_register(state, "get_ini_int",		&scriptLua::lua_get_ini_int);
		lua_register(state, "get_ini_int2",		&scriptLua::lua_get_ini_int2);
		lua_register(state, "load_ini",			&scriptLua::lua_load_ini);
		lua_register(state, "get_version",		&scriptLua::lua_get_version);
		lua_register(state, "get_key_stat2",		&scriptLua::lua_get_key_stat2);
		lua_register(state, "get_key_map2",		&scriptLua::lua_get_key_map2);
		return loadResource(LUA_SCRIPT_RC);
	}
	void scriptLua::startScript() {
		call("api_main");
	}
	void scriptLua::endScript() {
		ffree();
		lua_close(state);
	}

  bool scriptLua::execBuffer(const char *buffer, int size, const char *name) {
    int a = luaL_loadbuffer(state, buffer, size, name);
    if(a != 0) {
      FILE *fp = fopen("error_tmp.tmp147258369","a");
      fprintf(fp,"Error:require(%s)\n",name);
      if(a==LUA_ERRFILE) {
        fprintf(fp,"fileread failed(ファイル読み取りに失敗しました)\n");
      } else if(a==LUA_ERRSYNTAX) {
        fprintf(fp,"syntax error(構文エラーが発生しました)\n");
      } else if(a==LUA_ERRMEM ) {
        fprintf(fp,"memory malloc failed(メモリー割り当てに失敗しました)\n");
      } else {
        fprintf(fp,"unknown error %d(想定外のエラーに遭遇しました)\n",a);
      }
      fclose(fp);
      return false;
    }
    a = lua_pcall(state,0,0,0);
    if(a != 0) {
      FILE *fp = fopen("error_tmp.tmp147258369","a");
      fprintf(fp,"Error:call(%s)\n",name);
      if(a==LUA_ERRRUN) {
        fprintf(fp,"run error(実行時エラー)\n");
      } else if(a==LUA_ERRERR) {
        fprintf(fp,"errorhandler error(エラーハンドラ実行中にエラーしました)\n");
      } else if(a==LUA_ERRMEM) {
        fprintf(fp,"memory malloc failed(メモリー割り当てに失敗しました)\n");
      } else {
        fprintf(fp,"unknown error %d(想定外のエラーに遭遇しました)\n",a);
      }
      fclose(fp);
      return false;
    }
    return true;
  }



	int scriptLua::lua_yield(lua_State *L) {
		int n = lua_gettop(L);

		if(n!=0) {
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		yield();
		return 0;
	}
	int scriptLua::lua_key_reset(lua_State *L) {
		int n = lua_gettop(L);

		if(n!=0) {
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		key_reset();
		return 0;
	}
	int scriptLua::lua_key_off(lua_State *L) {
		int n = lua_gettop(L);

		if(n!=1 || !lua_isnumber(L,1)) {
			lua_pushstring(L,"引数が正しくありません\n");
			lua_error(L);
		}
		key_off(static_cast<int>(lua_tonumber(L,1)));
		return 0;
	}
	int scriptLua::lua_key_on(lua_State *L) {
		int n = lua_gettop(L);

		if(n!=1 || !lua_isnumber(L,1)) {
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		key_on(static_cast<int>(lua_tonumber(L,1)));
		return 0;
	}
	int scriptLua::lua_require(lua_State *L) {
		int n = lua_gettop(L);

		if(n!=1 || !lua_isstring(L,1)) {
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		scriptLua::getCurrentInstance()->loadFile(lua_tostring(L,1));
		return 0;
	}
	int scriptLua::lua_get_skill_lv(lua_State *L) {
		int n = lua_gettop(L);
		lua_Number a;

		if(n!=2 || !lua_isnumber(L,1) || !lua_isnumber(L,2)) {
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		a = GetSkillLv(static_cast<int>(lua_tonumber(L,1)), static_cast<int>(lua_tonumber(L,2)));
		lua_pushnumber(L,a);
		return 1;
	}
	int scriptLua::lua_get_card_id(lua_State *L) {
		int n = lua_gettop(L);
		lua_Number a;

		if(n!=2 || !lua_isnumber(L,1) || !lua_isnumber(L,2)) {
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		a = GetCardId(static_cast<int>(lua_tonumber(L,1)), static_cast<int>(lua_tonumber(L,2)));
		lua_pushnumber(L,a);
		return 1;
	}
	int scriptLua::lua_get_card_cost(lua_State *L) {
		int n = lua_gettop(L);
		lua_Number a;

		if(n!=2 || !lua_isnumber(L,1) || !lua_isnumber(L,2)) {
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		a = GetCardCost(static_cast<int>(lua_tonumber(L,1)), static_cast<int>(lua_tonumber(L,2)));
		lua_pushnumber(L,a);
		return 1;
	}
	int scriptLua::lua_get_card_cost2(lua_State *L) {
		int n = lua_gettop(L);
		lua_Number a;

		if(n!=2 || !lua_isnumber(L,1) || !lua_isnumber(L,2)) {
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		a = GetCardCost2(static_cast<int>(lua_tonumber(L,1)), static_cast<int>(lua_tonumber(L,2)));
		lua_pushnumber(L,a);
		return 1;
	}
	int scriptLua::lua_set_key_delay(lua_State *L) {
		int n = lua_gettop(L);

		if(n!=1 || !lua_isnumber(L,1)) {
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		set_key_delay(static_cast<int>(lua_tonumber(L,1)));
		return 0;
	}
	int scriptLua::lua_set_data_delay(lua_State *L) {
		int n = lua_gettop(L);

		if(n!=1 || !lua_isnumber(L,1)) {
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		set_data_delay(static_cast<int>(lua_tonumber(L,1)));
		return 0;
	}
	int scriptLua::lua_get_obj_data(lua_State *L) {
		const unsigned int n = ::lua_gettop(L);

		if(n!=2 || !::lua_isnumber(L, 1) || !::lua_isnumber(L, 2) || ::lua_tointeger(L, 1)<0 || ::lua_tointeger(L, 1)>1) {
			::lua_pushstring(L,"引数が正しくありません");
			::lua_error(L);
		}
		const unsigned int player_type = ::lua_tointeger(L, 1);
		const unsigned int index = ::lua_tointeger(L, 2);
		const Character &player = (player_type == 0 ? my_data : enemy_data);
		if(index >= player.object.size()) {
			return 0;
		}
		const Obj &obj = player.GetObject(index);
		::lua_pushnumber(L, static_cast<lua_Number>(obj.action));
		::lua_pushnumber(L, static_cast<lua_Number>(obj.x));
		::lua_pushnumber(L, static_cast<lua_Number>(obj.y));
		::lua_pushnumber(L, static_cast<lua_Number>(obj.hp));
		::lua_pushnumber(L, static_cast<lua_Number>(obj.img_no));
		::lua_pushnumber(L, static_cast<lua_Number>(obj.frame));
		::lua_pushnumber(L, static_cast<lua_Number>(obj.speed.x));
		::lua_pushnumber(L, static_cast<lua_Number>(obj.speed.y));
		::lua_pushnumber(L, static_cast<lua_Number>(obj.base_addr));
		::lua_pushnumber(L, static_cast<lua_Number>(obj.attackarea.size()));
		::lua_pushnumber(L, static_cast<lua_Number>(obj.hitarea.size()));
		return 11;
	}
	int scriptLua::lua_get_obj_attackarea(lua_State *L) {
		const unsigned int n = lua_gettop(L);
		if(n != 3 || !::lua_isnumber(L, 1) || !::lua_isnumber(L, 2) || !::lua_isnumber(L, 3) || ::lua_tointeger(L, 1) < 0 || ::lua_tointeger(L, 1) > 1) {
			::lua_pushstring(L,"引数が正しくありません");
			::lua_error(L);
		}
		const unsigned int player_type = ::lua_tointeger(L, 1);
		const unsigned int index = ::lua_tointeger(L, 2);
		const Character &player = (player_type == 0 ? my_data : enemy_data);
		if(index >= player.object.size()) {
			return 0;
		}
		const Obj &obj = player.GetObject(index);
		const unsigned int attackarea_index = ::lua_tointeger(L, 3);
		if(obj.attackarea.size() > attackarea_index) {
			const Box &b = obj.attackarea[attackarea_index];
			::lua_pushnumber(L, static_cast<lua_Number>(b.left));
			::lua_pushnumber(L, static_cast<lua_Number>(b.top));
			::lua_pushnumber(L, static_cast<lua_Number>(b.right));
			::lua_pushnumber(L, static_cast<lua_Number>(b.bottom));
			return 4;
		}
		return 0;
	}
	int scriptLua::lua_get_obj_hitarea(lua_State *L) {
		const unsigned int n = ::lua_gettop(L);

		if(n != 3 || !::lua_isnumber(L, 1) || !::lua_isnumber(L, 2) || !::lua_isnumber(L, 3) || ::lua_tointeger(L, 1) < 0 || ::lua_tointeger(L, 1) > 1) {
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		const unsigned int player_type = ::lua_tointeger(L, 1);
		const unsigned int index = ::lua_tointeger(L, 2);
		const Character &player = (player_type == 0 ? my_data : enemy_data);
		if(index >= player.object.size()) {
			return 0;
		}
		const Obj &obj = player.GetObject(index);
		const unsigned int hitarea_index = ::lua_tointeger(L, 3);
		if(obj.hitarea.size() > hitarea_index) {
			const Box &b = obj.hitarea[hitarea_index];
			::lua_pushnumber(L, static_cast<lua_Number>(b.left));
			::lua_pushnumber(L, static_cast<lua_Number>(b.top));
			::lua_pushnumber(L, static_cast<lua_Number>(b.right));
			::lua_pushnumber(L, static_cast<lua_Number>(b.bottom));
			return 4;
		}
		return 0;
	}
	int scriptLua::lua_get_correction(lua_State *L) {
		int n = lua_gettop(L);

		if(n!=2 || !lua_isnumber(L,1) || !lua_isnumber(L,2)) {
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		lua_pushboolean(L,get_correction(static_cast<int>(lua_tonumber(L,1)), static_cast<int>(lua_tonumber(L,2))));
		return 1;
	}
	int scriptLua::lua_set_weather_delay(lua_State *L) {
		int n = lua_gettop(L);

		if(n!=1 || !lua_isnumber(L,1)) {
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		set_weather_delay(static_cast<int>(lua_tonumber(L,1)));
		return 0;
	}
	int scriptLua::lua_get_opt_xy(lua_State *L) {
		const unsigned int n = ::lua_gettop(L);

		if(n != 2 || !::lua_isnumber(L, 1) || !::lua_isnumber(L, 2) || ::lua_tointeger(L, 1)<0 || ::lua_tointeger(L, 1)>1) {
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		const unsigned int player_type = ::lua_tointeger(L, 1);
		const unsigned int index = ::lua_tointeger(L, 2);
		const Character &player = (player_type == 0 ? my_data : enemy_data);
		if(index >= player.object.size()) {
			return 0;
		}
		const Obj * const obj = player.GetOptionObject(index);
		if(obj == NULL) {
			return 0;
		}
		::lua_pushnumber(L, static_cast<lua_Number>(obj->x));
		::lua_pushnumber(L, static_cast<lua_Number>(obj->y));
		return 2;
	}
	int scriptLua::lua_get_deck_list(lua_State *L) {
		int n = lua_gettop(L);
		short *a;
		int i;
		if(n!=1 || !lua_isnumber(L,1)) {
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		a = GetDeckList(0);
		if(a==NULL)return 0;
		i = 0;
		while(*a != -1) {
			lua_pushnumber(L,*a);
			i++;
			a++;
		}
		return i;
	}
	int scriptLua::lua_get_key_stat(lua_State *L) {
		int n = lua_gettop(L);
		lua_Number a;

		if(n!=1 || !lua_isnumber(L,1)) {
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		a = get_key_stat((int)lua_tonumber(L,1));
		lua_pushnumber(L,a);
		return 1;
	}
	int scriptLua::lua_get_key_map(lua_State *L) {
		int n = lua_gettop(L);
		int i;
		lua_Number a;

		if(n!=0) {
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		i = 0;
		while(i < 10) {
			a = get_key_stat(i);
			lua_pushnumber(L,a);
			i++;
		}
		return 10;
	}
	int scriptLua::lua_get_real_key_stat(lua_State *L) {
		int n = lua_gettop(L);
		lua_Number a;

		if(n!=1 || !lua_isnumber(L,1)) {
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		a = GetKeyState(static_cast<unsigned int>(lua_tonumber(L,1)));
		lua_pushnumber(L,a);
		return 1;
	}
	int scriptLua::lua_get_special_data(lua_State *L) {
		int n = lua_gettop(L);
		lua_Number a;

		if(n!=2 || !lua_isnumber(L,1) || !lua_isnumber(L,2)) {
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		a = GetSpecialData(static_cast<int>(lua_tonumber(L,1)), static_cast<int>(lua_tonumber(L,2)));
		lua_pushnumber(L,a);
		return 1;
	}
	int scriptLua::lua_get_attackarea(lua_State *L) {
		int n = lua_gettop(L);
		Box *a;

		if(n!=2 || !lua_isnumber(L,1) || !lua_isnumber(L,2)) {
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		a = get_attackarea(static_cast<int>(lua_tonumber(L,1)), static_cast<int>(lua_tonumber(L,2)));
		if(a==NULL) {
			return 0;
		}
		lua_pushnumber(L,(lua_Number)a->left);
		lua_pushnumber(L,(lua_Number)a->top);
		lua_pushnumber(L,(lua_Number)a->right);
		lua_pushnumber(L,(lua_Number)a->bottom);
		return 4;
	}
	int scriptLua::lua_get_hitarea(lua_State *L) {
		int n = lua_gettop(L);
		Box *a;

		if(n!=2 || !lua_isnumber(L,1) || !lua_isnumber(L,2)) {
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		a = get_hitarea(static_cast<int>(lua_tonumber(L,1)), static_cast<int>(lua_tonumber(L,2)));
		if(a==NULL) {
			return 0;
		}
		lua_pushnumber(L,(lua_Number)a->left);
		lua_pushnumber(L,(lua_Number)a->top);
		lua_pushnumber(L,(lua_Number)a->right);
		lua_pushnumber(L,(lua_Number)a->bottom);
		return 4;
	}
	int scriptLua::lua_get_fflags(lua_State *L) {
		int n = lua_gettop(L);

		if(n!=2 || !lua_isnumber(L,1) || !lua_isnumber(L,2) || lua_tointeger(L,1)<0 || lua_tointeger(L,1)>1) {
			lua_pushstring(L,"引数が正しくありません");
			return lua_error(L);
		}
		bool val;
		if(lua_tointeger(L,1)==0) {
			val = my_data.CheckFFlags(lua_tointeger(L,2));
		} else {
			val = enemy_data.CheckFFlags(lua_tointeger(L,2));
		}
		lua_pushboolean(L,val);
		return 1;
	}
	int scriptLua::lua_get_aflags(lua_State *L) {
		int n = lua_gettop(L);

		if(n!=2 || !lua_isnumber(L,1) || !lua_isnumber(L,2) || lua_tointeger(L,1)<0 || lua_tointeger(L,1)>1) {
			lua_pushstring(L,"引数が正しくありません");
			return lua_error(L);
		}
		bool val;
		if(lua_tointeger(L,1)==0) {
			val = my_data.CheckAFlags(lua_tointeger(L,2));
		} else {
			val = enemy_data.CheckAFlags(lua_tointeger(L,2));
		}
		lua_pushboolean(L,val);
		return 1;
	}

	int scriptLua::lua_getfunc(lua_State *L) {
		int n = lua_gettop(L);

		if(n!=1 || (!lua_isstring(L,1) && !lua_isfunction(L,1))) {
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		if(lua_isstring(L,1)) {
			lua_getglobal(L,lua_tostring(L,1));
		} else {
			lua_insert(L,1);
		}
		return 1;
	}
	int scriptLua::lua_get_ini_value(lua_State *L) {
		int n = lua_gettop(L);

		if(n!=1 || !lua_isstring(L,1)) {
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		std::wstring key;
		const bool result = org::click3::Utility::SJISToWChar(key, lua_tostring(L,1));
		BOOST_ASSERT(result);
		const std::wstring *ptr = ini_value(key);
		if(ptr == NULL) {
			return 0;
		}
		std::string value;
		const bool result2 = org::click3::Utility::WCharToSJIS(value, *ptr);
		BOOST_ASSERT(result2);
		lua_pushstring(L, value.c_str());
		return 1;
	}

	int scriptLua::lua_get_ini_int(lua_State *L) {
		int n = lua_gettop(L);

		if(n!=1 || !lua_isstring(L,1)) {
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		std::wstring key;
		const bool result = org::click3::Utility::SJISToWChar(key, lua_tostring(L,1));
		BOOST_ASSERT(result);
		lua_pushnumber(L,(lua_Number)ini_int(key));
		return 1;
	}

	int scriptLua::lua_get_ini_int2(lua_State *L) {
		int n = lua_gettop(L);

		if(n!=2 || !lua_isstring(L,1) || !lua_isnumber(L,2)) {
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		std::wstring key;
		const bool result = org::click3::Utility::SJISToWChar(key, lua_tostring(L,1));
		BOOST_ASSERT(result);
		lua_pushnumber(L,(lua_Number)ini_int2(key, static_cast<int>(lua_tonumber(L,2))));
		return 1;
	}

	int scriptLua::lua_load_ini(lua_State *L) {
		int n = lua_gettop(L);

		if(n!=1 || !lua_isstring(L,1)) {
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		lua_pushnumber(L,(lua_Number)load_ini(lua_tostring(L,1)));
		return 1;
	}

	int scriptLua::lua_get_version(lua_State *L) {
		int n = lua_gettop(L);

		if(n!=0) {
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		lua_pushstring(L, GetTH123AIVersionString());
		return 1;
	}
	int scriptLua::lua_get_key_stat2(lua_State *L) {
		int n = lua_gettop(L);

		if(n!=2 || !lua_isnumber(L,1) || !lua_isnumber(L,2)) {
			lua_pushstring(L,"引数が正しくありません");
			lua_error(L);
		}
		const Character &player = (::lua_tonumber(L,1) == 0 ? my_data : enemy_data);
		::lua_pushnumber(L, player.GetKeyState(static_cast<unsigned int>(::lua_tonumber(L, 2))));
		return 1;
	}
	int scriptLua::lua_get_key_map2(lua_State *L) {
		int n = ::lua_gettop(L);

		if(n!=1 || !::lua_isnumber(L, 1)) {
			::lua_pushstring(L,"引数が正しくありません");
			::lua_error(L);
		}
		const Character &player = (::lua_tonumber(L,1) == 0 ? my_data : enemy_data);
		for(unsigned int i = 0; i < 10; i++) {
			::lua_pushnumber(L, player.GetKeyState(i));
		}
		return 10;
	}
