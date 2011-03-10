
#pragma once

#include "scriptBase.h"
#include "lib.h"
#include "version.h"
#include "network.h"
#include "consts.h"
#include "resource.h"
#include "Python.h"


class scriptPython : public scriptEngine {
private:
	static scriptPython *instance;//èâä˙ílNULL
	scriptPython() : scriptEngine() {
		Py_Initialize();
		instance = this;
	}
	~scriptPython() {
		instance = NULL;
		Py_Finalize();
	}
public:
	static scriptPython *getCurrentInstance();
	virtual int getScriptValue(const char *name)const;
	virtual void setScriptValue(const char *name, double value);
	virtual void setScriptValueBool(const char *name, bool value);
	virtual void call(const char *name);

protected:
	static int panic_reload(PyObject* self);
	virtual bool initScript();
	virtual void startScript();
	virtual void endScript();
	virtual bool execBuffer(const char *buffer, int size, const char *name);
	static int lua_yield(PyObject* self);
	static int lua_key_reset(PyObject* self);
	static int lua_key_off(PyObject* self);
	static int lua_key_on(PyObject* self);
	static int lua_require(PyObject* self);
	static int lua_get_skill_lv(PyObject* self);
	static int lua_get_card_id(PyObject* self);
	static int lua_get_card_cost(PyObject* self);
	static int lua_get_card_cost2(PyObject* self);
	static int lua_set_key_delay(PyObject* self);
	static int lua_set_data_delay(PyObject* self);
	static int lua_get_obj_data(PyObject* self);
	static int lua_get_obj_attackarea(PyObject* self);
	static int lua_get_obj_hitarea(PyObject* self);
	static int lua_get_correction(PyObject* self);
	static int lua_set_weather_delay(PyObject* self);
	static int lua_get_opt_xy(PyObject* self);
	static int lua_get_deck_list(PyObject* self);
	static int lua_get_key_stat(PyObject* self);
	static int lua_get_key_map(PyObject* self);
	static int lua_get_real_key_stat(PyObject* self);
	static int lua_get_special_data(PyObject* self);
	static int lua_get_attackarea(PyObject* self);
	static int lua_get_hitarea(PyObject* self);
	static int lua_get_fflags(PyObject* self);
	static int lua_get_aflags(PyObject* self);
	static int lua_getfunc(PyObject* self);
	static int lua_get_ini_value(PyObject* self);
	static int lua_get_ini_int(PyObject* self);
	static int lua_get_ini_int2(PyObject* self);
	static int lua_load_ini(PyObject* self);
	static int lua_get_version(PyObject* self);
	static int lua_get_key_stat2(PyObject* self);
	static int lua_get_key_map2(PyObject* self);
};

