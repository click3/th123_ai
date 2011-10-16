
#include "stdafx.h"

#pragma comment (lib,"winmm.lib")

void KeybdEvent::ExecEvent(int code, DWORD dwFlags) {
	if((dwFlags&KEYEVENTF_KEYUP) == 0)code += 768;
	keybd_event(0, static_cast<BYTE>(code), dwFlags, 0);
}

void KeybdEvent::AddEvent(int code, DWORD dwFlags) {
	KeyAction action;
	action.frame = key_frame+key_delay;
	action.code = code;
	action.flag = dwFlags;
	action_list.push_back(action);
}

void KeybdEvent::ProcessEvent(void) {
	for(std::list<KeyAction>::iterator it = action_list.begin(); it != action_list.end(); ) {
		if(it->frame <= key_frame) {
			ExecEvent(it->code, it->flag);
			it = action_list.erase(it);
			continue;
		}
		++it;
	}
	key_frame++;
}

void KeybdEvent::Clear(void) {
	BOOST_FOREACH(const KeyAction &act, action_list) {
		ExecEvent(act.code, act.flag);
	}
	action_list.clear();
}
