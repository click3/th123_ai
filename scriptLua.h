
#pragma once

#include "scriptBase.h"
#include "lib.h"
#include "version.h"
#include "network.h"
#include "consts.h"
#include "resource.h"
#include "lua_src/lua.h"
#include "lua_src/lualib.h"
#include "lua_src/lauxlib.h"
#pragma comment (lib,"Comdlg32.lib")


class scriptLua : public scriptEngine {
private:
	static scriptLua *instance;//èâä˙ílNULL
	lua_State *state;
	scriptLua() : scriptEngine() {
		state = NULL;
		instance = this;
	}
	~scriptLua() {
		instance = NULL;
	}
public:
	static scriptLua *getCurrentInstance();
	virtual int getScriptValue(const char *name)const;
	virtual void setScriptValue(const char *name, double value);
	virtual void setScriptValueBool(const char *name, bool value);
	virtual void call(const char *name);

protected:
	static int panic_reload(lua_State *L);
	virtual bool initScript();
	virtual void startScript();
	virtual void endScript();
	virtual bool execBuffer(const char *buffer, int size, const char *name);
	static int lua_yield(lua_State *L);
	static int lua_key_reset(lua_State *L);
	static int lua_key_off(lua_State *L);
	static int lua_key_on(lua_State *L);
	static int lua_require(lua_State *L);
	static int lua_get_skill_lv(lua_State *L);
	static int lua_get_card_id(lua_State *L);
	static int lua_get_card_cost(lua_State *L);
	static int lua_get_card_cost2(lua_State *L);
	static int lua_set_key_delay(lua_State *L);
	static int lua_set_data_delay(lua_State *L);
	static int lua_get_obj_data(lua_State *L);
	static int lua_get_obj_attackarea(lua_State *L);
	static int lua_get_obj_hitarea(lua_State *L);
	static int lua_get_correction(lua_State *L);
	static int lua_set_weather_delay(lua_State *L);
	static int lua_get_opt_xy(lua_State *L);
	static int lua_get_deck_list(lua_State *L);
	static int lua_get_key_stat(lua_State *L);
	static int lua_get_key_map(lua_State *L);
	static int lua_get_real_key_stat(lua_State *L);
	static int lua_get_special_data(lua_State *L);
	static int lua_get_attackarea(lua_State *L);
	static int lua_get_hitarea(lua_State *L);
	static int lua_get_fflags(lua_State *L);
	static int lua_get_aflags(lua_State *L);
	static int lua_getfunc(lua_State *L);
	static int lua_get_ini_value(lua_State *L);
	static int lua_get_ini_int(lua_State *L);
	static int lua_get_ini_int2(lua_State *L);
	static int lua_load_ini(lua_State *L);
	static int lua_get_version(lua_State *L);
	static int lua_get_key_stat2(lua_State *L);
	static int lua_get_key_map2(lua_State *L);
};

