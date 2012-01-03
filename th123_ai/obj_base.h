
class ObjBase
{
public:
	enum{
		FFLAG_STAND,	FFLAG_CROUCHI,	FFLAG_AIR,	FFLAG_GUARD,
		FFLAG_CANCEL
	};
	enum AI_MODE {
		AI_MODE_1P = 1,
		AI_MODE_2P,
	};

	unsigned int base_addr;
	float x;
	float y;
	struct speed {
		float x;
		float y;
	} speed;
	char dir; // -1 or 1
	unsigned short action;
	unsigned short act_block;
	unsigned int frame;
	unsigned int img_no;
	unsigned short hp;
	unsigned short hit_stop;
	std::vector<Box> hitarea;
	std::vector<Box> attackarea;
	unsigned int fflags;
	unsigned int aflags;

	ObjBase() {
		initialize();
	}
	ObjBase(org::click3::Utility::SHARED_HANDLE h) {
		initialize();
		SetProcessHandle(h);
	}

	virtual void ReloadBaseAddr(AI_MODE mode) = 0;
	virtual void ReloadVal(AI_MODE mode) = 0;
	
	void SetProcessHandle(org::click3::Utility::SHARED_HANDLE ph);
	void Reload(AI_MODE mode);

	bool CheckFFlags(unsigned int n) const {
		return (fflags&n) > 0;
	}
	bool CheckAFlags(unsigned int n) const {
		return (aflags&n) > 0;
	}
protected:
	org::click3::Utility::SHARED_HANDLE ph;

	void initialize() {
		base_addr = 0;
	}
};
