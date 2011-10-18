
#define SPECIALDATA_MAX	28

class obj : public ObjBase
{
public:
	obj(org::click3::Utility::SHARED_HANDLE ph, unsigned int addr) : ObjBase(ph) {
		base_addr = addr;
	}

	void ReloadBaseAddr(AI_MODE) { }

	void ReloadVal(AI_MODE) { }
};

class player : public ObjBase
{
public:
	enum TYPE{
		MY, ENEMY
	};
	player(TYPE a) : ObjBase() {
		type = a;
	}

	char prof[20];
	char correction;
	unsigned char win_count;
	CHAR_ID char_id;
	unsigned short rei;
	unsigned short rei_max;
	unsigned short rei_stop;
	int gauge;
	struct combo{
		unsigned short count;
		unsigned short damage;
		unsigned short limit;
		float rate;
	} combo;
	short card_id;
	char air_dash_count;
	std::vector<obj> object;
	struct status{
		float attack;
		float defense;
		float speed;
	} status;
	short time_stop;
	struct card{
		short id;
		short cost;
	} card[5];
	char skill[16];
	int sp_data[SPECIALDATA_MAX];
	int keyState[8];

	TYPE SetType(TYPE a);
	int SetRootAddress(int addr);
	int GetSpecialData(int n);
	virtual void ReloadBaseAddr(AI_MODE mode);
	virtual void ReloadVal(AI_MODE mode);
	void ReloadObject(AI_MODE mode);
	void ReloadLuaValue(AI_MODE mode);
	obj *GetObject(int n);
	obj *GetOptionObject(int n);
	int GetKeyState(int n);
private:
	TYPE type;
	int root;
};
