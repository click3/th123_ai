
#define SPECIALDATA_MAX	28

class Character : public ObjBase {
public:
	enum TYPE {
		MY, ENEMY
	};
	Character(TYPE a) : ObjBase() {
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
	std::vector<Obj> object;
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
	unsigned int SetRootAddress(unsigned int addr);
	int GetSpecialData(unsigned int n) const;
	virtual void ReloadBaseAddr(AI_MODE mode);
	virtual void ReloadVal(AI_MODE mode);
	void ReloadObject(AI_MODE mode);
	void ReloadLuaValue(AI_MODE mode);
	const Obj &GetObject(unsigned int index) const;
	const Obj *GetOptionObject(unsigned int index) const;
	int GetKeyState(unsigned int n) const;
private:
	TYPE type;
	unsigned int character_manager_addr;
};
