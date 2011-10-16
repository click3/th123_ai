
#include "stdafx.h"

#pragma comment (lib,"winmm.lib")

struct KeyAction{
	int code;
	int flag;
	int frame;
};

int key_frame = 0;
std::list<KeyAction> action_list;

void keybd_event3(int code, DWORD dwFlags) {
	if((dwFlags&KEYEVENTF_KEYUP) == 0)code += 768;
	keybd_event(0, static_cast<BYTE>(code), dwFlags, 0);
}

void keybd_event2(int code, DWORD dwFlags) {
	KeyAction action;
	action.frame = key_frame+key_delay;
	action.code = code;
	action.flag = dwFlags;
	action_list.push_back(action);
}

void kev_call(void) {
	for(std::list<KeyAction>::iterator it = action_list.begin(); it != action_list.end(); ) {
		if(it->frame <= key_frame) {
			keybd_event3(it->code, it->flag);
			it = action_list.erase(it);
			continue;
		}
		++it;
	}
}

void kev_clear(void) {
	BOOST_FOREACH(const KeyAction &act, action_list) {
		keybd_event3(act.code, act.flag);
	}
	action_list.clear();
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
