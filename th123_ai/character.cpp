#include "stdafx.h"

Character::TYPE Character::SetType(TYPE a) {
	TYPE temp;
	temp = type;
	type = a;
	return temp;
}
unsigned int Character::SetRootAddress(unsigned int addr) {
	const unsigned int result = character_manager_addr;
	character_manager_addr = addr;
	return result;
}
int Character::GetSpecialData(unsigned int n) const {
	int data,i;
	short mini;
	float f;

	data = 0;
	switch(n) {
		case 0://�u���̐��E�v
			if(char_id!=CHAR_SAKUYA) {
				return -1;
			}
			ReadProcessMemory(ph, base_addr+ADDR_SAKUYAWORLDOFS, mini);
			if(mini==-1)data = 0;
			else data = mini;
			break;
		case 1://�����u�v���C�x�[�g�X�N�E�F�A�v
			if(char_id!=CHAR_SAKUYA) {
				return -1;
			}
			ReadProcessMemory(ph, base_addr+ADDR_PRIVATESQOFS, mini);
			if(mini==-1)data = 0;
			else data = mini;
			break;
		case 2://�u���z�V���v
			if(char_id!=CHAR_REIMU) {
				return -1;
			}
			ReadProcessMemory(ph, base_addr+ADDR_TENSEITIMEOFS, mini);
			if(mini==-1)data = 0;
			else data = mini;
			break;
		case 3://�����u�H���̋�ցv//��鮁u�H�������������̖@�v
			if(char_id!=CHAR_YOUMU) {
				return -1;
			}
			ReadProcessMemory(ph, base_addr+ADDR_TEKETENOFS, mini);
			if(mini==-1)data = 0;
			else data = mini;
			break;
		case 4://�̗͉�
			ReadProcessMemory(ph, base_addr+ADDR_LIFERECOVERYOFS, mini);
			if(mini==-1)data = 0;
			else data = mini;
			break;
		case 5://�V���u�I�[�����[�Y�T���v
			if(char_id!=CHAR_MARISA) {
				return -1;
			}
			ReadProcessMemory(ph, base_addr+ADDR_ORRERYOFS, mini);
			if(mini==-1)data = 0;
			else data = mini;
			break;
		case 6://�ΐ��؋��y���u���҂̐΁v
			if(char_id!=CHAR_PATCHOULI) {
				return -1;
			}
			ReadProcessMemory(ph, base_addr+ADDR_PHILOSOPHEROFS, mini);
			if(mini==-1)data = 0;
			else data = mini;
			break;
		case 7://����u���m���o�̖�v
			if(char_id!=CHAR_UDONGE) {
				return -1;
			}
			if(::IsSWR()) {
				ReadProcessMemory(ph, base_addr+0x4BC, f);
				data = (int)floor((f-1.05)/0.05+0.5);
				if(data<0) {
					data = 0;
				}
			} else {
				ReadProcessMemory(ph, base_addr+ADDR_KOKUSHIOFS, data);
				if(data < 0 || data > 3) {
					ReadProcessMemory(ph, base_addr+ADDR_KOKUSHIOFS, f);
					data = (int)f;
					if(data < 0 || data > 3) {
						data = 0;
					}
				}
			}
			break;
		case 8://�u���z�V���v�_����
			if(char_id!=CHAR_REIMU) {
				return -1;
			}
			ReadProcessMemory(ph, base_addr+ADDR_TENSEINUMOFS, mini);
			if(mini<1)data = 0;
			else data = mini;
			break;
		case 9://��z�u��z���z�̌��v
			ReadProcessMemory(ph, base_addr+ADDR_HIHISOUOFS, mini);
			if(mini==-1)data = 0;
			else data = mini;
			break;
		case 10://�C���u���O���z�̋��n�v
			if(char_id!=CHAR_TENSHI) {
				return -1;
			}
			ReadProcessMemory(ph, base_addr+ADDR_MUNENOFS, mini);
			if(mini==-1)data = 0;
			else data = mini;
			break;
		case 11://�t�B�[���h�E���g�����b�h
			if(char_id!=CHAR_UDONGE) {
				return -1;
			}
			ReadProcessMemory(ph, base_addr+ADDR_FIELDREDOFS, mini);
			if(mini==-1)data = 0;
			else data = mini;
			break;
		case 12://�t�B�[���h�E���g���o�C�I���b�g
			if(char_id!=CHAR_UDONGE) {
				return -1;
			}
			ReadProcessMemory(ph, base_addr+ADDR_FIELDPURPLEOFS, mini);
			if(mini==-1)data = 0;
			else data = mini;
			break;
		case 13://�����u�W�F���[�t�B�b�V���v�����Z�X�v�c�莞��
		case 14://���A�c��HP
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
		case 15://�S�_�u�~�b�V���O�p�[�v���p���[�v
			if(char_id!=CHAR_SUIKA) {
				return -1;
			}
			ReadProcessMemory(ph, base_addr+ADDR_MPPOFS, mini);
			if(mini==-1)data = 0;
			else data = mini;
			break;
		case 16://�ŉ����u���z�D���̋ʁv
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
		case 17://����_�g�p��
			ReadProcessMemory(ph, base_addr+ADDR_ATTACKSTICKOFS, f);
			data = (int)f;
			if(data < 0 || data > 4) {
				data = 0;
			}
			break;
		case 18://�g����l�`�g�p��
			ReadProcessMemory(ph, base_addr+ADDR_DEFENSEDOLLOFS, f);
			data = (int)f;
			if(data < 0 || data > 4) {
				data = 0;
			}
			break;
		case 19://�V��c��g�p��
			ReadProcessMemory(ph, base_addr+ADDR_UTHIWAOFS, mini);
			data = mini;
			if(data < 0 || data > 4) {
				data = 0;
			}
			break;
		case 20://�O�������[���g�p��
			ReadProcessMemory(ph, base_addr+ADDR_GRIMOREOFS, mini);
			data = mini;
			if(data < 0 || data > 4) {
				data = 0;
			}
			break;
		case 21://�O���̐��H�g�p��
			ReadProcessMemory(ph, base_addr+ADDR_DROPWATEROFS, mini);
			data = mini;
			if(data < 0 || data > 3) {
				data = 0;
			}
			break;
		case 22://�O���̐��H���G���Ԏc��t���[����
			ReadProcessMemory(ph, base_addr+ADDR_DROPWATERTIMEOFS, mini);
			data = mini;
			if(data < 0) {
				data = 0;
			}
			break;
		case 23://�������G���Ԏc��t���[����
			ReadProcessMemory(ph, base_addr+ADDR_RYUUSEIOFS, mini);
			data = mini;
			if(data < 0) {
				data = 0;
			}
			break;
		case 24://�_�ގq�Q�[�W
			if(char_id!=CHAR_SANAE) {
				return -1;
			}
			ReadProcessMemory(ph, base_addr+ADDR_KANAKOOFS, data);
			if(data < 0) {
				data = 0;
			}
			break;
		case 25://�z�K�q�Q�[�W
			if(char_id!=CHAR_SANAE) {
				return -1;
			}
			ReadProcessMemory(ph, base_addr+ADDR_SUWAKOOFS, data);
			if(data < 0) {
				data = 0;
			}
			break;
		case 26://�C���t���A�h���[��
			if(char_id!=CHAR_UDONGE) {
				return -1;
			}
			ReadProcessMemory(ph, base_addr+ADDR_FIELDRED2OFS, mini);
			if(mini==-1)data = 0;
			else data = mini;
			break;
		case 27://�_�C�A�����h�n�[�h�l�X
			if(char_id!=CHAR_PATCHOULI) {
				return -1;
			}
			ReadProcessMemory(ph, base_addr+ADDR_DIAHARDOFS, mini);
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
void Character::ReloadBaseAddr(AI_MODE mode) {
	if( (mode == AI_MODE_1P && type == MY) || (mode == AI_MODE_2P && type == ENEMY)) {
		ReadProcessMemory(ph, character_manager_addr+ADDR_LCHAROFS, base_addr);
		ReadProcessMemory(ph, ADDR_LCHARID, &char_id, 4);
	} else {
		ReadProcessMemory(ph, character_manager_addr+ADDR_RCHAROFS, base_addr);
		ReadProcessMemory(ph, ADDR_RCHARID, &char_id, 4);
	}
}
void Character::ReloadVal(AI_MODE mode) {
	ReadProcessMemory(ph, base_addr+ADDR_AIRDASHCOUNTOFS, air_dash_count);
	ReadProcessMemory(ph, base_addr+ADDR_REIPOWEROFS, rei);
	ReadProcessMemory(ph, base_addr+ADDR_REISTOPROFS, rei_stop);
	ReadProcessMemory(ph, base_addr+ADDR_MAXREIPOWEROFS, rei_max);
	ReadProcessMemory(ph, base_addr+ADDR_TIMESTOPOFS, time_stop);
	ReadProcessMemory(ph, base_addr+ADDR_CORRECTIONOFS, correction);
	ReadProcessMemory(ph, base_addr+ADDR_COMBORATEOFS, combo.rate);
	ReadProcessMemory(ph, base_addr+ADDR_COMBOCOUNTOFS, combo.count);
	ReadProcessMemory(ph, base_addr+ADDR_COMBODAMAGEOFS, combo.damage);
	ReadProcessMemory(ph, base_addr+ADDR_COMBOLIMITOFS, combo.limit);
	ReadProcessMemory(ph, base_addr+ADDR_ATTACKPOWEROFS, status.attack);
	ReadProcessMemory(ph, base_addr+ADDR_DEFENSEPOWEROFS, status.defense);
	ReadProcessMemory(ph, base_addr+ADDR_SPEEDPOWEROFS, status.speed);
	if(weather==11) {
		gauge = 0;
		card_id = -1;
	} else {
		gauge = 0;
		ReadProcessMemory(ph, base_addr+ADDR_CARDGAUGEOFS, &gauge, 2);
		unsigned char temp;
		ReadProcessMemory(ph, base_addr+ADDR_CARDCOUNTOFS, temp);
		gauge += temp * 500;
		card_id = GetCardId(type,0);
	}
	win_count = 0;
	ReadProcessMemory(ph, base_addr+ADDR_WINCNTOFS, win_count);

	{
		int count;
		int point;
		int base;
		int max;
		int p;
		ReadProcessMemory(ph, base_addr+ADDR_CARDCOUNT2OFS, count);
		if(!(count<0 || count>5)) {
			ReadProcessMemory(ph, base_addr+ADDR_SELECTCARDOFS, point);
			ReadProcessMemory(ph, base_addr+ADDR_HANDCARDBASEOFS, base);
			ReadProcessMemory(ph, base_addr+ADDR_HANDCARDMAXOFS, max);
			int i = 0;
			while(i < 5) {
				if(weather!=11 && max>0 && i<gauge/500) {
					ReadProcessMemory(ph, base+((point+i)%max)*4, p);
					ReadProcessMemory(ph, p, &card[i], 4);
				} else {
					card[i].id = -1;
					card[i].cost = -1;
				}
				i++;
			}
		}
	}
	//�X�L�����x���ꗗ
	ReadProcessMemory(ph, base_addr+ADDR_SKILLLEVELMAPOFS, skill, sizeof(skill), NULL);
	{
		int i = 0;
		while(i < SPECIALDATA_MAX) {
			sp_data[i] = GetSpecialData(i);
			i++;
		}
	}
	{//�L�[���͏��
		int p;
		ReadProcessMemory(ph, base_addr+ADDR_KEYMGROFS, p);
		ReadProcessMemory(ph, p, p);
		int i = 0;
		while(i < 8) {
			ReadProcessMemory(ph, p+ADDR_KEYMAPOFS+i*4, keyState[i]);
			i++;
		}
	}
	ReloadObject(mode);
	ReloadLuaValue(mode);
}

void Character::ReloadObject(AI_MODE mode) {
	//STL���X�g�R���e�i
	typedef struct{
		int alloc;
		int head;
		int size;
	}OBJ_LIST;

	//STL���X�g�R���e�i�̃m�[�h
	typedef struct{
		int next;
		int prev;
		int val;
	}NODE;

	int sysObj;
	int objCharHead;
	int objCharTail;
	int objChar;
	int objCharNum;
	int objProjMgr;
	OBJ_LIST objProjList;
	NODE objProjIter;

	//������
	object.clear();

	//�����Ǘ��p�̃I�u�W�F�N�g���擾����
	if(!ReadProcessMemory(ph, ADDR_GAMEDATAMGR, sysObj)) {
		return;
	}

	//�L�����N�^�ϒ��z��̏����擾����
	//(std::vector<ICharacter*>)
	if(!ReadProcessMemory(ph, sysObj + ADDR_CHAROBJHEADOFS, objCharHead)) {
		return;
	}
	if(!ReadProcessMemory(ph, sysObj + ADDR_CHAROBJTAILOFS, objCharTail)) {
		return;
	}
	objCharNum = (objCharTail - objCharHead) / 4;
	if(objCharNum!=2) {
		return;
	}

	int p;
	if( (mode == AI_MODE_1P && type == MY) || (mode == AI_MODE_2P && type == ENEMY)) {
		p = 0;
	} else {
		p = 1;
	}
	//�L�����N�^�I�u�W�F�N�g�̊Ǘ��I�u�W�F�N�g���擾����
	//(CharacterObjectManager<CharacterObject *>*)
	if(!ReadProcessMemory(ph, objCharHead + p*4, objChar)) {
		return;
	}
	if(!ReadProcessMemory(ph, objChar + ADDR_OBJLISTMGR, objProjMgr)) {
		return;
	}

	//�L�����N�^�I�u�W�F�N�g���X�g�𑖍�����
	//(std::list<CharacterObject *>)
	objProjMgr = objProjMgr + 4;
	if(!ReadProcessMemory(ph, objProjMgr + ADDR_OBJPROJOFS, &objProjList, sizeof(objProjList))) {
		return;
	}
	if(!ReadProcessMemory(ph, objProjList.head, &objProjIter, sizeof(objProjIter))) {
		return;
	}
	for(unsigned int k = 0; objProjList.head != objProjIter.next && objProjIter.next != 0 && objProjList.size > k; k++) {
		if(!ReadProcessMemory(ph, objProjIter.next, &objProjIter, sizeof(objProjIter))) {
			return;
		}
		Obj a(ph, objProjIter.val);
		a.Reload(mode);
		object.push_back(a);
	}
}

void Character::ReloadLuaValue(AI_MODE) {
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
	setValue("hitarea_n",	hitarea.size());
	setValue("attackarea_n",	attackarea.size());
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

const Obj &Character::GetObject(unsigned int index) const {
	BOOST_ASSERT(object.size() > index);
	return object[index];
}

const Obj *Character::GetOptionObject(unsigned int index) const {
	struct Inner {
		static bool IsAliceOption(const Obj &obj) {
			if(obj.attackarea.size() != 0 || obj.action != 805 || obj.img_no == 221) {
				return false;
			}
			return true;
		}
		static bool IsYoumuOption(const Obj &obj) {
			return (obj.action == 899);
		}
		static bool IsKomachiOption(const Obj &obj) {
			if(obj.action != 801 && obj.action != 852 && obj.action != 855) {
				return false;
			}
			return true;
		}
	};
	static bool (* const is_option_func_list[SWRS_CHAR_ID_MAX+1])(const Obj &) = {
		NULL,				NULL,				NULL,	Inner::IsAliceOption,
		NULL,				Inner::IsYoumuOption,	NULL,	NULL,
		NULL,				NULL,				NULL,	NULL,
		Inner::IsKomachiOption,	NULL,				NULL,	NULL,
		NULL,				NULL,				NULL,	NULL
	};
	if(object.size() <= index || (this->char_id == CHAR_YOUMU && index != 0) || is_option_func_list[this->char_id] == NULL) {
		return false;
	}
	unsigned int found_count = 0;
	for(unsigned int i = 0; index <= found_count && i < object.size(); i++) {
		if(!is_option_func_list[this->char_id](object[i])) {
			continue;
		}
		if(index == found_count) {
			return &object[i];
		}
		found_count++;
	}
	return NULL;
}

int Character::GetKeyState(int n) const {
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
