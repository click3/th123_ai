
#pragma once


void scriptExec(const char *fn);
void SetActiveWindow2(HWND hWnd);

void reload(void);
void reload2(void);

int OpenAI(const char *s);


extern scriptEngine *engine;
