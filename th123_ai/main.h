
#pragma once


enum {ACT_UP,ACT_DOWN,ACT_LEFT,ACT_RIGHT,ACT_A,ACT_B,ACT_C,ACT_D,ACT_AB,ACT_BC,ACT_DLEFT,ACT_DRIGHT,ACT_ULEFT,ACT_URIGHT};
enum {MY, ENEMY};

void th105_active(void);
void MyWndActive(int flag);
void reload_check(void);

void yield(void);
void set_data_delay(int delay);
void set_weather_delay(int n);


char GetSkillLv(int player,int n);
short GetCardId(int player,int n);
int GetCardCost(int player,int n);
int GetCardCost2(int player,int n);
Box *get_hitarea(int player,int n);
Box *get_attackarea(int player,int n);
short *GetDeckList(int player);
int GetSpecialData(int player,int n);
int get_correction(int player,int flag);

void ai(void);
void SWR_ScanProjectile(HANDLE proc);
org::click3::Utility::SHARED_HANDLE GetProcessHandle(void);
void get_th105param(void);
void is_bullethit(void);
int GetPlayerAddr(int player);
long WINAPI exep(EXCEPTION_POINTERS *pep);
void changeLoadLibraryExW(int flag);
void load_inis(void);
void change_player(void);

char get_key_stat(int n);

extern int seen,mode,weather,weather2;

extern org::click3::Utility::SHARED_HANDLE ph;
extern HINSTANCE hInst;
extern HWND wh;
extern HWND my_wh;
extern char aimanager_signature[14];



extern Character my_data;
extern Character enemy_data;


