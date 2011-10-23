
#include "stdafx.h"

KeybdEvent keyboard;

int key[10] = {DIK_UP,DIK_DOWN,DIK_LEFT,DIK_RIGHT,DIK_Z,DIK_X,DIK_C,DIK_A,DIK_S,DIK_D};
int muki,gyaku,dis,dfront,dback,ufront,uback,dis_y,dis2,is_dir_front;

char last_on_key;
char on[10] = {0,0,0,0,0,0,0,0,0,0};
char get_key_stat(int n) {
	BOOST_ASSERT(n >= 0);
	BOOST_ASSERT(n <= 9);
	return on[n];
}

void search_key(CHAR_ID operation_char) {
	int map[2][10];
	int i;
	int p;

	p = 0;

	//ゲームパッドがいるなら逆側を使う
	ReadProcessMemory(ph, ADDR_1PKEYMAP, map[0], 10*4);
	ReadProcessMemory(ph, ADDR_2PKEYMAP, map[1], 10*4);
	if(map[0][0]==map[0][1] && map[0][1]==map[0][2] && map[0][2]==map[0][3]) {
		p = 2;
	} else if(map[1][0]==map[1][1] && map[1][1]==map[1][2] && map[1][2]==map[1][3]) {
		p = 1;
	}

	//AIManager指定キャラがいるならそちらを使う
	if(p==0) {
		int char_id[2];
		ReadProcessMemory(ph, ADDR_LCHARID, char_id[0]);
		ReadProcessMemory(ph, ADDR_RCHARID, char_id[1]);
		if(char_id[0]==operation_char && char_id[1]!=operation_char) {
			p = 1;
		} else if(char_id[1]==operation_char && char_id[0]!=operation_char) {
			p = 2;
		}
	}

	//キーコンフィグがデフォルトに近いか、最後に入力されたキーをキーコンフィグに含んでいるなら逆側を使う
	if(p==0) {
		const unsigned char def[10] = {DIK_UP,DIK_DOWN,DIK_LEFT,DIK_RIGHT,DIK_Z,DIK_X,DIK_C,DIK_A,DIK_S,DIK_D};
		int count[2];
		int i = 0;
		while(i < 2) {
			int l = 0;
			count[i] = 0;
			while(l < 10) {
				if(map[i][l] == last_on_key) {
					count[i] += 10;
				}
				if(map[i][l] == def[l]) {
					count[i]++;
				}
				l++;
			}
			i++;
		}
		if(count[0] < count[1]) {
			i = 1;
		} else {
			i = 0;
		}
		if(count[i] > 5) {
			if(i==0) {
				p = 2;
			} else {
				p = 1;
			}
		}
	}

	//全ての条件で検出できなかったらINIの設定を使用する。
	if(p==0) {
		p = ini_int2("Player",1);
		if(p!=2) {
			p = 1;
		}
	}
	i = ini_int2("Player",1);
	if(i!=2) {
		i = 1;
	}
	if(p!=i) {
		if(p==1) {
			ini_add("default","Player","1");
		} else {
			ini_add("default","Player","2");
		}
		change_player();
	}

	p = ini_int2("Player",1);
	if(p!=2) {
		p = 1;
	}
	i = 0;
	while(i < 10) {
		key[i] = map[p-1][i];
		if(key[i]==0x90) {
			key[i] = MapVirtualKey(0xDE,0);
		} else if(key[i]==0x91) {
			key[i] = MapVirtualKey(0xC0,0);
		} else if(key[i]==0x1A) {
			key[i] = MapVirtualKey(0xDB,0);
		} else if(key[i]==0x1B) {
			key[i] = MapVirtualKey(0xDD,0);
		} else if(key[i]==0x92) {
			key[i] = MapVirtualKey(0xBA,0);
		} else if(key[i]==0x2B) {
			key[i] = MapVirtualKey(0xE2,0);
		}
		i++;
	}
}

//ACT_DLEFT以降に対応していないため、key_onとkey_off以外から呼ばないこと
void on_check(int n) {
	if(on[n]) {
		keyboard.AddEvent(key[n],KEYEVENTF_KEYUP);
		on[n] = 0;
	}
	if(n == 2 && on[3]) {
		keyboard.AddEvent(key[3],KEYEVENTF_KEYUP);
		on[3] = 0;
	} else if(n == 3 && on[2]) {
		keyboard.AddEvent(key[2],KEYEVENTF_KEYUP);
		on[2] = 0;
	} else if(n == 0 && on[1]) {
		keyboard.AddEvent(key[1],KEYEVENTF_KEYUP);
		on[1] = 0;
	} else if(n == 1 && on[0]) {
		keyboard.AddEvent(key[0],KEYEVENTF_KEYUP);
		on[0] = 0;
	}
}

void key_on(int n) {
	if(n >= ACT_DLEFT) {
		switch(n) {
			case ACT_DLEFT:
				key_on(ACT_DOWN);
				key_on(ACT_LEFT);
				break;
			case ACT_DRIGHT:
				key_on(ACT_DOWN);
				key_on(ACT_RIGHT);
				break;
			case ACT_ULEFT:
				key_on(ACT_UP);
				key_on(ACT_LEFT);
				break;
			case ACT_URIGHT:
				key_on(ACT_UP);
				key_on(ACT_RIGHT);
				break;
		}
	} else if(!on[n]) {
		on_check(n);
		keyboard.AddEvent(key[n], 0);
		on[n] = 1;
	}
}
void key_off(int n) {
	if(n >= ACT_DLEFT) {//実質的に十字キー全OFFと同義なので
		key_off(ACT_DOWN);
		key_off(ACT_LEFT);
	} else {
		on_check(n);
	}
}
void key_reset(void) {
	int i;
	i = 0;
	while(i < 10) {
		key_off(i);
		i++;
	}
}
int key_check(int n) {
	int a,b;
	if(n >= ACT_DLEFT) {
		switch(n) {
			case ACT_DLEFT:
				a=key_check(ACT_DOWN);
				b=key_check(ACT_LEFT);
				break;
			case ACT_DRIGHT:
				a=key_check(ACT_DOWN);
				b=key_check(ACT_RIGHT);
				break;
			case ACT_ULEFT:
				a=key_check(ACT_UP);
				b=key_check(ACT_LEFT);
				break;
			case ACT_URIGHT:
				a=key_check(ACT_UP);
				b=key_check(ACT_RIGHT);
				break;
		}
		if(a&&b)return 1;
		if(a)return 2;
		if(b)return 3;
	} else if(on[n]) {
		return 1;
	}
	return 0;
}

void set_key_delay(int delay) {
	keyboard.SetKeyDelay(delay);
	engine->setScriptValue("key_delay", keyboard.GetKeyDelay());
}

void check_lastkey() {
	if(seen==0x05 || seen==0x0D || seen==0x0E) {
		return;
	}
	int i=0;
	short t;
	while(i < 224) {
		t = GetAsyncKeyState(i);
		if(t<0 && MapVirtualKey(i,0)!=0) {
			last_on_key = MapVirtualKey(i,0);
			break;
		}
		i++;
	}
}

void Clear() {
	keyboard.Clear();
}
