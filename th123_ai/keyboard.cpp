
#include "stdafx.h"

#pragma comment (lib,"winmm.lib")

struct KeyAction{
	int code;
	int flag;
	int frame;
	KeyAction *next;
};

int key_frame = 0;
KeyAction *key_list = NULL;

void keybd_event3(int code, DWORD dwFlags) {
	if((dwFlags&KEYEVENTF_KEYUP) == 0)code += 768;
	keybd_event(0, static_cast<BYTE>(code), dwFlags, 0);
}

void keybd_event2(int code, DWORD dwFlags) {
	static KeyAction *a;

	if(key_list == NULL){
		key_list = static_cast<KeyAction*>(malloc(sizeof(KeyAction)));
		a = key_list;
	} else {
		a->next = static_cast<KeyAction*>(malloc(sizeof(KeyAction)));
		a = a->next;
	}
	a->next = NULL;
	a->frame = key_frame+key_delay;
	a->code = code;
	a->flag = dwFlags;
}

void kev_call(void) {
	KeyAction *a,*b,*c;

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

void kev_clear(void) {
	KeyAction *a,*b;

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

void joy_proc(int flag) {
	if(list[flag]) {
		__proc(flag,KEY_UP);
	} else {
		__proc(flag,KEY_DOWN);
	}
	list[flag] = !list[flag];
}

void regist_joyproc(void (*func)(int,int)) {
	__proc = func;
}

void JoyLoop(void) {
	static JOYINFOEX *JoyInfoEx = NULL;

	if(JoyInfoEx == NULL) {
		JoyInfoEx = (JOYINFOEX *)malloc(sizeof(JOYINFOEX));
		JoyInfoEx->dwSize = sizeof(JOYINFOEX);
		JoyInfoEx->dwFlags = JOY_RETURNALL;
	}

	if(joyGetPosEx(0, JoyInfoEx) == JOYERR_NOERROR) {
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
