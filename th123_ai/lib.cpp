
#include "stdafx.h"

using namespace std;

namespace {

//IEEE754形式小数点演算だと仮定して、不正なfloatではないかチェックする
bool CheckFloat(float f) {
	const unsigned int * const to_int = reinterpret_cast<const unsigned int *>(&f);
	const unsigned int exponent = (*to_int >> 23)&0xFF;
	if(exponent > 254 || (*to_int != 0 && exponent < 108)) {
		return false;
	}
	return true;
}

} // anonymous

//デフォルト指定可能、16進数なども可能版atoi
int atoi2(const char *str,int def) {
	int i;
	char *s = NULL;

	if(str == NULL || !isdigit(str[0]))return def;
	i = strtol(str,&s,0);
	if(i == 0 && s != NULL && str[0]!='0')return def;

	return i;
}
//デフォルト指定可能版atof
float atof2(const char *str, float def) {
	float i;

	if(str == NULL || !isdigit(str[0]))return def;
	i = atof(str);
	if(i == 0 && str[0] != '0')return def;

	return i;
}

void set_clipboard(char *s) {
	HGLOBAL mem;
	char *str;

	mem = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, strlen(s)+1);
	str = static_cast<char*>(GlobalLock(mem));
	strcpy(str, s);
	GlobalUnlock(mem);

	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_TEXT,mem);
	CloseClipboard();
	return;
}

void to_lowstring(char *s) {
	while(*s != '\0') {
		*s = tolower(*s);
		s++;
	}
}

void SetConsoleTitle2(char *str) {
	static char *s = NULL;

	if(s != NULL) {
		if(strncmp(s,str,strlen(str)) == 0)return;
		free(s);
	}
	s = static_cast<char*>(malloc(strlen(str)+1+strlen(GetTH123AIVersionString())+3));
	strcpy(s,str);
	strcat(s," - ");
	strcat(s,GetTH123AIVersionString());
	SetConsoleTitle(s);
}

void change_icon(int id) {
	static int id_list[10] = {0};
	static long icon_list[10][2] = {0};
	int i;

	if(id_list[0] == 0) {
		memset(id_list,0,sizeof(int)*10);
	}
	i = 0;
	while(i < 10) {
		if(id_list[i] == id) {
			break;
		}
		i++;
	}
	if(i==10) {
		i = 0;
		while(id_list[i]!=0)i++;
		id_list[i] = id;
		icon_list[i][0] = (long)LoadImage(hInst,MAKEINTRESOURCE(id),IMAGE_ICON,16,16,LR_DEFAULTCOLOR);
		icon_list[i][1] = (long)LoadImage(hInst,MAKEINTRESOURCE(id),IMAGE_ICON,32,32,LR_DEFAULTCOLOR);
	}
	SendMessage(my_wh,WM_SETICON,0,icon_list[i][0]);
	SendMessage(my_wh,WM_SETICON,1,icon_list[i][1]);
}

bool ReadProcessMemory(org::click3::Utility::SHARED_HANDLE ph, unsigned int base_addr, void *buffer, unsigned int buf_size, unsigned int *read_size) {
	return ::ReadProcessMemory(ph.get(), reinterpret_cast<const void *>(base_addr), buffer, buf_size, reinterpret_cast<DWORD *>(read_size)) != FALSE;
}
bool ReadProcessMemory(org::click3::Utility::SHARED_HANDLE ph, unsigned int base_addr, int &buffer) {
	return ReadProcessMemory(ph, base_addr, &buffer, 4, NULL);
}
bool ReadProcessMemory(org::click3::Utility::SHARED_HANDLE ph, unsigned int base_addr, unsigned int &buffer) {
	return ReadProcessMemory(ph, base_addr, &buffer, 4, NULL);
}
bool ReadProcessMemory(org::click3::Utility::SHARED_HANDLE ph, unsigned int base_addr, long &buffer) {
	return ReadProcessMemory(ph, base_addr, &buffer, 4, NULL);
}
bool ReadProcessMemory(org::click3::Utility::SHARED_HANDLE ph, unsigned int base_addr, unsigned long &buffer) {
	return ReadProcessMemory(ph, base_addr, &buffer, 4, NULL);
}
bool ReadProcessMemory(org::click3::Utility::SHARED_HANDLE ph, unsigned int base_addr, short &buffer) {
	return ReadProcessMemory(ph, base_addr, &buffer, 2, NULL);
}
bool ReadProcessMemory(org::click3::Utility::SHARED_HANDLE ph, unsigned int base_addr, unsigned short &buffer) {
	return ReadProcessMemory(ph, base_addr, &buffer, 2, NULL);
}
bool ReadProcessMemory(org::click3::Utility::SHARED_HANDLE ph, unsigned int base_addr, char &buffer) {
	return ReadProcessMemory(ph, base_addr, &buffer, 1, NULL);
}
bool ReadProcessMemory(org::click3::Utility::SHARED_HANDLE ph, unsigned int base_addr, unsigned char &buffer) {
	return ReadProcessMemory(ph, base_addr, &buffer, 1, NULL);
}
bool ReadProcessMemory(org::click3::Utility::SHARED_HANDLE ph, unsigned int base_addr, float &buffer) {
	float f;
	const bool result = ReadProcessMemory(ph, base_addr, &f, 4, NULL);
	if(!result || !CheckFloat(f)) {
		return false;
	}
	buffer = f;
	return true;
}

th_ini g_ini;

BOOST_FUSION_ADAPT_STRUCT (
	ini::Section,
	(std::wstring, name)
	(std::vector<ini::Param>, params)
)

bool ini::LoadFile(const boost::filesystem::path &path) {
	if(!boost::filesystem::exists(path) || !boost::filesystem::is_regular_file(path)) {
		return false;
	}
	const unsigned int size = static_cast<unsigned int>(boost::filesystem::file_size(path));
	if(size == 0) {
		return true;
	}
	boost::filesystem::wifstream wifs(path);
	if(!wifs.is_open()) {
		return false;
	}
	wifs.imbue(std::locale("japanese"));
	std::vector<wchar_t> data(size);
	wifs.read(&data[0], data.size());
	if(wifs.eof()) {
		const unsigned int pos = static_cast<unsigned int>(wifs.gcount());
		data.resize(pos);
		wifs.clear(wifs.rdstate() & ~boost::filesystem::wifstream::eofbit & ~boost::filesystem::wifstream::failbit);
	}
	if(!wifs.good()) {
		return false;
	}
	wifs.close();

	typedef std::vector<wchar_t>::iterator Iterator;
	namespace qi = boost::spirit::qi;
	const qi::rule<Iterator, std::wstring()> quotedString = '"' >> *(qi::standard_wide::char_ - L'"' - qi::eol) >> '"';
	const qi::rule<Iterator, std::wstring()> emptyString = qi::lit("");
	const qi::rule<Iterator, std::wstring()> key = quotedString | +(qi::standard_wide::char_ - '=' - qi::space);
	const qi::rule<Iterator, std::wstring()> value = quotedString | +(qi::standard_wide::char_ - qi::eol) | emptyString;
	const qi::rule<Iterator, Param()> paramLine = key >> qi::omit[*qi::blank] >> '=' >> qi::omit[*qi::blank] >> value >> *qi::eol;
	const qi::rule<Iterator> comment = (qi::lit("//") | qi::lit(";") | qi::lit("#")) >> *(qi::standard_wide::char_ - qi::eol);
	const qi::rule<Iterator> commentLine = comment >> *qi::eol;
	const qi::rule<Iterator, std::wstring()> sectionName = '[' >> (quotedString | *(qi::standard_wide::char_ - ']')) >> ']';
	const qi::rule<Iterator, std::wstring()> sectionNameLine = sectionName >> *qi::eol;
	const qi::rule<Iterator, Section()> section = *commentLine >> sectionNameLine >> *(*commentLine >> paramLine);
	const qi::rule<Iterator, std::vector<Section>()> ini = *section;
	std::vector<wchar_t>::iterator it = data.begin();
	std::vector<Section> sectionList;
	const bool result = qi::parse(it, data.end(), ini, sectionList);
	if(!result || it != data.end()) {
		std::wcout << std::wstring(it, data.end());
		return false;
	}
	BOOST_FOREACH(const Section &section, sectionList) {
		BOOST_FOREACH(const Param &param, section.params) {
			this->Add(section.name, param.first, param.second);
		}
	}
	return true;
}
struct IniFindSection {
	const std::wstring &sectionName;
	IniFindSection(const std::wstring &sectionName) : sectionName(sectionName) { }
	bool operator()(ini::Section &section) const {
		return section.name == sectionName;
	}
};
struct IniFindParam {
	const std::wstring &key;
	IniFindParam(const std::wstring &key) : key(key) { }
	bool operator()(const std::pair<std::wstring, std::wstring> &param) const {
		return param.first == key;
	}
};
#include <WinError.h>
void ini::Add(const std::wstring &sectionName, const std::wstring &key, const std::wstring &value){
	std::vector<Section>::iterator section = std::find_if(sectionList.begin(), sectionList.end(), IniFindSection(sectionName));
	if(section == sectionList.end()) {
		Section newSection;
		newSection.name = sectionName;
		sectionList.push_back(newSection);
		section = sectionList.end() - 1;
	}
	const std::vector<std::pair<std::wstring, std::wstring> >::iterator param = std::find_if(section->params.begin(), section->params.end(), IniFindParam(key));
	if(param == section->params.end()) {
		section->params.push_back(std::make_pair(key, value));
		return;
	}
	param->second = value;
}
const ini::Param *ini::Search(const std::wstring &key) const {
	BOOST_FOREACH(const Section &section, sectionList) {
		const std::vector<std::pair<std::wstring, std::wstring> >::const_iterator param = std::find_if(section.params.begin(), section.params.end(), IniFindParam(key));
		if(param != section.params.end()) {
			return &*param;
		}
	}
	return NULL;
}
ini::Param *ini::Search(const std::wstring &key) {
	const Param * const param = const_cast<const ini * const>(this)->Search(key);
	return const_cast<Param * const>(param);
}


const std::wstring *ini_value(const std::wstring &key) {
	return g_ini.GetValue(key);
}
int ini_int(const std::wstring &key) {
	return g_ini.GetInt(key);
}
int ini_int2(const std::wstring &key, int def) {
	return g_ini.GetInt(key, def);
}
float ini_float(const std::wstring &key) {
	return g_ini.GetFloat(key);
}
bool load_ini(const boost::filesystem::path &path) {
	return g_ini.LoadFile(path);
}
void ini_add(const std::wstring &sectionName, const std::wstring &key, const std::wstring &value) {
	g_ini.Add(sectionName, key, value);
}
void create_ini(void) {
}
