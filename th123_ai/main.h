
#pragma once

enum{MY,ENEMY};

void th105_active(void);
void MyWndActive(int flag);
void reload_check(void);

void yield(void);
void key_reset(void);
void key_off(int n);
void key_on(int n);
void set_key_delay(int delay);
void set_data_delay(int delay);
void set_weather_delay(int n);


char GetSkillLv(int player,int n);
int GetCardCost(int player,int n);
int GetCardCost2(int player,int n);
Box *get_hitarea(int player,int n);
Box *get_attackarea(int player,int n);
short *GetDeckList(int player);
int GetSpecialData(int player,int n);
int get_correction(int player,int flag);

void ai(void);
void SWR_ScanProjectile(HANDLE proc);
HANDLE GetProcessHandle(void);
void get_th105param(void);
void search_key(void);
void is_bullethit(void);
int GetPlayerAddr(int player);
long WINAPI exep(EXCEPTION_POINTERS *pep);
void changeLoadLibraryExW(int flag);
void load_inis(void);

char get_key_stat(int n);

extern int seen,mode,weather,weather2;

extern HANDLE ph;
extern HINSTANCE hInst;
extern HWND wh;
extern HWND my_wh;
extern char aimanager_signature[14];



extern player my_data;
extern player enemy_data;


