
#include "stdafx.h"

#pragma comment (lib,"winmm.lib")

void KeybdEvent::ExecEvent(BYTE code, DWORD flags) {
	::keybd_event(0, static_cast<BYTE>(code), flags, 0);
}

void KeybdEvent::AddEvent(BYTE code, DWORD flags) {
	const KeyAction action(code, flags, key_frame + key_delay);
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
