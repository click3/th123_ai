
#include "stdafx.h"

//#define DEBUG
#if !defined(DEBUG) && defined(_DEBUG)
#define DEBUG
#endif
#define DEBUG2

org::click3::Utility::SHARED_HANDLE ph;
HINSTANCE hInst =NULL;
HWND wh=0;
HWND my_wh;
char exe_path[256] = "";
char aimanager_signature[14]="";
int operation_char = -1;

int weather_delay = 300;
int get_delay = 0;

int seen,mode,weather,weather2;
int obj_dis=0,obj_dis2=0,battle_time;
short weather_time;
char stage_num,bgm_num;
int dis_y,dis2;

Character my_data(Character::MY);
Character enemy_data(Character::ENEMY);

#pragma	comment(lib,"advapi32.lib")
#pragma	comment(lib,"psapi.lib")
#pragma	comment(lib,"gdi32.lib")
#pragma	comment(lib,"user32.lib")


//不正対策
int __argc__;
char **__argv__;
int main2(int argc,char *argv[]);

void mj(void) {
	exit(main2(__argc__,__argv__));
}

long WINAPI main_jump(EXCEPTION_POINTERS *pep) {
	pep->ContextRecord->Eip = (int)mj;
	return EXCEPTION_CONTINUE_EXECUTION;
}
#ifdef DEBUG
int main2(int argc,char *argv[]) {
#else
int main(int argc,char *argv[]) {
#endif
#ifdef __BORLANDC__
	__asm{
		push EAX
		mov EAX,DWORD PTR FS:[0]
		mov EAX,DWORD PTR DS:[EAX]
		mov DWORD PTR FS:[0],EAX
		pop EAX
	}
	if(IsDebuggerPresent())exit(0);
#endif
	__argc__ = argc;
	__argv__ = argv;
	AddVectoredExceptionHandler(NULL, main_jump);
	RaiseException(0xC00000FD,0/*EXCEPTION_CONTINUABLE*/,0,NULL);
	return 0;
}


long WINAPI exep(EXCEPTION_POINTERS *pep) {
	PEXCEPTION_RECORD per = pep->ExceptionRecord;
	PCONTEXT pctx = pep->ContextRecord;
	const HANDLE raw_ph = ::OpenProcess(PROCESS_VM_READ, FALSE, GetCurrentProcessId());
	org::click3::Utility::SHARED_HANDLE ph = org::click3::Utility::ToSharedPtr(raw_ph);
	char name[10][4] ={
		"EAX",	"ECX",	"EDX",	"EBX",	"ESP",
		"EBP",	"ESI",	"EDI",	"EIP",	"EFL"};
	int list[10];
	int i,a;
	char s[2048]="",str[256];
	static time_t t = 0;
	static int flag = 0;
	static int ftol = 0;

	if(ftol==0 && per->ExceptionCode == 0xC0000090) {
		ftol = 1;
		pctx->FloatSave.StatusWord = 0x0020;
		pctx->FloatSave.ControlWord = 0x027F;
		//memset(pctx->FloatSave.RegisterArea,0,80);
		//printf("ErrorCounter\n");
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	if(t==0)t=time(NULL);

	i = 0;
	list[i++] = pctx->Eax;
	list[i++] = pctx->Ecx;
	list[i++] = pctx->Edx;
	list[i++] = pctx->Ebx;
	list[i++] = pctx->Esp;
	list[i++] = pctx->Ebp;
	list[i++] = pctx->Esi;
	list[i++] = pctx->Edi;
	list[i++] = pctx->Eip;
	list[i] = pctx->EFlags;

	sprintf(&s[strlen(s)],"Exception!!!\n--Status--\n" );
	sprintf(&s[strlen(s)],"Version: %s\n",GetTH123AIVersionString());
	sprintf(&s[strlen(s)],"CloseVersion: %d\n",GetTH123AIVersion());
	sprintf(&s[strlen(s)],"Code: %08X\n", per->ExceptionCode );
	sprintf(&s[strlen(s)],"Address: %08X\n", per->ExceptionAddress );
	if(!ph) {
		sprintf(&s[strlen(s)],"fixdata: failed(%d)\n", ::GetLastError());
	}

	sprintf(&s[strlen(s)],"--Register--\n" );
	i = 0;
	while(i < 10) {
		sprintf(&s[strlen(s)],"%s %08x",name[i],list[i]);
		if(ph && ReadProcessMemory(ph, list[i], a)) {
			sprintf(&s[strlen(s)],"=>%08x",a);
		}
		sprintf(&s[strlen(s)],"\n");
		i++;
	}

	sprintf(&s[strlen(s)],"--Stack--\n");
	i = 0;
	while(i < 20) {
		if(!ReadProcessMemory(ph, list[4] + i * 4, a)) {
			break;
		}
		sprintf(&s[strlen(s)],"%08x %08x\n",list[4]+i*4,a);
		i++;
	}
	sprintf(&s[strlen(s)],"\n\n");
	if(flag == 0) {
		printf("%s",s);
		printf("以上の内容が作者あてにバグ報告として送信可能です。\n");
		while(flag == 0) {
			printf("送信しますか？Y/N ");
			fgets(str,256,stdin);
			a = toupper(str[0]);
			if(a == 'Y') {
				flag = 1;
				printf("送信しました\n");
			} else if(a == 'N') {
				flag = -1;
				printf("送信を中止しました\n");
			}
		}
	}
	if(flag == 1) {
		ErrorPost(URL_AI_ERROR_CGI,s);
	}
	return EXCEPTION_EXECUTE_HANDLER;
}


/*
処理に直接関係する定義群ここから
*/
#define is_th105_active()	(wh==GetForegroundWindow())

void th105_active(void) {
	SetActiveWindow2(wh);
}
void SetActiveWindow2(HWND hWnd) {
	DWORD pid;
	HANDLE p;
	HWND w;
	HMODULE hm;
	LPTHREAD_START_ROUTINE proc;

	w = GetForegroundWindow();
	if(w != NULL) {
		GetWindowThreadProcessId(w,&pid);
		if(pid != 0) {
			p = OpenProcess(PROCESS_ALL_ACCESS,FALSE,pid);
			if(p!=NULL) {
				hm = GetModuleHandle("user32.dll");
				proc = (LPTHREAD_START_ROUTINE)GetProcAddress(hm,"SetForegroundWindow");
				CreateRemoteThread(p,NULL,0,proc,hWnd,0,NULL);
				CloseHandle(p);
			}
		}
	}
	//ダメ押し
	ShowWindow(hWnd,SW_NORMAL);
	SetForegroundWindow(hWnd);

	Sleep(50);
}
//flag==TRUEで強制最前窓、FALSEでフルスクリーンでないときのみ最前窓
void MyWndActive(int flag) {
	int a;
	if(flag==FALSE) {
		if(ini_int2(L"MyWindowActive", 1)==0)return;
		ReadProcessMemory(ph, ADDR_ACTIVEFLAG, a);
		if(a==0) {
			return;
		}
	}
	SetActiveWindow2(my_wh);
}


void change_ini(char *fn,char *name,char *param) {
	FILE *rfp,*wfp;
	char temp_file[] = "th105_ai.tmp0123";
	char temp[256];
	int flag = FALSE;
	char dir[256];

	GetCurrentDirectory(256,dir);
	const bool set_dir_result = org::click3::Utility::SetAppDir();
	BOOST_ASSERT(set_dir_result);
	rfp = fopen(fn,"r");
	if((rfp==NULL) || (wfp=fopen(temp_file,"w"))==NULL) {
		SetCurrentDirectoryA(dir);
		return;
	}
	while(fgets(temp,256,rfp)!=NULL) {
		if(flag == FALSE && strncmp(temp,name,strlen(name))==0) {
			fprintf(wfp,"%s=\"%s\"\n",name,param);
			flag = TRUE;
		} else {
			fwrite(temp,1,strlen(temp),wfp);
		}
	}
	fclose(rfp);
	if(flag==FALSE) {
		fprintf(wfp,"%s=\"%s\"\n",name,param);
	}
	fclose(wfp);
	remove(fn);
	rename(temp_file,fn);
	SetCurrentDirectoryA(dir);
}

const wchar_t *GetTouhouPath(const std::wstring &regkey, const std::wstring &name, const std::wstring &exe_name, const std::wstring &iniKey) {
	static wchar_t str[256] = L"";
	int i;
	int ret;

	if(ini_value(iniKey) != NULL) {
		::wcscpy(str, ini_value(iniKey)->c_str());
		if(boost::filesystem::exists(str) && boost::filesystem::is_regular_file(str)) {
			return str;
		}
		str[0] = '\0';
	}
	
	HKEY hKey;
	if(ERROR_SUCCESS == RegOpenKeyExW(HKEY_LOCAL_MACHINE, regkey.c_str(), 0, KEY_READ, &hKey)) {
		wchar_t temp[256];
		DWORD size = 256;
		if(ERROR_SUCCESS == ::RegQueryValueExW(hKey, L"Inno Setup: App Path", NULL, NULL, reinterpret_cast<LPBYTE>(temp), &size)) {
			::RegCloseKey(hKey);
			::_snwprintf(str, _countof(str), L"%s\\%s", temp, exe_name.c_str());
			if(boost::filesystem::exists(str) && boost::filesystem::is_regular_file(str)) {
				return str;
			}
			str[0] = '\0';
		}
		::RegCloseKey(hKey);
	}

	std::string dir_str;
	org::click3::Utility::GetAppDir(dir_str);
	const boost::filesystem::path dir(dir_str);
	OPENFILENAMEW ofn;
	::memset(&ofn, 0, sizeof(OPENFILENAMEW));
	ofn.lStructSize = sizeof(OPENFILENAMEW);
	wchar_t temp[256];
	::_snwprintf(temp, _countof(temp), L"%s実行ファイル(%s)\n%s\nAll files(*.*)\n*.*\n\n", name.c_str(), exe_name.c_str(), exe_name.c_str());
	for(unsigned int i = ::wcslen(temp)-1; i > 0; i--) {
		if(temp[i]=='\n') {
			temp[i] = '\0';
		}
	}
	ofn.lpstrFilter = temp;
	ofn.lpstrFile = str;
	ofn.nMaxFile = 256;
	ofn.lpstrInitialDir = dir.wstring().c_str();
	ofn.Flags = OFN_FILEMUSTEXIST;
	ofn.lpstrTitle = L"ファイルを開く";
	ofn.lpstrDefExt = L"exe";

	changeLoadLibraryExW(1);
	ret = GetOpenFileNameW(&ofn);
	changeLoadLibraryExW(0);
	if(ret == 0) {
		return NULL;
	}
	return str;
}
const wchar_t *GetTh105Path(void) {
	return GetTouhouPath(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{F9942587-59C1-43CC-8B6A-A5DB09CBA735}_is1", L"緋想天", L"th105.exe", L"StartupTH105");
}
const wchar_t *GetTh123Path(void) {
	return GetTouhouPath(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{8E5CFA2B-8CC5-4C8D-88CB-C4A1D4AD9790}_is1", L"非想天則", L"th123.exe", L"StartupTH123");
}

org::click3::Utility::SHARED_HANDLE WindowName2Handle(const std::wstring &name, const std::wstring &text) {
	org::click3::Utility::SHARED_HANDLE ret;
	wh = ::FindWindowW(name.c_str(), text.c_str());
	if(wh == NULL) {
		return ret;
	}
	DWORD pid = 0;
	::GetWindowThreadProcessId(wh,&pid);
	if(pid == 0) {
		return ret;
	}
	HANDLE raw_ph = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if(raw_ph == NULL) {
		return ret;
	}
	return org::click3::Utility::ToSharedPtr(raw_ph);
}

org::click3::Utility::SHARED_HANDLE GetSWRHandle(void) {
	const std::wstring *className = ini_value(L"SWR_WINDOW_CLASS");
	const std::wstring *title = ini_value(L"SWR_WINDOW_TEXT");
	BOOST_ASSERT(className != NULL);
	BOOST_ASSERT(title != NULL);
	return WindowName2Handle(*className, *title);
}

org::click3::Utility::SHARED_HANDLE GetSOKUHandle(void) {
	const std::wstring *className = ini_value(L"SWRS_WINDOW_CLASS");
	const std::wstring *title = ini_value(L"SWRS_WINDOW_TEXT");
	BOOST_ASSERT(className != NULL);
	BOOST_ASSERT(title != NULL);
	return WindowName2Handle(*className, *title);
}

void AutoStartTouhouExe(bool swr, bool soku) {
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	const wchar_t *s = NULL;

	changeLoadLibraryExW(1);
	if(soku) {
		s = GetTh123Path();
	}
	if(s==NULL && swr) {
		s = GetTh105Path();
		soku = false;
	}
	changeLoadLibraryExW(0);
	if(s == NULL) {
		::printf("自動起動に失敗しました\n");
	} else {
		if(soku) {
			::wprintf(L"非想天則を自動起動します(%s)\n", s);
		} else if(swr) {
			::wprintf(L"緋想天を自動起動します(%s)\n", s);
		}
		::memset(&si,0,sizeof(si));
		::memset(&pi,0,sizeof(pi));
		si.cb = sizeof(si);
		changeLoadLibraryExW(1);
		if(::CreateProcessW(s, NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi)) {
			::CloseHandle(pi.hProcess);
			::CloseHandle(pi.hThread);
			//change_ini("th105_ai.ini","StartupTH105",s);
		} else {
			char message[2048];
			::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, ::GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), message, sizeof(message), NULL);
			::printf("CreateProcessError:%s\n", message);
		}
		changeLoadLibraryExW(0);
	}
} 

org::click3::Utility::SHARED_HANDLE GetProcessHandle(void) {
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	
	static bool is_first_run = true;
	//終了時を検出する
	if(!is_first_run && ini_int2(L"AppDownExit", 0) == 0) {
		::exit(0);
	}
	addr.reset();

	//デバッグ権限の獲得
	changeLoadLibraryExW(1);
	OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	LookupPrivilegeValue(NULL, SE_DEBUG_NAME,&tkp.Privileges[0].Luid);
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0,(PTOKEN_PRIVILEGES)NULL, 0);
	changeLoadLibraryExW(0);

	SetConsoleTitle("緋想天もしくは非想天則を検索しています");
	while(1) {
		changeLoadLibraryExW(1);
		ph = GetSOKUHandle();
		changeLoadLibraryExW(0);
		if(ph) {
			::AIModeSWRS();
			g_ini.SetType(th_ini::TYPE_TH123_SOKU);
			break;
		}
		changeLoadLibraryExW(1);
		ph = GetSWRHandle();
		changeLoadLibraryExW(0);
		if(ph) {
			::AIModeSWR();
			g_ini.SetType(th_ini::TYPE_TH105_SWR);
			break;
		}
		reload_check();
		Sleep(50);
		if(is_first_run || GetKeyState(VK_F10) < 0) {
			is_first_run = false;
			while(GetKeyState(VK_F10)<0)Sleep(100);
			AutoStartTouhouExe(true,true);
		} else if(GetKeyState(VK_F9) < 0) {
			while(GetKeyState(VK_F9)<0)Sleep(100);
			AutoStartTouhouExe(true,false);
		}
	}
	is_first_run = false;
	my_data.SetProcessHandle(ph);
	enemy_data.SetProcessHandle(ph);
	return ph;
}

int EnumProcess(char *name) {
	int i = 0;
	// スナップショットハンドル
	HANDLE hSnapShot;
	// プロセスエントリ構造体(第1要素をサイズで初期化)
	PROCESSENTRY32 p32 = {sizeof(PROCESSENTRY32)};

	// スナップショットの作成
	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
	if(INVALID_HANDLE_VALUE != hSnapShot) {
		// スナップショットの先頭からエントリの取得
		Process32First(hSnapShot, &p32);
		do{
			if(strcmp(p32.szExeFile,name)==0)i++;
		}while(Process32Next(hSnapShot, &p32));
	}
	return i;
}

void load_inis(void) {
	const char * const iniList[] = {
		"SwrAddr.ini",
		"SWRSAddr.ini",
		"th105_ai.ini",
		"th123_ai.ini",
		"dev.ini"
	};
	BOOST_FOREACH(const char * const name, iniList) {
		load_ini(name);
	}
}

void change_player(void) {
	static HANDLE h = NULL;
	int n = ini_int2(L"Player", 1);

	if(h) {
		ReleaseMutex(h);
		CloseHandle(h);
		h = NULL;
	}
	if(n!=2) {
		h = CreateMutex(NULL,FALSE,"TH105AI_1P");
	} else {
		h = CreateMutex(NULL,FALSE,"TH105AI_2P");
	}
	if(WAIT_OBJECT_0 != WaitForSingleObject(h,0)) {
		if(n!=2) {
			ini_add(L"default", L"Player", L"2");
			n = 2;
		} else {
			ini_add(L"default", L"Player", L"1");
			n = 1;
		}
		CloseHandle(h);
		h = NULL;
		if(n!=2) {
			h = CreateMutex(NULL,FALSE,"TH105AI_1P");
		} else {
			h = CreateMutex(NULL,FALSE,"TH105AI_2P");
		}
		if(WAIT_OBJECT_0 != WaitForSingleObject(h,0)) {
			CloseHandle(h);
			h = NULL;
			exit(0);
		}
	}
}

void init(void) {
	const bool set_dir_result = org::click3::Utility::SetAppDir();
	BOOST_ASSERT(set_dir_result);
	create_ini();
	g_ini.Add(L"default",L"SWR_WINDOW_CLASS",	L"th105_106");
	g_ini.Add(L"default",L"SWR_WINDOW_TEXT",	L"東方緋想天 Ver1.06");
	g_ini.Add(L"default",L"SWRS_WINDOW_CLASS",	L"th123_110a");//
	g_ini.Add(L"default",L"SWRS_WINDOW_TEXT",	L"東方非想天則 ～ 超弩級ギニョルの謎を追え Ver1.10a");//
	g_ini.Add(L"default",L"StartupAI",		L"dialog");
	g_ini.Add(L"default",L"Player",			L"1");
	g_ini.Add(L"default",L"AppDownExit",		L"0");
	addr.initialize();
	load_inis();
	change_player();
}

void title_message(void) {
	unsigned int root;
	char str[256] = "",s[256];

	switch(seen) {
		case 0xFF:
		case 0x00:
			SetConsoleTitle2("starting");
			break;
		case 0x01:
			SetConsoleTitle2("OP Movie");
			break;
		case 0x02:
			SetConsoleTitle2("Start Menu");
			break;
		case 0x03:
			SetConsoleTitle2("Character Select");
			break;
		case 0x06:
		case 0x0a:
		case 0x0b:
		case 0x0c:
			SetConsoleTitle2("少女祈祷中");
			break;
		case 0x0f:
			SetConsoleTitle2("少女観戦中");
			break;
		case 0x05:
			sprintf(str,"Battle 1P:%d 2P:%d",my_data.hp,enemy_data.hp);
			SetConsoleTitle2(str);
			break;
		case 0x0d:
		case 0x0e:
			ReadProcessMemory(ph, ADDR_PNETOBJECT, root);
			ReadProcessMemory(ph, root+ADDR_LPROFOFS, my_data.prof, sizeof(my_data.prof));
			ReadProcessMemory(ph, root+ADDR_RPROFOFS, enemy_data.prof, sizeof(enemy_data.prof));
			sprintf(str, "Net %s(%s:%d) VS %s(%s:%d)", my_data.prof, ::ID2Char(my_data.char_id), my_data.win_count, enemy_data.prof, ::ID2Char(enemy_data.char_id), enemy_data.win_count);
			SetConsoleTitle2(str);
			break;
		case 0x08:
		case 0x09:
			ReadProcessMemory(ph, ADDR_PNETOBJECT, root);
			ReadProcessMemory(ph, root+ADDR_LPROFOFS, my_data.prof, sizeof(my_data.prof));
			ReadProcessMemory(ph, root+ADDR_RPROFOFS, enemy_data.prof, sizeof(enemy_data.prof));
			strcpy(s,"Net %s VS %s ");
			if(seen == 0x08)strcat(s,"(Server)");
			if(seen == 0x09)strcat(s,"(Client)");
			sprintf(str,s,my_data.prof,enemy_data.prof);
			SetConsoleTitle2(str);
			break;
		case 0x10:
			SetConsoleTitle2("Scenario Select");
			break;
		case 0x14:
			SetConsoleTitle2("Scenario Ending");
			break;
		default:
			sprintf(s,"Unknown Seen(0x%02x)",seen);
			SetConsoleTitle2(s);
			break;
	}
}

int is_CardUse(void) {
	int p,temp,temp2,temp3;
	short a;

	if(weather==11) {
		return FALSE;
	}

	p = GetPlayerAddr(0);
	ReadProcessMemory(ph, p+ADDR_CARDCOUNT2OFS, temp);
	if(temp<1) {
		return FALSE;
	}

	ReadProcessMemory(ph, p+ADDR_SELECTCARDOFS, temp);
	ReadProcessMemory(ph, p+ADDR_HANDCARDBASEOFS, temp2);
	ReadProcessMemory(ph, temp2+temp*4, temp3);
	ReadProcessMemory(ph, temp3+2, a);
	if(weather==2 && a>1)a--;
	if(a <= my_data.gauge/500 && a>=1)return TRUE;
	else return FALSE;
}

void set_data_delay(int delay) {
	get_delay = delay;
	engine->setScriptValue("data_delay",get_delay);
}
void set_weather_delay(int n) {
	weather_delay = n;
	engine->setScriptValue("weather_delay",weather_delay);
}


void check_th105(void) {
	HMODULE mh;
	DWORD m_num;

	while(TRUE) {
		seen = 0;
		while(0 == ReadProcessMemory(ph, ADDR_SCENEID, &seen, 1)) {
			ph = GetProcessHandle();
			if(!::EnumProcessModules(ph.get(), &mh, sizeof(HMODULE), &m_num)) {
				//printf("Error:EnumProcessModules\n");
			} else {
				::GetModuleFileNameEx(ph.get(), mh, exe_path, 256);
			}
		}
		if(seen!=0x08 && seen!=0x09 && seen!=0x0E && seen!=0x0D) {
			break;
		}
		title_message();
		Sleep(50);
	}
}

void get_th105param(void) {
	unsigned int root,temp=0;

	check_th105();
	switch(seen) {
		case 0x03:
		case 0x06:
		case 0x0a:
		case 0x0b:
		case 0x0c:
			if(mode != seen) {
				change_icon(DOLL_ICON);
				key_reset();
				keyboard.Clear();
			}
			break;
		case 0x08:
		case 0x09:/*
			ReadProcessMemory(ph,(void *)ADDR_PNETOBJECT,&root,4,NULL);
			ReadProcessMemory(ph,(void *)(root+ADDR_LPROFOFS),my_data.prof,20,NULL);
			ReadProcessMemory(ph,(void *)(root+ADDR_RPROFOFS),enemy_data.prof,20,NULL);*/
			break;
		case 0x05:
		case 0x0d:
		case 0x0e:
			if(mode != seen) {
				change_icon(GEAR_ICON);
				search_key(static_cast<CHAR_ID>(operation_char));
				if(engine->scriptName[0] != '\0') {
					mode = seen;
					reload();
				}
			}
			ReadProcessMemory(ph, ADDR_STAGENUMBER, stage_num);
			ReadProcessMemory(ph, ADDR_BGMNUMBER, bgm_num);
			ReadProcessMemory(ph, ADDR_TIMECOUNT, battle_time);
			ReadProcessMemory(ph, ADDR_WEATHER, weather);
			ReadProcessMemory(ph, ADDR_DISPLAYWEATHER, weather2);
			ReadProcessMemory(ph, ADDR_WEATHERCOUNTER, weather_time);

			ReadProcessMemory(ph, ADDR_PBATTLEMGR, root);
			my_data.SetRootAddress(root);
			enemy_data.SetRootAddress(root);
			const Character::AI_MODE ai_mode = (::ini_int2(L"Player", 1) == 1 ? Character::AI_MODE_1P : Character::AI_MODE_2P);
			my_data.Reload(ai_mode);
			enemy_data.Reload(ai_mode);
			is_bullethit();
			if(my_data.x<enemy_data.x || (my_data.x==enemy_data.x && my_data.dir==1)) {
				muki = ACT_RIGHT;
				gyaku = ACT_LEFT;
				dfront = ACT_DRIGHT;
				dback = ACT_DLEFT;
				ufront = ACT_URIGHT;
				uback = ACT_ULEFT;
				is_dir_front = (my_data.dir==1);
			} else if(my_data.x>enemy_data.x  || (my_data.x==enemy_data.x && my_data.dir==-1)) {
				muki = ACT_LEFT;
				gyaku = ACT_RIGHT;
				dfront = ACT_DLEFT;
				dback = ACT_DRIGHT;
				ufront = ACT_ULEFT;
				uback = ACT_URIGHT;
				is_dir_front = (my_data.dir==-1);
			}
			dis = abs(my_data.x-enemy_data.x);
			dis_y = abs(my_data.y-enemy_data.y);
			dis2 = sqrt(abs((double)dis*dis+dis_y*dis_y));
			break;
	}
	check_lastkey();
	title_message();
	mode = seen;
}
/*
処理に直接関係する定義群ここまで
*/
/*
メイン
*/

void reload_check(void) {
	int a;
/*
	char b;
	HMODULE hm;
	static BOOL (WINAPI *ReadProcessMemory)(HANDLE,LPCVOID,LPVOID,DWORD,LPDWORD) = NULL;

	//不正対策
	if(ReadProcessMemory==NULL) {
		a = 1;
		hm = GetModuleHandle(&"\0Kernel32.dll"[a]);
		ReadProcessMemory = GetProcAddress(hm,&"\0ReadProcessMemory"[a]);
	}
	a = 0x006ECE7C*2;
	ReadProcessMemory(ph,(void*)(a/2),&b,1,NULL);
	if(b==0x0E || b==0x0D || b==0x08 || b==0x09) {
		__asm int 3
	}
*/

	if(GetAsyncKeyState(VK_F12)<0) {
		a = VK_F12;
	} else if(GetAsyncKeyState(VK_F11)<0) {
		a = VK_F11;
	} else {
		return;
	}

	while(GetAsyncKeyState(a)<0)Sleep(100);
	printf("reload script\n");
	if(a==VK_F12)reload();
	else reload2();
}
void yield(void) {
	static unsigned int t1 = 0;
	unsigned int t2;
	static short *act = NULL;
	static short *act_block = NULL;
	static int *frame = NULL;
	static int delay = 0;
	static int bt=0;
	int a;
	keyboard.SetKeyDelay(engine->getScriptValue("key_delay"));
	get_delay = engine->getScriptValue("data_delay");
	weather_delay = engine->getScriptValue("weather_delay");

	if(!ph) {
		ph = GetProcessHandle();
	}

	while(wh && !is_th105_active()) {
		reload_check();
		check_th105();
		Sleep(50);
#ifndef DEBUG
		__asm{
			push EAX
			mov EAX,DWORD PTR FS:[0x18]
			mov EAX,DWORD PTR DS:[EAX+0x30]
			movzx EAX,BYTE PTR DS:[EAX+2]
			mov a,EAX
			pop EAX
		}
		if(a) {
			a = (int)malloc(1024)+512;
			__asm{
				mov ESP,a
				mov EAX,0
				push EAX
				push EAX
				jmp Sleep
			}
		}
#endif
	}

	while(bt==battle_time) {
		Sleep(1);
		get_th105param();
		if(timeGetTime()-t1>320) {
			key_reset();
			keyboard.Clear();
			break;
		}
	}
	keyboard.ProcessEvent();
	bt = battle_time;

	if(get_delay != delay) {
		if(act != NULL) {
			free(act);
			free(act_block);
			free(frame);
			act = NULL;
			act_block = NULL;
			frame = NULL;
		}
		if(get_delay == 0) {
			act = NULL;
			act_block = NULL;
			frame = NULL;
		} else {
			act = static_cast<short*>(malloc(sizeof(short)*get_delay));
			act_block = static_cast<short*>(malloc(sizeof(short)*get_delay));
			frame = static_cast<int*>(malloc(sizeof(int)*get_delay));
			memset(act, 0, sizeof(short)*get_delay);
			memset(act_block, 0, sizeof(short)*get_delay);
			memset(frame, 0, sizeof(int)*get_delay);
		}
		delay = get_delay;
	}

	while(1) {
		if(seen != 0x05) {
			reload_check();
			Sleep(50);
		} else {
			break;
		}
		get_th105param();
	}
	if(t1 == 0) {
		//Sleep(16);
		t1 = battle_time;//同期ズレし難くなる様に
		while(t1 == battle_time) {
			get_th105param();
		}
		t1 = timeGetTime();
	} else {
		t2 = t1+15-timeGetTime();
		if(t2 > 15)t2 = 15;
		Sleep(t2);
		t1 = timeGetTime();
	}
	reload_check();
	get_th105param();
	engine->setScriptValue("front",muki);
	engine->setScriptValue("back",gyaku);
	engine->setScriptValue("d_front",dfront);
	engine->setScriptValue("d_back",dback);
	engine->setScriptValue("u_front",ufront);
	engine->setScriptValue("u_back",uback);
	engine->setScriptValue("dis",dis);
	engine->setScriptValue("dis_x",dis);
	engine->setScriptValue("dis_y",dis_y);
	engine->setScriptValue("dis2",dis2);
	engine->setScriptValueBool("is_dir_front",is_dir_front);
	engine->setScriptValue("obj_dis",obj_dis);
	engine->setScriptValue("obj_dis2",obj_dis2);
	engine->setScriptValue("stage_number",stage_num);
	engine->setScriptValue("bgm_number",bgm_num);

	//engine->setScriptValue("key_delay",key_delay);
	//engine->setScriptValue("data_delay",get_delay);
	engine->setScriptValue("battle_time",battle_time);

	engine->setScriptValue("weather2",weather2);
	if(((::IsSWR() && weather2 == 14) || (!::IsSWR() && weather2==19)) && 1000-weather_delay<weather_time) {
		engine->setScriptValue("weather",weather2);
	} else {
		engine->setScriptValue("weather",weather);
	}
	//engine->setScriptValue("weather_delay",weather_delay);
	engine->setScriptValue("weather_time",weather_time);
	engine->setScriptValueBool("is_card_use",is_CardUse());

	engine->setScriptValue("my_frame",my_data.frame);
	engine->setScriptValue("my_act",my_data.action);
	engine->setScriptValue("my_act_block",my_data.act_block);
	if(delay == 0) {
		engine->setScriptValue("enemy_act",enemy_data.action);
		engine->setScriptValue("enemy_act_block",enemy_data.act_block);
		engine->setScriptValue("enemy_frame",enemy_data.frame);
	} else {
		engine->setScriptValue("enemy_act",act[0]);
		engine->setScriptValue("enemy_act_block",act_block[0]);
		engine->setScriptValue("enemy_frame",frame[0]);
		memmove(act,&act[1],sizeof(short)*(delay-1));
		memmove(act,&act_block[1],sizeof(short)*(delay-1));
		memmove(frame,&frame[1],sizeof(int)*(delay-1));
		act[delay-1] = enemy_data.action;
		act_block[delay-1] = enemy_data.act_block;
		frame[delay-1] = enemy_data.frame;
	}

	engine->setScriptValueBool("is_th105", ::IsSWR());
	engine->setScriptValueBool("is_th123", !::IsSWR());
	engine->loadable = true;
}

int GetPlayerAddr(int player) {
	int p;
	if(player == MY) {
		p = my_data.base_addr;
	} else {
		p = enemy_data.base_addr;
	}
	return p;
}

char GetSkillLv(int player,int n) {
	if(n < 0 || n > 14 || (player!=MY && player!=ENEMY))return -1;
	if(player==MY) {
		return my_data.skill[n];
	} else {
		return enemy_data.skill[n];
	}
}

short GetCardId(int player,int n) {
	if(weather==11 || (player!=MY && player!=ENEMY) || n<0 || n>5)return -1;
	if(player==MY) {
		return my_data.card[n].id;
	} else {
		return enemy_data.card[n].id;
	}
}
int GetCardCost(int player,int n) {
	if(weather==11 || (player!=MY && player!=ENEMY) || n<0 || n>5)return -1;
	if(player==MY) {
		return my_data.card[n].cost;
	} else {
		return enemy_data.card[n].cost;
	}
}
int GetCardCost2(int player,int n) {
	int a;

	a = GetCardCost(player,n);
	if(a==-1) {
		return -1;
	}
	if(weather==2 && a>1)a--;
	return a;
}

short *GetDeckList(int player) {
	static short deck[256];
	int c;
	FILE *fp;
	char fn[256],s[256],path[256];
	int i;
	unsigned char n;

	if(player==0) {
		c = my_data.char_id;
		ReadProcessMemory(ph, ADDR_LPROFNAME, s, 20);
	} else {
		c = enemy_data.char_id;
		ReadProcessMemory(ph, ADDR_RPROFNAME, s, 20);
	}

	strcpy(fn,exe_path);
	i = strlen(fn);
	while(i>0 && fn[i]!='/' && fn[i]!='\\')i--;
	fn[i] = '\0';
	sprintf(path,"%s/Profile/%s.dat",fn,s);
	fp = fopen(path,"r");
	if(fp==NULL) {
		i = *(int *)s;
		ReadProcessMemory(ph, i, s, 256);
		sprintf(path,"%s/Profile/%s.dat",fn,s);
		fp = fopen(path,"r");
		if(fp==NULL)return NULL;
	}
	fseek(fp,106,SEEK_SET);
	i = 0;
	while(i <= c) {
		fread(&n,1,1,fp);
		if(i==c) {
			fread(deck,2,n,fp);
			deck[n] = -1;
			break;
		}
		fseek(fp,n*2,SEEK_CUR);
		i++;
	}
	fclose(fp);
	return deck;
}


void is_bullethit(void) {
	obj_dis = 10000;
	obj_dis2 = 10000;
	BOOST_FOREACH(const Obj &obj, enemy_data.object) {
		const unsigned int dis2 = static_cast<unsigned int>(::abs(my_data.x - obj.x));
		obj_dis2 = (std::min)(obj_dis2, (int)dis2);

		BOOST_FOREACH(const Box &box, obj.attackarea) {
			if(box.left < my_data.x && box.right > my_data.x) {
				obj_dis = 1;
			} else {
				const unsigned int dis1 = static_cast<unsigned int>((std::min)(::abs(box.left - my_data.x), ::abs(box.right - my_data.x)));
				obj_dis = (std::min)(obj_dis, (int)dis1);
			}
		}
	}
}

Box *get_hitarea(int player,int n) {
	if((player!=MY && player!=ENEMY) || n<0 || (player==MY && n>=my_data.hitarea.size()) || (player==ENEMY && n>=enemy_data.hitarea.size())) {
		return NULL;
	}
	if(player==MY) {
		return &my_data.hitarea[n];
	} else {
		return &enemy_data.hitarea[n];
	}
}
Box *get_attackarea(int player,int n) {
	if((player!=MY && player!=ENEMY) || n<0 || (player==MY && n>=my_data.attackarea.size()) || (player==ENEMY && n>=enemy_data.attackarea.size())) {
		return NULL;
	}
	if(player==MY) {
		return &my_data.attackarea[n];
	} else {
		return &enemy_data.attackarea[n];
	}
}

int get_correction(int player,int flag) {
	int a;
	if(player==0) {
		a = my_data.correction;
	} else {
		a = enemy_data.correction;
	}
	return a&flag;
}

int GetSpecialData(int player,int n) {
	int data;
	if((player!=MY && player!=ENEMY) || n<0 || n>SPECIALDATA_MAX) {
		return -1;
	}
	//緋想緋緋想だけ逆
	if(n==9) {
		if(player==MY) {
			player = ENEMY;
		} else {
			player = MY;
		}
	}
	if(player==MY) {
		data = my_data.sp_data[n];
	} else {
		data = enemy_data.sp_data[n];
	}
	return data;
}

void changeDbgBreakPoint(void) {
	HMODULE hm;
	DWORD old;
	int paddr,a=1;
#ifdef DEBUG2
	return;
#endif

	hm = GetModuleHandle(&"\0ntdll.dll"[a]);
	paddr = (int)GetProcAddress(hm,&"\0DbgBreakPoint"[a]);
	if(*(unsigned char *)paddr != 0xCC) {
//		printf("Error\n");//機械語が想定しているものと違うため中断
		return;
	}
	if(VirtualProtect((void *)hm,(int)paddr-(int)hm+1,PAGE_EXECUTE_READWRITE,&old)) {//勝手に範囲を決めてくれるのでアバウドでよい
		*(unsigned char *)paddr = 0xC3;
		VirtualProtect((void *)hm,paddr-(int)hm+1,old,&old);//変更前の権限に戻す
	}
}
void changeLoadLibraryExW(int flag) {
	HMODULE hm;
	DWORD old;
	static unsigned char code[15];
	int paddr,a=1;
#ifdef DEBUG2
	return;
#endif
	hm = GetModuleHandle("Kernel32.dll");
	paddr = (int)GetProcAddress(hm,&"\0LoadLibraryExW"[a]);
	if(flag == 0) {
		code[0] = 0x6A;//code[0]~code[6]まで上書きで消してしまう機械語の代わり
		code[1] = 0x34;
		code[2] = 0x68;
		*(int *)&code[3] = 0x7C80E0E8;
		code[7] = 0xE8;
	} else {
		code[0] = 0xb8;
		*(int*)&code[1] = 0x00000000;
		code[5] = 0xc2;
		*(short*)&code[6] = 0x000c;
	}
	if(*(int *)code != *(int *)paddr || *(int *)&code[4] != *(int *)(paddr+4)) {
		//printf("Error\n");//機械語が想定しているものと違うため中断
		return;
	}
	if(VirtualProtect((void *)hm,(int)paddr-(int)hm+7,PAGE_EXECUTE_READWRITE,&old)) {//勝手に範囲を決めてくれるのでアバウドでよい
		if(flag == 0) {
			((unsigned char*)paddr)[0] = 0xb8;
			*(unsigned int*)&((char*)paddr)[1] = 0x00000000;
			((unsigned char*)paddr)[5] = 0xc2;
			*(unsigned short*)&((char*)paddr)[6] = 0x000c;
		} else {
			((unsigned char*)paddr)[0] = 0x6A;//code[0]~code[6]まで上書きで消してしまう機械語の代わり
			((unsigned char*)paddr)[1] = 0x34;
			((unsigned char*)paddr)[2] = 0x68;
			*(unsigned int *)&((char*)paddr)[3] = 0x7C80E0E8;
			((unsigned char*)paddr)[7] = 0xE8;
		}
		VirtualProtect((void *)hm,paddr-(int)hm+5,old,&old);//変更前の権限に戻す
	}
/*
	if(LoadLibraryA("user32.dll") == NULL) {//成功していれば必ず存在するdll相手でも失敗するはず
		printf("OK\n");
	} else {
		printf("NG\n");
	}
*/
}

#ifdef DEBUG
int main(int argc,char *_argv[]) {
#else
int main2(int argc,char *_argv[]) {
#endif
	char s[256];
	hInst = GetModuleHandle(NULL);

	SetUnhandledExceptionFilter(exep);
	my_wh = GetForegroundWindow();
	init();
	timeBeginPeriod(1);
	engine = scriptLua::getCurrentInstance();
	engine->init();
	changeDbgBreakPoint();
	changeLoadLibraryExW(0);

	std::locale::global(std::locale("japanese"));
	char *argv[64];
	{
		int i = 0;
		while(i < argc) {
			argv[i] = _argv[i];
			i++;
		}
		i = 0;
		while(i < argc) {
			if(argv[i][0]=='-') {
				if(strcmp(argv[i],"-signature")==0) {
					strcpy(aimanager_signature,argv[i+1]);
					memmove(&argv[i],&argv[i+2],sizeof(char*)*(argc-i-2));
					argc -= 2;
				} else if(strcmp(argv[i],"-char")==0) {
					int n = atoi2(argv[i+1],-1);
					if(n==-1) {
						printf("オプション -char の使用方法が間違っています。\n");
					} else {
						operation_char = n;
					}
					memmove(&argv[i],&argv[i+2],sizeof(char*)*(argc-i-2));
					argc -= 2;
				} else {
					printf("無効なオプション %s を無視します。\n",argv[i]);
					memmove(&argv[i],&argv[i+1],sizeof(char*)*(argc-i-1));
					argc--;
				}
				i--;
			}
			i++;
		}
	}

	if(argc == 2) {
		OpenAI(argv[1]);
	} else if(ini_value(L"StartupAI") != NULL) {
		std::string value;
		const bool result = org::click3::Utility::WCharToSJIS(value, *ini_value(L"StartupAI"));
		BOOST_ASSERT(result);
		::strcpy(s, value.c_str());
		to_lowstring(s);
		OpenAI(s);
	} else {
		OpenAI(NULL);
	}
	timeEndPeriod(1);
	return 0;
}
