
#include "stdafx.h"

#pragma comment (lib,"winmm.lib")

struct code_array{
	BYTE key_code;
	BYTE scan_code;
};

struct code_array code_map[] = {
	//{仮想キーコード,ハードウェアスキャンコード},
	{VK_ESCAPE,	DIK_ESCAPE},
	{VK_1,	DIK_1},
	{VK_2,	DIK_2},
	{VK_3,	DIK_3},
	{VK_4,	DIK_4},
	{VK_5,	DIK_5},
	{VK_6,	DIK_6},
	{VK_7,	DIK_7},
	{VK_8,	DIK_8},
	{VK_9,	DIK_9},
	{VK_0,	DIK_0},
	{VK_OEM_MINUS,	DIK_MINUS},
	{VK_OEM_MINUS,	DIK_EQUALS},
	{VK_BACK,	DIK_BACK},
	{VK_TAB,	DIK_TAB},
	{VK_Q,	DIK_Q},
	{VK_W,	DIK_W},
	{VK_E,	DIK_E},
	{VK_R,	DIK_R},
	{VK_T,	DIK_T},
	{VK_Y,	DIK_Y},
	{VK_U,	DIK_U},
	{VK_I,	DIK_I},
	{VK_O,	DIK_O},
	{VK_P,	DIK_P},
	{VK_OEM_4,	DIK_LBRACKET},
	{VK_OEM_6,	DIK_RBRACKET},
	{VK_RETURN,	DIK_RETURN},
	{VK_CONTROL,	DIK_LCONTROL},
	{VK_A,	DIK_A},
	{VK_S,	DIK_S},
	{VK_D,	DIK_D},
	{VK_F,	DIK_F},
	{VK_G,	DIK_G},
	{VK_H,	DIK_H},
	{VK_J,	DIK_J},
	{VK_K,	DIK_K},
	{VK_L,	DIK_L},
	{VK_OEM_1,	DIK_SEMICOLON},
	{VK_OEM_7,	DIK_APOSTROPHE},
	{VK_OEM_3,	DIK_GRAVE},
	{VK_SHIFT,	DIK_LSHIFT},
	{VK_OEM_5,	DIK_BACKSLASH},
	{VK_Z,	DIK_Z},
	{VK_X,	DIK_X},
	{VK_C,	DIK_C},
	{VK_V,	DIK_V},
	{VK_B,	DIK_B},
	{VK_N,	DIK_N},
	{VK_M,	DIK_M},
	{VK_OEM_COMMA,	DIK_COMMA},
	{VK_OEM_PERIOD,	DIK_PERIOD},
	{VK_OEM_2,	DIK_SLASH},
	{VK_SHIFT,	DIK_RSHIFT},
	{VK_MULTIPLY,	DIK_MULTIPLY},
	{VK_MENU,	DIK_LMENU},
	{VK_SPACE,	DIK_SPACE},
	{VK_CAPITAL,	DIK_CAPITAL},
	{VK_F1,	DIK_F1},
	{VK_F2,	DIK_F2},
	{VK_F3,	DIK_F3},
	{VK_F4,	DIK_F4},
	{VK_F5,	DIK_F5},
	{VK_F6,	DIK_F6},
	{VK_F7,	DIK_F7},
	{VK_F8,	DIK_F8},
	{VK_F9,	DIK_F9},
	{VK_F10,	DIK_F10},
	{VK_NUMLOCK,	DIK_NUMLOCK},
	{VK_SCROLL,	DIK_SCROLL},
	{VK_NUMPAD7,	DIK_NUMPAD7},
	{VK_NUMPAD8,	DIK_NUMPAD8},
	{VK_NUMPAD9,	DIK_NUMPAD9},
	{VK_SUBTRACT,	DIK_SUBTRACT},
	{VK_NUMPAD4,	DIK_NUMPAD4},
	{VK_NUMPAD5,	DIK_NUMPAD5},
	{VK_NUMPAD6,	DIK_NUMPAD6},
	{VK_ADD,	DIK_ADD},
	{VK_NUMPAD1,	DIK_NUMPAD1},
	{VK_NUMPAD2,	DIK_NUMPAD2},
	{VK_NUMPAD3,	DIK_NUMPAD3},
	{VK_NUMPAD0,	DIK_NUMPAD0},
	{VK_DECIMAL,	DIK_DECIMAL},
	{VK_OEM_102,	DIK_OEM_102},
	{VK_F11,	DIK_F11},
	{VK_F12,	DIK_F12},
	{VK_F13,	DIK_F13},
	{VK_F14,	DIK_F14},
	{VK_F15,	DIK_F15},
	{VK_KANA,	DIK_KANA},
	{0,	DIK_ABNT_C1},
	{VK_CONVERT,	DIK_CONVERT},
	{VK_NONCONVERT,	DIK_NOCONVERT},
	{VK_OEM_5,	DIK_YEN},
	{0,	DIK_ABNT_C2},
	{VK_OEM_MINUS,	DIK_NUMPADEQUALS},
	{0,	DIK_PREVTRACK},
	{VK_OEM_3,	DIK_AT},
	{VK_OEM_1,	DIK_COLON},
	{0,	DIK_UNDERLINE},
	{VK_KANJI,	DIK_KANJI},
	{0,	DIK_STOP},
	{0,	DIK_AX},
	{0,	DIK_UNLABELED},
	{0,	DIK_NEXTTRACK},
	{VK_RETURN,	DIK_NUMPADENTER},
	{VK_CONTROL,	DIK_RCONTROL},
	{0,	DIK_MUTE},
	{0,	DIK_CALCULATOR},
	{0,	DIK_PLAYPAUSE},
	{0,	DIK_MEDIASTOP},
	{0,	DIK_VOLUMEDOWN},
	{0,	DIK_VOLUMEUP},
	{0,	DIK_WEBHOME},
	{0,	DIK_NUMPADCOMMA},
	{VK_DIVIDE,	DIK_DIVIDE},
	{0,	DIK_SYSRQ},
	{VK_MENU,	DIK_RMENU},
	{VK_PAUSE,	DIK_PAUSE},
	{VK_HOME,	DIK_HOME},
	{VK_UP,	DIK_UP},
	{VK_PRIOR,	DIK_PRIOR},
	{VK_LEFT,	DIK_LEFT},
	{VK_RIGHT,	DIK_RIGHT},
	{VK_END,	DIK_END},
	{VK_DOWN,	DIK_DOWN},
	{VK_NEXT,	DIK_NEXT},
	{VK_INSERT,	DIK_INSERT},
	{VK_DELETE,	DIK_DELETE},
	{VK_LWIN,	DIK_LWIN},
	{VK_RWIN,	DIK_RWIN},
	{VK_APPS,	DIK_APPS},
	{0,	DIK_POWER},
	{0,	DIK_SLEEP},
	{0,	DIK_WAKE},
	{0,	DIK_WEBSEARCH},
	{0,	DIK_WEBFAVORITES},
	{0,	DIK_WEBREFRESH},
	{0,	DIK_WEBSTOP},
	{0,	DIK_WEBFORWARD},
	{0,	DIK_WEBBACK},
	{0,	DIK_MYCOMPUTER},
	{0,	DIK_MAIL},
	{0,	DIK_MEDIASELECT},
	{0,0}		//終端識別用
};

typedef struct key_action{
	int code;
	int flag;
	int frame;
	struct key_action *next;
}KEY_ACTION;

int key_frame = 0;
KEY_ACTION *key_list = NULL;
/*
void keybd_event3(BYTE code, DWORD dwFlags){
	static int max = sizeof(code_map)/sizeof(struct code_array);
	int bScan;
	int i = 0;
	while(code_map[i].key_code != code){
		i++;
		if(i >= max)return;
	}
	bScan = code_map[i].scan_code;
	//bScan = MapVirtualKey(code,0);
	if((dwFlags&KEYEVENTF_KEYUP) == 0)bScan += 768;
	keybd_event(code,bScan,dwFlags,0);
}*/
void keybd_event3(int code, DWORD dwFlags){
	if((dwFlags&KEYEVENTF_KEYUP) == 0)code += 768;
	keybd_event(0, static_cast<BYTE>(code), dwFlags, 0);
}

void keybd_event2(int code, DWORD dwFlags){
	static KEY_ACTION *a;

	if(key_list == NULL){
		key_list = static_cast<KEY_ACTION*>(malloc(sizeof(KEY_ACTION)));
		a = key_list;
	} else {
		a->next = static_cast<KEY_ACTION*>(malloc(sizeof(KEY_ACTION)));
		a = a->next;
	}
	a->next = NULL;
	a->frame = key_frame+key_delay;
	a->code = code;
	a->flag = dwFlags;
}

void kev_call(void){
	KEY_ACTION *a,*b,*c;

	a = key_list;
	if(a == NULL){
		return;
	}
	key_list = NULL;
	c = NULL;
	while(a != NULL){
		if(a->frame<=key_frame){
			keybd_event3(a->code,a->flag);
			b = a;
			a = a->next;
			free(b);
		} else if(c==NULL){
			key_list = a;
			c = a;
			a = a->next;
			c->next = NULL;
		} else {
			c->next = a;
			c = a;
			a = a->next;
			c->next = NULL;
		}
	}
}

void kev_clear(void){
	KEY_ACTION *a,*b;

	a = key_list;
	if(a == NULL){
		return;
	}
	while(a != NULL){
		keybd_event3(a->code,KEYEVENTF_KEYUP);
		b = a;
		a = a->next;
		free(b);
	}
	key_list = a;
}

BOOL list[MAX] = {FALSE};
void (*__proc)(int,int) = NULL;

void joy_proc(int flag){
	if(list[flag])__proc(flag,KEY_UP);
	else __proc(flag,KEY_DOWN);
	list[flag] = !list[flag];
	return;
}

void regist_joyproc(void (*func)(int,int)){
	__proc = func;
}

void JoyLoop(void){
	static JOYINFOEX *JoyInfoEx = NULL;

	if(JoyInfoEx == NULL){
		JoyInfoEx = (JOYINFOEX *)malloc(sizeof(JOYINFOEX));
		JoyInfoEx->dwSize = sizeof(JOYINFOEX);
		JoyInfoEx->dwFlags = JOY_RETURNALL;
	}

	if(joyGetPosEx(0, JoyInfoEx) == JOYERR_NOERROR){
#define JOY_PROC(x,y)	if(x){if(!list[y]){joy_proc(y);}}else{if(list[y]){joy_proc(y);}}
		JOY_PROC(JoyInfoEx->dwXpos > 0x7FFF + 0x4000,RIGHT);
		JOY_PROC(JoyInfoEx->dwXpos < 0x7FFF - 0x4000,LEFT);
		JOY_PROC(JoyInfoEx->dwYpos > 0x7FFF + 0x4000,DOWN);
		JOY_PROC(JoyInfoEx->dwYpos < 0x7FFF - 0x4000,UP);
		JOY_PROC(JoyInfoEx->dwButtons & JOY_BUTTON1,BOTTON1);
		JOY_PROC(JoyInfoEx->dwButtons & JOY_BUTTON2,BOTTON2);
		JOY_PROC(JoyInfoEx->dwButtons & JOY_BUTTON3,BOTTON3);
		JOY_PROC(JoyInfoEx->dwButtons & JOY_BUTTON4,BOTTON4);
		JOY_PROC(JoyInfoEx->dwButtons & JOY_BUTTON5,BOTTON5);
		JOY_PROC(JoyInfoEx->dwButtons & JOY_BUTTON6,BOTTON6);
		JOY_PROC(JoyInfoEx->dwButtons & JOY_BUTTON7,BOTTON7);
		JOY_PROC(JoyInfoEx->dwButtons & JOY_BUTTON8,BOTTON8);
		JOY_PROC(JoyInfoEx->dwButtons & JOY_BUTTON9,BOTTON9);
	}
}
