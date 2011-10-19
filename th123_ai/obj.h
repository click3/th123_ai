
class obj : public ObjBase
{
public:
	obj(org::click3::Utility::SHARED_HANDLE ph, unsigned int addr) : ObjBase(ph) {
		base_addr = addr;
	}

	void ReloadBaseAddr(AI_MODE) { }

	void ReloadVal(AI_MODE) { }
};
