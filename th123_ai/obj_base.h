
class ObjBase
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

	ObjBase() {
		initialize();
	}
	ObjBase(org::click3::Utility::SHARED_HANDLE h) {
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
