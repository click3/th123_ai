
#pragma once

class scriptEngine {
private:
	void convertFilePath(char *path) const;
public:
	char scriptName[256];
	bool loadable;

	scriptEngine() {
		scriptName[0] = '\0';
		loadable = false;
	}
	~scriptEngine() {
		if(loadable) {
			end();
		}
		scriptName[0] = '\0';
		loadable = false;
	}

	bool loadFile(const char *fileName);
	bool loadFile(FILE *fp, const char *name);
	bool loadFile(FILE *fp);
	bool loadDialog();
	bool loadResource(int id);
	bool loadBuffer(const char *buffer, unsigned int size, const char *name);
	bool loadBuffer(const char *buffer, unsigned int size);
	bool loadBuffer(const char *buffer, const char *name);
	bool loadBuffer(const char *buffer);
	bool reload();
	void init();
	void start();
	void end();

	virtual int getScriptValue(const char *name)const = 0;
	virtual void setScriptValue(const char *name, double value) = 0;
	virtual void setScriptValueBool(const char *name, bool value) {
		setScriptValue(name, value ? 1 : 0);
	}
	virtual void call(const char *name) = 0;

protected:
	virtual bool initScript() = 0;
	virtual void startScript() = 0;
	virtual void endScript() = 0;
	virtual bool execBuffer(const char *buffer, unsigned int size, const char *name) = 0;

};



