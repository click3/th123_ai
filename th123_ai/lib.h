
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

	const std::wstring *GetValue(const std::wstring &key) const {
		return GetValueImpl(key);
	}
	int GetInt(const std::wstring &key, int def = 0) const {
		const std::wstring *ptr = GetValue(key);
		if(ptr == NULL) {
			return def;
		}
		std::string value;
		const bool result = org::click3::Utility::WCharToSJIS(value, *ptr);
		BOOST_ASSERT(result);
		return atoi2(value.c_str(), def);
	}
	float GetFloat(const std::wstring &key, float def = 0.0) const {
		const std::wstring *ptr = GetValue(key);
		if(ptr == NULL) {
			return def;
		}
		std::string value;
		const bool result = org::click3::Utility::WCharToSJIS(value, *ptr);
		BOOST_ASSERT(result);
		return atof2(value.c_str(), def);
	}

	bool LoadFile(const boost::filesystem::path &path);

	void Add(const std::wstring &sectionName, const std::wstring &key, const std::wstring &value);
protected:
	std::vector<Section> sectionList;

	const std::wstring *GetValueImpl(const std::wstring &key) const {
		const Param *param = Search(key);
		if(param == NULL) {
			return NULL;
		}
		return &param->second;
	}
	
	const Param *Search(const std::wstring &key) const;
	Param *Search(const std::wstring &key);
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

	const std::wstring *GetValue(const std::wstring &key) const {
		return ini::GetValue(GetTypeName(key, type));
	}
protected:
	TYPE type;

	static const std::wstring GetTypeName(const std::wstring &key, TYPE type) {
		if(key.compare(0, 5, L"ADDR_", 5) != 0) {
			return key;
		}
		static std::wstring newKey;
		if(type == TYPE_TH105_SWR) {
			newKey.assign(L"SWR_");
		} else if(type == TYPE_TH123_SOKU) {
			newKey.assign(L"SWRS_");
		} else {
			printf("Error ini::GetValue type error(key = %s, type = %d)\n", key, type);
			BOOST_ASSERT(false);
			return L"";
		}
		newKey.append(key);
		return newKey;
	}

};

extern th_ini g_ini;

const std::wstring *ini_value(const std::wstring &key);
int ini_int(const std::wstring &key);
int ini_int2(const std::wstring &key, int def);
float ini_float(const std::wstring &key);
bool load_ini(const boost::filesystem::path &path);
void ini_add(const std::wstring &sectionName, const std::wstring &key, const std::wstring &value);
void create_ini(void);