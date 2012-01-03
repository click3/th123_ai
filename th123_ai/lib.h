
#pragma once

int atoi2(const char *str, int def);
float atof2(const char *str, float def);
void set_clipboard(char *s);
void to_lowstring(char *s);
void SetConsoleTitle2(char *str);
void change_icon(int id);
bool ReadProcessMemory(org::click3::Utility::SHARED_HANDLE ph, unsigned int base_addr, void *buffer, unsigned int buf_size, unsigned int *read_size = NULL);
bool ReadProcessMemory(org::click3::Utility::SHARED_HANDLE ph, unsigned int base_addr, int &buffer);
bool ReadProcessMemory(org::click3::Utility::SHARED_HANDLE ph, unsigned int base_addr, unsigned int &buffer);
bool ReadProcessMemory(org::click3::Utility::SHARED_HANDLE ph, unsigned int base_addr, long &buffer);
bool ReadProcessMemory(org::click3::Utility::SHARED_HANDLE ph, unsigned int base_addr, unsigned long &buffer);
bool ReadProcessMemory(org::click3::Utility::SHARED_HANDLE ph, unsigned int base_addr, short &buffer);
bool ReadProcessMemory(org::click3::Utility::SHARED_HANDLE ph, unsigned int base_addr, unsigned short &buffer);
bool ReadProcessMemory(org::click3::Utility::SHARED_HANDLE ph, unsigned int base_addr, char &buffer);
bool ReadProcessMemory(org::click3::Utility::SHARED_HANDLE ph, unsigned int base_addr, unsigned char &buffer);
bool ReadProcessMemory(org::click3::Utility::SHARED_HANDLE ph, unsigned int base_addr, float &buffer);


class ini : boost::noncopyable {
public:
	typedef std::pair<std::wstring, std::wstring> Param;
	struct Section {
		std::wstring name;
		std::vector<Param> params;
	};
	ini() { }
	~ini() { }

	const char *GetValue(const char *name) const {
		const std::wstring * const value = GetValueImpl(name);
		if(value == NULL) {
			return NULL;
		}
		static std::string result; // TODO remove
		if(!org::click3::Utility::WCharToSJIS(result, *value)) {
			return NULL;
		}
		return result.c_str();
	}
	int GetInt(const char *name, int def = 0) const {
		return atoi2(GetValue(name), def);
	}
	float GetFloat(const char *name, float def = 0.0) const {
		return atof2(GetValue(name), def);
	}

	bool LoadFile(const boost::filesystem::path &path);

	void Add(const char *section, const char *name, const char *value);
protected:
	std::vector<Section> sectionList;

	const std::wstring *GetValueImpl(const char *name) const {
		const Param *param = Search(name);
		if(param == NULL) {
			return NULL;
		}
		return &param->second;
	}
	
	const Param *Search(const char *name) const;
	Param *Search(const char *name);

	Param *get_list(const char *name) {
		return Search(name);
	}
	const Param *get_list(const char *name) const {
		return Search(name);
	}
};

class th_ini : public ini
{
public:
	enum TYPE{
		TYPE_NONE,
		TYPE_TH105_SWR,
		TYPE_TH123_SOKU
	};

	th_ini() : type(TYPE_NONE) { }
	th_ini(TYPE a) : type(a) { }
	~th_ini() { }

	bool SetType(TYPE a) {
		if(a!=TYPE_NONE && a!=TYPE_TH105_SWR && a!=TYPE_TH123_SOKU) {
			printf("Error ini::SetType param error( type = %d)\n",a);
			BOOST_ASSERT(false);
			return false;
		}
		type = a;
		return true;
	}
	TYPE GetType(void) const {
		return type;
	}

	const char *GetValue(const char *name) const {
		return ini::GetValue(GetTypeName(name, type));
	}
protected:
	TYPE type;

	static const char *GetTypeName(const char *name, TYPE type) {
		if(strncmp(name, "ADDR_", 4) != 0) {
			return name;
		}
		static char temp[256];
		if(type == TYPE_TH105_SWR) {
			::strcpy(temp, "SWR_");
		} else if(type == TYPE_TH123_SOKU) {
			::strcpy(temp, "SWRS_");
		} else {
			printf("Error ini::GetValue type error(name = %s, type = %d)\n", name, type);
			BOOST_ASSERT(false);
			return "";
		}
		::strcat(temp, name);
		return temp;
	}

};

extern th_ini g_ini;

const char *ini_value(const char *name);
int ini_int(const char *name);
int ini_int2(const char *name, int def);
float ini_float(const char *name);
bool load_ini(const boost::filesystem::path &path);
void ini_add(const char *section,const char *name,const char *value);
void create_ini(void);
