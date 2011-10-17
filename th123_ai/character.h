
#define SPECIALDATA_MAX	28

class obj_base
{
public:
	enum{
		FFLAG_STAND,	FFLAG_CROUCHI,	FFLAG_AIR,	FFLAG_GUARD,
		FFLAG_CANCEL
	};

	int base_addr;
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
	obj_base(HANDLE h) {
		initialize();
		SetProcessHandle(h);
	}

	virtual void ReloadBaseAddr(int) = 0;
	virtual void ReloadVal(int) = NULL;

	HANDLE SetProcessHandle(HANDLE handle);
	void Reload(int param);

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
		MY, ENEMY
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
