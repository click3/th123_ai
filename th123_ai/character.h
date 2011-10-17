
#define SPECIALDATA_MAX	28

class obj_base
{
public:
	enum{
		FFLAG_STAND,	FFLAG_CROUCHI,	FFLAG_AIR,	FFLAG_GUARD,
		FFLAG_CANCEL
	};

	unsigned int base_addr;
	float x;
	float y;
	struct speed {
		float x;
		float y;
	} speed;
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
	obj_base(org::click3::Utility::SHARED_HANDLE h) {
		initialize();
		SetProcessHandle(h);
	}

	virtual void ReloadBaseAddr(int) = 0;
	virtual void ReloadVal(int) = NULL;
	
	void SetProcessHandle(org::click3::Utility::SHARED_HANDLE ph);
	void Reload(int param);

	bool CheckFFlags(int n) {
		return fflags&n;
	}
	bool CheckAFlags(int n) {
		return aflags&n;
	}
protected:
	org::click3::Utility::SHARED_HANDLE ph;

	void initialize() {
		attackarea_n = hitarea_n = 0;
		base_addr = 0;
	}
};

class obj : public obj_base
{
public:
	obj(org::click3::Utility::SHARED_HANDLE ph, unsigned int addr) : obj_base(ph) {
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
		MY, ENEMY
	};
	player(TYPE a) : obj_base() {
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
	virtual void ReloadBaseAddr(int mode);
	virtual void ReloadVal(int mode);
	void ReloadObject(int mode);
	void ReloadLuaValue(int mode);
	obj *GetObject(int n);
	obj *GetOptionObject(int n);
	int GetKeyState(int n);
private:
	TYPE type;
	int root;
};
