
#pragma once

enum{ACT_UP,ACT_DOWN,ACT_LEFT,ACT_RIGHT,ACT_A,ACT_B,ACT_C,ACT_D,ACT_AB,ACT_BC,ACT_DLEFT,ACT_DRIGHT,ACT_ULEFT,ACT_URIGHT};
enum{MY,ENEMY};


#define SPECIALDATA_MAX	28

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



class obj_base
{
public:
	int base_addr;
	float x;
	float y;
	struct speed{
		float x;
		float y;
	};
	struct speed speed;
	char dir;
	short action;
	short act_block;
	int frame;
	int img_no;
	short hp;
	short hit_stop;
	char attackarea_n;
	char hitarea_n;
	Box hitarea[16];
	Box attackarea[16];
	unsigned int fflags;
	unsigned int aflags;

	obj_base() {
		initialize();
	}
	obj_base(HANDLE h) {
		initialize();
		SetProcessHandle(h);
	}

	virtual void ReloadBaseAddr(int) = 0;
	virtual void ReloadVal(int) = 0;

	HANDLE SetProcessHandle(HANDLE handle) {
		HANDLE temp;
		temp = ph;
		ph = handle;
		return ph;
	}
	void Reload(int param) {
		ReloadBaseAddr(param);
		if(base_addr == 0) {
			return;
		}
		int temp;
		//base+0x08	4	画像構造体的な何かへのアドレス？
		ReadProcessMemoryFloat(ph,(void *)(base_addr+ADDR_POINTXOFS),&x,4,NULL);
		ReadProcessMemoryFloat(ph,(void *)(base_addr+ADDR_POINTYOFS),&y,4,NULL);
		ReadProcessMemoryFloat(ph,(void *)(base_addr+ADDR_SPEEDXOFS),&speed.x,4,NULL);
		ReadProcessMemoryFloat(ph,(void *)(base_addr+ADDR_SPEEDYOFS),&speed.y,4,NULL);
		ReadProcessMemory(ph,(void *)(base_addr+ADDR_DIRECTIONOFS),&dir,1,NULL);//0x104は画像の向き、0x1AEは向き+何か
		//base+0x11c	4	x倍率
		//base+0x120	4	y倍率
		ReadProcessMemory(ph,(void *)(base_addr+ADDR_ACTIONIDOFS),&action,2,NULL);
		ReadProcessMemory(ph,(void *)(base_addr+ADDR_ACTIONBLOCKIDOFS),&act_block,2,NULL);
		//base+0x140	2	そのアクションの何画像目か
		//base+0x142	2	その画像の何フレーム目か
		ReadProcessMemory(ph,(void *)(base_addr+ADDR_FRAMECOUNTOFS),&frame,4,NULL);
		//base+0x150	4	136byteのアニメーション(?)構造体のアドレス(anim)
		//anim+0x0A	4	画像ID
		ReadProcessMemory(ph,(void *)(base_addr+ADDR_IMAGESTRUCTOFS),&temp,4,NULL);
		ReadProcessMemory(ph,(void *)(temp + ADDR_IMAGENUMBEROFS),&img_no,4,NULL);
		ReadProcessMemory(ph,(void *)(temp + ADDR_FRAMEFLAGSOFS),&fflags,4,NULL);
		ReadProcessMemory(ph,(void *)(temp + ADDR_ATTACKFLAGSOFS),&aflags,4,NULL);
		//base+0x158	4	158byteの構造体(詳細不明)
		ReadProcessMemory(ph,(void *)(base_addr+ADDR_HPOFS),&hp,2,NULL);
		ReadProcessMemory(ph,(void *)(base_addr+ADDR_HITSTOPOFS),&hit_stop,2,NULL);
		ReadProcessMemory(ph,(void *)(base_addr+ADDR_ATTACKAREACOUNTOFS),&attackarea_n,1,NULL);
		ReadProcessMemory(ph,(void *)(base_addr+ADDR_HITAREACOUNTOFS),&hitarea_n,1,NULL);

		if(is_swr) {//食らい判定取得
			int p;
			ReadProcessMemory(ph, (void *)(base_addr+ADDR_MOTIONSTRUCTOFS), &p, 4, NULL);
			ReadProcessMemory(ph, (void *)(p+ADDR_HITAREAOFS), &p, 4, NULL);
			int i = 0;
			Box *a = hitarea;
			while(i < 10) {
				if(i < hitarea_n) {
					ReadProcessMemory(ph, (void *)(p + i*16), a, sizeof(*a), NULL);
					a->left = (int)x+a->left;
					a->top = -(int)y+a->top;
					a->right = (int)x+a->right;
					a->bottom = -(int)y+a->bottom;
					a->top *= -1;
					a->bottom *= -1;
				} else {
					memset(a, 0, sizeof(*a));
				}
				a++;
				i++;
			}
		} else {
			int i = 0;
			Box *a = hitarea;
			while(i < 16) {
				if(i < hitarea_n) {
					DWORD flag;
					ReadProcessMemory(ph, (void *)(base_addr + ADDR_HITAREAFLAGOFS + i*sizeof(flag)), &flag,sizeof(flag),NULL);
					ReadProcessMemory(ph, (void *)(base_addr + ADDR_HITAREA2OFS + i*sizeof(*a)), a, sizeof(*a),NULL);
					if(flag!=0) {//相対座標
						a->left = (int)x+a->left;
						a->top = -(int)y+a->top;
						a->right = (int)x+a->right;
						a->bottom = -(int)y+a->bottom;
					}
					a->top *= -1;
					a->bottom *= -1;
				} else {
					memset(a, 0, sizeof(*a));
				}
				a++;
				i++;
			}
		}
		{	//攻撃判定取得
			int p;
			int i = 0;
			Box *a = attackarea;
			while(i < 16) {
				if(i < attackarea_n) {
					ReadProcessMemory(ph,(void *)(base_addr+ADDR_ATTACKAREAOFS + i*4),&p,4,NULL);
					if(p == 0) {
						//当たり判定矩形を取得して表示する(地面最左下端からの相対)
						ReadProcessMemory(ph,(void *)(base_addr+ADDR_ATTACKAREA2OFS + i*sizeof(*a)),a,sizeof(*a),NULL);
					} else {
						//当たり判定矩形を取得して表示する(オブジェクト位置からの相対)
						ReadProcessMemory(ph,(void *)p,a,16,NULL);
						a->left = (int)x+a->left;
						a->top = -(int)y+a->top;
						a->right = (int)x+a->right;
						a->bottom = -(int)y+a->bottom;
					}
					a->top *= -1;
					a->bottom *= -1;
				} else {
					memset(a,0,sizeof(*a));
				}
				a++;
				i++;
			}
		}/*
		fflags =   0x003FFFFF;
		fflags &= ~0x0007E7C4;
		fflags |=  0x00000031;
		WriteProcessMemory(ph,(void *)(temp + ADDR_FRAMEFLAGSOFS),&fflags,4,NULL);*/
		ReloadVal(param);
	}

	enum{
		FFLAG_STAND,	FFLAG_CROUCHI,	FFLAG_AIR,	FFLAG_GUARD,
		FFLAG_CANCEL
	};
	bool CheckFFlags(int n) {
		return fflags&n;
	}
	bool CheckAFlags(int n) {
		return aflags&n;
	}
protected:
	HANDLE ph;

	void initialize() {
		attackarea_n = hitarea_n = 0;
		base_addr = 0;
	}
};

class obj : public obj_base
{
public:
	obj(HANDLE h,int addr) {
		ph = h;
		base_addr = addr;
	}

	void ReloadBaseAddr(int) {
	}

	void ReloadVal(int) {
	}
};

class player : public obj_base
{
public:
	enum TYPE{
		MY,ENEMY
	};
	player(TYPE a) {
		type = a;
	}

	char prof[20];
	char correction;
	int win_count;
	CHAR_ID char_id;
	int rei;
	int rei_max;
	int rei_stop;
	int gauge;
	struct combo{
		int count;
		int damage;
		int limit;
		float rate;
	};
	struct combo combo;
	short card_id;
	char air_dash_count;
	std::vector<obj> object;
	struct status{
		float attack;
		float defense;
		float speed;
	};
	struct status status;
	short time_stop;
	struct card{
		short id;
		short cost;
	};
	struct card card[5];
	char skill[16];
	int sp_data[SPECIALDATA_MAX];
	int keyState[8];

	TYPE SetType(TYPE a) {
		TYPE temp;
		temp = type;
		type = a;
		return temp;
	}
	int SetRootAddress(int addr) {
		int temp;
		temp = root;
		root = addr;
		return temp;
	}
	int GetSpecialData(int n) {
		int data,i;
		short mini;
		float f;

		data = 0;
		switch(n) {
			case 0://「咲夜の世界」
				if(char_id!=CHAR_SAKUYA) {
					return -1;
				}
				ReadProcessMemory(ph,(void*)(base_addr+ADDR_SAKUYAWORLDOFS),&mini,2,NULL);
				if(mini==-1)data = 0;
				else data = mini;
				break;
			case 1://時符「プライベートスクウェア」
				if(char_id!=CHAR_SAKUYA) {
					return -1;
				}
				ReadProcessMemory(ph,(void*)(base_addr+ADDR_PRIVATESQOFS),&mini,2,NULL);
				if(mini==-1)data = 0;
				else data = mini;
				break;
			case 2://「夢想天生」
				if(char_id!=CHAR_REIMU) {
					return -1;
				}
				ReadProcessMemory(ph,(void*)(base_addr+ADDR_TENSEITIMEOFS),&mini,2,NULL);
				if(mini==-1)data = 0;
				else data = mini;
				break;
			case 3://魂符「幽明の苦輪」//魂魄「幽明求聞持聡明の法」
				if(char_id!=CHAR_YOUMU) {
					return -1;
				}
				ReadProcessMemory(ph,(void*)(base_addr+ADDR_TEKETENOFS),&mini,2,NULL);
				if(mini==-1)data = 0;
				else data = mini;
				break;
			case 4://体力回復
				ReadProcessMemory(ph,(void*)(base_addr+ADDR_LIFERECOVERYOFS),&mini,2,NULL);
				if(mini==-1)data = 0;
				else data = mini;
				break;
			case 5://儀符「オーレリーズサン」
				if(char_id!=CHAR_MARISA) {
					return -1;
				}
				ReadProcessMemory(ph,(void*)(base_addr+ADDR_ORRERYOFS),&mini,2,NULL);
				if(mini==-1)data = 0;
				else data = mini;
				break;
			case 6://火水木金土符「賢者の石」
				if(char_id!=CHAR_PATCHOULI) {
					return -1;
				}
				ReadProcessMemory(ph,(void*)(base_addr+ADDR_PHILOSOPHEROFS),&mini,2,NULL);
				if(mini==-1)data = 0;
				else data = mini;
				break;
			case 7://生薬「国士無双の薬」
				if(char_id!=CHAR_UDONGE) {
					return -1;
				}
				if(is_swr) {
					ReadProcessMemoryFloat(ph,(void*)(base_addr+0x4BC),&f,4,NULL);
					data = (int)floor((f-1.05)/0.05+0.5);
					if(data<0) {
						data = 0;
					}
				} else {
					ReadProcessMemory(ph,(void*)(base_addr+ADDR_KOKUSHIOFS),&data,4,NULL);
					if(data < 0 || data > 3) {
						ReadProcessMemoryFloat(ph,(void*)(base_addr+ADDR_KOKUSHIOFS),&f,4,NULL);
						data = (int)f;
						if(data < 0 || data > 3) {
							data = 0;
						}
					}
				}
				break;
			case 8://「夢想天生」点灯数
				if(char_id!=CHAR_REIMU) {
					return -1;
				}
				ReadProcessMemory(ph,(void*)(base_addr+ADDR_TENSEINUMOFS),&mini,2,NULL);
				if(mini<1)data = 0;
				else data = mini;
				break;
			case 9://非想「非想非非想の剣」
				ReadProcessMemory(ph,(void*)(base_addr+ADDR_HIHISOUOFS),&mini,2,NULL);
				if(mini==-1)data = 0;
				else data = mini;
				break;
			case 10://気符「無念無想の境地」
				if(char_id!=CHAR_TENSHI) {
					return -1;
				}
				ReadProcessMemory(ph,(void*)(base_addr+ADDR_MUNENOFS),&mini,2,NULL);
				if(mini==-1)data = 0;
				else data = mini;
				break;
			case 11://フィールドウルトラレッド
				if(char_id!=CHAR_UDONGE) {
					return -1;
				}
				ReadProcessMemory(ph,(void*)(base_addr+ADDR_FIELDREDOFS),&mini,2,NULL);
				if(mini==-1)data = 0;
				else data = mini;
				break;
			case 12://フィールドウルトラバイオレット
				if(char_id!=CHAR_UDONGE) {
					return -1;
				}
				ReadProcessMemory(ph,(void*)(base_addr+ADDR_FIELDPURPLEOFS),&mini,2,NULL);
				if(mini==-1)data = 0;
				else data = mini;
				break;
			case 13://水符「ジェリーフィッシュプリンセス」残り時間
			case 14://同、残りHP
				if(char_id!=CHAR_PATCHOULI) {
					return -1;
				}
				i = 0;
				while(i<object.size()  && (object[i].action!=0x358 || (object[i].img_no!=0x154 && object[i].img_no!=0x1B3))) {
					i++;
				}
				if(i == object.size()) {
					data = 0;
				} else {
					if(n==13) {
						data = 361 - object[i].frame;
					} else {
						data = 3001+object[i].hp;
					}
				}
				break;
			case 15://鬼神「ミッシングパープルパワー」
				if(char_id!=CHAR_SUIKA) {
					return -1;
				}
				ReadProcessMemory(ph,(void*)(base_addr+ADDR_MPPOFS),&mini,2,NULL);
				if(mini==-1)data = 0;
				else data = mini;
				break;
			case 16://毒煙幕「瓦斯織物の玉」
				if(char_id!=CHAR_UDONGE) {
					return -1;
				}
				i = 0;
				while(i<object.size() && (object[i].action!=0x35A || (object[i].img_no!=0xF0 && object[i].img_no!=0x15F))) {
					i++;
				}
				if(i==object.size()) {
					data = 0;
				} else {
					data = 601 - object[i].frame;
				}
				break;
			case 17://制御棒使用個数
				ReadProcessMemory(ph,(void*)(base_addr+ADDR_ATTACKSTICKOFS),&f,4,NULL);
				data = (int)f;
				if(data < 0 || data > 4) {
					data = 0;
				}
				break;
			case 18://身代わり人形使用個数
				ReadProcessMemory(ph,(void*)(base_addr+ADDR_DEFENSEDOLLOFS),&f,4,NULL);
				data = (int)f;
				if(data < 0 || data > 4) {
					data = 0;
				}
				break;
			case 19://天狗団扇使用個数
				ReadProcessMemory(ph,(void*)(base_addr+ADDR_UTHIWAOFS),&data,2,NULL);
				if(data < 0 || data > 4) {
					data = 0;
				}
				break;
			case 20://グリモワール使用個数
				ReadProcessMemory(ph,(void*)(base_addr+ADDR_GRIMOREOFS),&data,2,NULL);
				if(data < 0 || data > 4) {
					data = 0;
				}
				break;
			case 21://三粒の水滴使用個数
				ReadProcessMemory(ph,(void*)(base_addr+ADDR_DROPWATEROFS),&data,2,NULL);
				if(data < 0 || data > 3) {
					data = 0;
				}
				break;
			case 22://三粒の水滴無敵時間残りフレーム数
				ReadProcessMemory(ph,(void*)(base_addr+ADDR_DROPWATERTIMEOFS),&data,2,NULL);
				if(data < 0) {
					data = 0;
				}
				break;
			case 23://竜星無敵時間残りフレーム数
				ReadProcessMemory(ph,(void*)(base_addr+ADDR_RYUUSEIOFS),&data,2,NULL);
				if(data < 0) {
					data = 0;
				}
				break;
			case 24://神奈子ゲージ
				if(char_id!=CHAR_SANAE) {
					return -1;
				}
				ReadProcessMemory(ph,(void*)(base_addr+ADDR_KANAKOOFS),&data,4,NULL);
				if(data < 0) {
					data = 0;
				}
				break;
			case 25://諏訪子ゲージ
				if(char_id!=CHAR_SANAE) {
					return -1;
				}
				ReadProcessMemory(ph,(void*)(base_addr+ADDR_SUWAKOOFS),&data,4,NULL);
				if(data < 0) {
					data = 0;
				}
				break;
			case 26://インフレアドムーン
				if(char_id!=CHAR_UDONGE) {
					return -1;
				}
				ReadProcessMemory(ph,(void*)(base_addr+ADDR_FIELDRED2OFS),&mini,2,NULL);
				if(mini==-1)data = 0;
				else data = mini;
				break;
			case 27://ダイアモンドハードネス
				if(char_id!=CHAR_PATCHOULI) {
					return -1;
				}
				ReadProcessMemory(ph, (void*)(base_addr+ADDR_DIAHARDOFS),&mini,2,NULL);
				if(mini==-1) {
					data = 0;
				} else {
					data = mini;
				}
				break;
			default:
				data = -1;
				break;
		}
		return data;
	}
	virtual void ReloadBaseAddr(int mode) {
		if( (mode==1 && type==MY) || (mode==2 && type==ENEMY)) {
			ReadProcessMemory(ph,(void *)(root+ADDR_LCHAROFS),&base_addr,4,NULL);
			ReadProcessMemory(ph,(void *)ADDR_LCHARID,&char_id,4,NULL);
		} else {
			ReadProcessMemory(ph,(void *)(root+ADDR_RCHAROFS),&base_addr,4,NULL);
			ReadProcessMemory(ph,(void *)ADDR_RCHARID,&char_id,4,NULL);
		}
	}
	virtual void ReloadVal(int mode) {
		ReadProcessMemory(ph,(void *)(base_addr+ADDR_AIRDASHCOUNTOFS),&air_dash_count,2,NULL);
		ReadProcessMemory(ph,(void *)(base_addr+ADDR_REIPOWEROFS),&rei,2,NULL);
		ReadProcessMemory(ph,(void *)(base_addr+ADDR_REISTOPROFS),&rei_stop,2,NULL);
		ReadProcessMemory(ph,(void *)(base_addr+ADDR_MAXREIPOWEROFS),&rei_max,2,NULL);
		ReadProcessMemory(ph,(void *)(base_addr+ADDR_TIMESTOPOFS),&time_stop,2,NULL);
		ReadProcessMemory(ph,(void *)(base_addr+ADDR_CORRECTIONOFS),&correction,1,NULL);
		ReadProcessMemoryFloat(ph,(void *)(base_addr+ADDR_COMBORATEOFS),&combo.rate,4,NULL);
		ReadProcessMemory(ph,(void *)(base_addr+ADDR_COMBOCOUNTOFS),&combo.count,2,NULL);
		ReadProcessMemory(ph,(void *)(base_addr+ADDR_COMBODAMAGEOFS),&combo.damage,2,NULL);
		ReadProcessMemory(ph,(void *)(base_addr+ADDR_COMBOLIMITOFS),&combo.limit,2,NULL);
		ReadProcessMemoryFloat(ph,(void *)(base_addr+ADDR_ATTACKPOWEROFS),&status.attack,4,NULL);
		ReadProcessMemoryFloat(ph,(void *)(base_addr+ADDR_DEFENSEPOWEROFS),&status.defense,4,NULL);
		ReadProcessMemoryFloat(ph,(void *)(base_addr+ADDR_SPEEDPOWEROFS),&status.speed,4,NULL);
		if(weather==11) {
			gauge = 0;
			card_id = -1;
		} else {
			int temp;
			ReadProcessMemory(ph,(void *)(base_addr+ADDR_CARDGAUGEOFS),&gauge,2,NULL);
			temp = 0;
			ReadProcessMemory(ph,(void *)(base_addr+ADDR_CARDCOUNTOFS),&temp,1,NULL);
			gauge += temp*500;
			card_id = GetCardId(type,0);
		}
		ReadProcessMemory(ph,(void *)(base_addr+ADDR_WINCNTOFS), &win_count,1,NULL);

		{
			int count;
			int point;
			int base;
			int max;
			int p;
			ReadProcessMemory(ph,(void *)(base_addr+ADDR_CARDCOUNT2OFS),&count,4,NULL);
			if(!(count<0 || count>5)) {
				ReadProcessMemory(ph,(void *)(base_addr+ADDR_SELECTCARDOFS),&point,4,NULL);
				ReadProcessMemory(ph,(void *)(base_addr+ADDR_HANDCARDBASEOFS),&base,4,NULL);
				ReadProcessMemory(ph,(void *)(base_addr+ADDR_HANDCARDMAXOFS),&max,4,NULL);
				int i = 0;
				while(i < 5) {
					if(weather!=11 && max>0 && i<gauge/500) {
						ReadProcessMemory(ph,(void *)(base+((point+i)%max)*4),&p,4,NULL);
						ReadProcessMemory(ph,(void *)p,&card[i],4,NULL);
					} else {
						card[i].id = -1;
						card[i].cost = -1;
					}
					i++;
				}
			}
		}
		//スキルレベル一覧
		ReadProcessMemory(ph,(void *)(base_addr+ADDR_SKILLLEVELMAPOFS),skill,sizeof(skill),NULL);
		{
			int i = 0;
			while(i < SPECIALDATA_MAX) {
				sp_data[i] = GetSpecialData(i);
				i++;
			}
		}
		{//キー入力状態
			int p;
			ReadProcessMemory(ph,(void *)(base_addr+ADDR_KEYMGROFS),&p,4,NULL);
			ReadProcessMemory(ph,(void *)p,&p,4,NULL);
			int i = 0;
			while(i < 8) {
				ReadProcessMemory(ph,(void *)(p+ADDR_KEYMAPOFS+i*4),&keyState[i],4,NULL);
				i++;
			}
		}
		ReloadObject(mode);
		ReloadLuaValue(mode);
	}

	//STLリストコンテナ
	typedef struct{
		int alloc;
		int head;
		int size;
	}OBJ_LIST;

	//STLリストコンテナのノード
	typedef struct{
		int next;
		int prev;
		int val;
	}NODE;
	void ReloadObject(int mode) {
		int sysObj;
		DWORD readSize;
		int objCharHead;
		int objCharTail;
		int objChar;
		int objCharNum;
		int objProjMgr;
		OBJ_LIST objProjList;
		NODE objProjIter;

		//初期化
		object.clear();

		//何か管理用のオブジェクトを取得する
		ReadProcessMemory(ph,(void *)ADDR_GAMEDATAMGR,&sysObj,4,&readSize);
		if(readSize != 4)return;

		//キャラクタ可変長配列の情報を取得する
		//(std::vector<ICharacter*>)
		ReadProcessMemory(ph,(void *)(sysObj + ADDR_CHAROBJHEADOFS),&objCharHead,4,&readSize);
		if(readSize != 4)return;
		ReadProcessMemory(ph,(void *)(sysObj + ADDR_CHAROBJTAILOFS),&objCharTail,4,&readSize);
		if(readSize != 4)return;
		objCharNum = (objCharTail - objCharHead) / 4;
		if(objCharNum!=2) {
			return;
		}

		int p;
		if( (mode==1 && type==MY) || (mode==2 && type==ENEMY)) {
			p = 0;
		} else {
			p = 1;
		}
		//キャラクタオブジェクトの管理オブジェクトを取得する
		//(CharacterObjectManager<CharacterObject *>*)
		ReadProcessMemory(ph,(void *)(objCharHead + p*4),&objChar,4,&readSize);
		if(readSize != 4)return;
		ReadProcessMemory(ph,(void *)(objChar + ADDR_OBJLISTMGR),&objProjMgr,4,&readSize);
		if(readSize != 4)return;

		//キャラクタオブジェクトリストを走査する
		//(std::list<CharacterObject *>)
		objProjMgr = objProjMgr + 4;
		ReadProcessMemory(ph,(void *)(objProjMgr + ADDR_OBJPROJOFS),&objProjList,12,&readSize);
		if(readSize != 12)return;
		ReadProcessMemory(ph,(void *)objProjList.head,&objProjIter,12,&readSize);
		if(readSize != 12)return;
		int k = 0;
		while(objProjList.head != objProjIter.next && objProjIter.next != 0 && objProjList.size > k) {
			ReadProcessMemory(ph,(void *)objProjIter.next,&objProjIter,12,&readSize);
			if(readSize!=12) {
				return;
			}
			obj a(ph,objProjIter.val);
			a.Reload(mode);
			object.push_back(a);
			k++;
		}
	}

	void ReloadLuaValue(int mode) {
		char name[32],*s;

#define setValue(k,v)	strcpy(s,k);engine->setScriptValue(name,v)

		if(type==MY) {
			strcpy(name,"my_");
		} else {
			strcpy(name,"enemy_");
		}
		s = &name[strlen(name)];

		setValue("char",char_id);
		setValue("hp",hp);
		setValue("rei",rei);
		setValue("rmax",rei_max);
		setValue("rei_stop",rei_stop);
		setValue("timestop",time_stop);
		setValue("spell",gauge);
		setValue("combo",combo.count);
		setValue("dam",combo.damage);
		setValue("limit",combo.limit);
		setValue("rate",combo.rate);
		setValue("card",card_id);
		setValue("air",air_dash_count);
		setValue("dir",dir);
		setValue("hitstop",hit_stop);
		setValue("xspeed",speed.x);
		setValue("yspeed",speed.y);
		setValue("img",img_no);
		setValue("hitarea_n",hitarea_n);
		setValue("attackarea_n",attackarea_n);
		setValue("correction",correction);
		setValue("fflags",fflags);
		setValue("aflags",aflags);
		setValue("obj_n",object.size());
		setValue("attack_power",status.attack);
		setValue("defense_power",status.defense);
		setValue("speed_power",status.speed);
		setValue("win_count",win_count);

		if(type==MY) {
			strcpy(name,"");
		} else {
			strcpy(name,"e");
		}
		s = &name[strlen(name)];
		setValue("x",x);
		setValue("y",y);
#undef setValue
	}

	obj * GetObject(int n) {
		if(n<0 || object.size()<=n) {
			return NULL;
		}
		return &object[n];
	}

	obj *GetOptionObject(int n) {
		if(n<0 || object.size()<=n) {
			return NULL;
		}
		n++;
		int i = 0;
		while(n>0 && i < object.size()) {
			if(char_id==CHAR_ALICE) {
				if(object[i].attackarea_n==0 && object[i].action == 805 && object[i].img_no!=221)n--;
			} else if(char_id==CHAR_YOUMU) {
				if(n!=1) {
					return NULL;
				}
				if(object[i].action == 899) {
					n--;
				}
			} else if(char_id==CHAR_KOMACHI) {
				if(object[i].action==855)n--;
				if(object[i].action==801 || object[i].action==852)n--;
			} else return NULL;
			if(n<=0) {
				break;
			}
			i++;
		}
		if(n==0) {
			return &object[i];
		}
		return NULL;
	}
	int GetKeyState(int n) {
		switch(n) {
			case ACT_LEFT:
				if(keyState[0] > 0) {
					return 0;
				}
				return -keyState[0];
			case ACT_RIGHT:
				if(keyState[0] < 0) {
					return 0;
				}
				return keyState[0];
			case ACT_UP:
				if(keyState[1] > 0) {
					return 0;
				}
				return -keyState[1];
			case ACT_DOWN:
				if(keyState[1] < 0) {
					return 0;
				}
				return keyState[1];
			case ACT_A:
				return keyState[2];
			case ACT_B:
				return keyState[3];
			case ACT_C:
				return keyState[4];
			case ACT_D:
				return keyState[5];
			case ACT_AB:
				return keyState[6];
			case ACT_BC:
				return keyState[7];
		}
		return -1;
	}
private:
	TYPE type;
	int root;
};

extern player my_data;
extern player enemy_data;


