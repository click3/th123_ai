
class Obj : public ObjBase
{
public:
	Obj(org::click3::Utility::SHARED_HANDLE ph, unsigned int addr) : ObjBase(ph) {
		base_addr = addr;
	}

	void ReloadBaseAddr(AI_MODE) { }

	void ReloadVal(AI_MODE) { }
};
