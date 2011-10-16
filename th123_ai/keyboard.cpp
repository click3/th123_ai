
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
