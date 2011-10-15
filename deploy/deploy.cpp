#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>

#include <windows.h>

#include <boost/version.hpp>
#include <boost/static_assert.hpp>
#include <boost/assert.hpp>
#include <boost/type_traits/remove_pointer.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/program_options.hpp>

#include <org/click3/utility.h>

#include "../common/common.h"

const char * const list[] = {
	"th123_ai.exe",	"document.txt",	"th123_ai.ini",
	"History.txt",	"motion.txt",		"Readme.txt",
	"skill.txt",		"spell.txt",
	"SWRSAddr.ini",	"SwrAddr.ini",	"card.txt",
	""};
const char * const src[] = {
	"main.cpp",		"script.cpp",		"keyboard.cpp",
	"lib.cpp",		"network.cpp",	"consts.h",
	"address.cpp",	"scriptBase.cpp",	"scriptLua.cpp",
	"main.h",		"script.h",		"keyboard.h",
	"lib.h",		"network.h",
	"address.h",		"dik_list.h",		"version.h",
	"resource.h",		"scriptLua.h",	"scriptBase.h",
	"makefile",		"include",
	"lua_src",		"api.ai",
	"th105_ai.rc",	"th105_ai.ico",	"th105_ai2.ico",
	"th105_ai2.ico",
	""};

char *enumFile(char *str,int i){
	static char s[256];
	HANDLE hSearch;
	WIN32_FIND_DATA fd;

	hSearch = FindFirstFile(str, &fd );
	if( hSearch == INVALID_HANDLE_VALUE )return NULL;
	while(i > 0){
		if(!FindNextFile( hSearch, &fd )){
			if( GetLastError() == ERROR_NO_MORE_FILES ){
				return NULL;
			}
		}
		i--;
	}
	FindClose( hSearch );
	strcpy(s,fd.cFileName);
	return s;
}

int CopyFiles(char *src,char *dest,int flag){
	int i;
	char *s,a[256],fn[256],fn2[256];

	if(!CopyFile(src,dest,flag)){
		CreateDirectory(dest,NULL);
		sprintf(a,"%s\\*",src);
		i = 0;
		while(1){
			s = enumFile(a,i);
			if(s==NULL)break;
			if(s[0] != '.'){
				sprintf(fn,"%s\\%s",src,s);
				sprintf(fn2,"%s\\%s",dest,s);
				CopyFile(fn,fn2,flag);
			}
			i++;
		}
	}
	return TRUE;
}

namespace {

typedef boost::shared_ptr<boost::remove_pointer<HKEY>::type> SHARED_REG_KEY;

void MyRegCloseKey(HKEY key) {
	const LSTATUS result = ::RegCloseKey(key);
	BOOST_ASSERT(result == ERROR_SUCCESS);
}

SHARED_REG_KEY MyReadRegOpen(HKEY key, const wchar_t *sub_key) {
	HKEY reg_key;
	const LSTATUS result = ::RegOpenKeyExW(key, sub_key, 0, KEY_READ, &reg_key);
	BOOST_ASSERT(result == ERROR_SUCCESS);
	return SHARED_REG_KEY(reg_key);
}

}

bool GetRegString(std::string &out, HKEY key, const wchar_t *sub_key, const wchar_t *value_name) {
	const SHARED_REG_KEY reg = MyReadRegOpen(key, sub_key);
	DWORD type;
	DWORD buffer_size;
	const LSTATUS get_size_result = ::RegQueryValueExW(reg.get(), value_name, NULL, &type, NULL, &buffer_size);
	if(get_size_result != ERROR_SUCCESS || type != REG_SZ) {
		return false;
	}
	out.resize(buffer_size);
	const LSTATUS result = ::RegQueryValueExW(reg.get(), value_name, NULL, NULL, reinterpret_cast<BYTE *>(&out.front()), &buffer_size);
	if(result != ERROR_SUCCESS) {
		return false;
	}
	return true;
}

std::string history_path;
bool is_appli;
bool is_source;

bool ParseArgs(unsigned int argc, const char * const *argv) {
	boost::program_options::options_description opt("オプション");
	opt.add_options()
		("help,h",											"ヘルプを表示")
		("version,v",	boost::program_options::value<std::string>(&history_path),		"versionを取得するhistory.txtのパス")
		("appli,a",	boost::program_options::value<bool>(&is_appli)->default_value(true),	"アプリパッケージを作成")
		("source,s",	boost::program_options::value<bool>(&is_source)->default_value(true),	"ソースパッケージを作成");
	boost::program_options::variables_map vm;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, opt), vm);
	boost::program_options::notify(vm);
	if(vm.count("help") > 0 || vm.count("version") == 0) {
		std::cout << opt << std::endl;
		return false;
	}
	return true;
}

int main(unsigned int argc, const char * const *argv) {
	if(!::ParseArgs(argc, argv)) {
		return 1;
	}
	char str[256];
	char s[2048],temp[256],fn[256];
	int i;

	org::click3::Utility::SetAppDir();

	std::string version;
	if(!common::GetVersion(version, history_path.c_str())) {
		return 1;
	}

	std::string app_dir;
	if(!org::click3::Utility::GetAppDir(app_dir)) {
		return 1;
	}
	SPRINTF(temp, "%s\\%s", app_dir.c_str(), version.c_str());
	if(SetCurrentDirectoryA(temp)){
		printf("既に該当するVersionが存在します\n");
		return 1;
	}
	::CreateDirectoryA(temp,NULL);
	std::string zip_app;
	if(!::GetRegString(zip_app, HKEY_CURRENT_USER, L"Software\\HoeHoe\\Lhaplus", L"InstallPath")) {
		return 1;
	}
	::strcpy(s, zip_app.c_str());

	i = 0;
	while(list[i][0] != '\0'){
		sprintf(fn,"%s\\%s",app_dir.c_str(), list[i]);
		sprintf(temp," \"%s\"",fn);
		strcat(s,temp);
		sprintf(temp,"%s\\%s\\%s",app_dir.c_str(), version.c_str(), list[i]);
		CopyFiles(fn,temp,TRUE);
		i++;
	}
	system(s);
	i = 0;
	while(src[i][0] != '\0'){
		sprintf(fn,"%s\\%s", app_dir.c_str(), src[i]);
		sprintf(temp,"%s\\%s\\%s", app_dir.c_str(), version.c_str(), src[i]);
		CopyFiles(fn,temp,TRUE);
		i++;
	}
	return 0;
}
