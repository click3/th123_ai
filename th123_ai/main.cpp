
#include "stdafx.h"

//#define DEBUG
#define DEBUG2

HANDLE ph=NULL;
HINSTANCE hInst =NULL;
HWND wh=0;
HWND my_wh;
char exe_path[256] = "";
char aimanager_signature[14]="";
int operation_char = -1;

bool is_soku = false;
bool is_swr = false;


int weather_delay = 300;
int get_delay = 0;

char on[10] = {0,0,0,0,0,0,0,0,0,0};
char get_key_stat(int n){
	BOOST_ASSERT(n >= 0);
	BOOST_ASSERT(n <= 9);
	return on[n];
}

int seen,mode,weather,weather2;
int obj_dis=0,obj_dis2=0,battle_time;
short weather_time;
char stage_num,bgm_num,last_on_key;

player my_data(player::MY);
player enemy_data(player::ENEMY);

#pragma	comment(lib,"advapi32.lib")
#pragma	comment(lib,"psapi.lib")
#pragma	comment(lib,"gdi32.lib")
#pragma	comment(lib,"user32.lib")


//不正対策
int __argc__;
char **__argv__;
int main2(int argc,char *argv[]);

void mj(void){
	exit(main2(__argc__,__argv__));
}

long WINAPI main_jump(EXCEPTION_POINTERS *pep){
	pep->ContextRecord->Eip = (int)mj;
	return EXCEPTION_CONTINUE_EXECUTION;
}
#ifdef DEBUG
int main2(int argc,char *argv[]){
#else
int main(int argc,char *argv[]){
#endif
	int a;
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
	SetUnhandledExceptionFilter(main_jump);
	RaiseException(0xC00000FD,0/*EXCEPTION_CONTINUABLE*/,0,NULL);
	return 0;
}


long WINAPI exep(EXCEPTION_POINTERS *pep){
	PEXCEPTION_RECORD per = pep->ExceptionRecord;
	PCONTEXT pctx = pep->ContextRecord;
	HANDLE ph = OpenProcess(PROCESS_VM_READ,FALSE,GetCurrentProcessId());
	char name[10][4] ={
		"EAX",	"ECX",	"EDX",	"EBX",	"ESP",
		"EBP",	"ESI",	"EDI",	"EIP",	"EFL"};
	int list[10];
	int i,a;
	char s[2048]="",str[256];
	static int t = 0;
	static int flag = 0;
	static int ftol = 0;

	if(ftol==0 && per->ExceptionCode == 0xC0000090){
		ftol = 1;
		pctx->FloatSave.StatusWord = 0x0020;
		pctx->FloatSave.ControlWord = 0x027F;
		//memset(pctx->FloatSave.RegisterArea,0,80);
		if(ph!=NULL)CloseHandle(ph);
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
	if(ph==0){
		sprintf(&s[strlen(s)],"fixdata: failed(%d)\n",GetLastError());
	}

	sprintf(&s[strlen(s)],"--Register--\n" );
	i = 0;
	while(i < 10){
		sprintf(&s[strlen(s)],"%s %08x",name[i],list[i]);
		if(ph!=0 && ReadProcessMemory(ph,(void *)list[i],&a,4,NULL)){
			sprintf(&s[strlen(s)],"=>%08x",a);
		}
		sprintf(&s[strlen(s)],"\n");
		i++;
	}

	sprintf(&s[strlen(s)],"--Stack--\n");
	i = 0;
	while(i < 20){
		if(!ReadProcessMemory(ph,(void *)(list[4]+i*4),&a,4,NULL)){
			break;
		}
		sprintf(&s[strlen(s)],"%08x %08x\n",list[4]+i*4,a);
		i++;
	}
	sprintf(&s[strlen(s)],"\n\n");
	if(flag == 0){
		printf("%s",s);
		printf("以上の内容が作者あてにバグ報告として送信可能です。\n");
		while(flag == 0){
			printf("送信しますか？Y/N ");
			fgets(str,256,stdin);
			a = toupper(str[0]);
			if(a == 'Y'){
				flag = 1;
				printf("送信しました\n");
			} else if(a == 'N'){
				flag = -1;
				printf("送信を中止しました\n");
			}
		}
	}
	if(flag == 1){
		ErrorPost(URL_AI_ERROR_CGI,s);
	}
	if(ph!=NULL)CloseHandle(ph);
	return EXCEPTION_EXECUTE_HANDLER;
}


/*
処理に直接関係する定義群ここから
*/
#define is_th105_active()	(wh==GetForegroundWindow())

void th105_active(void){
	SetActiveWindow2(wh);
}
void SetActiveWindow2(HWND hWnd){
	DWORD pid;
	HANDLE p;
	HWND w;
	HMODULE hm;
	LPTHREAD_START_ROUTINE proc;

	w = GetForegroundWindow();
	if(w != NULL){
		GetWindowThreadProcessId(w,&pid);
		if(pid != 0){
			p = OpenProcess(PROCESS_ALL_ACCESS,FALSE,pid);
			if(p!=NULL){
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
void MyWndActive(int flag){
	int a;
	if(flag==FALSE){
		if(ini_int2("MyWindowActive",1)==0)return;
		ReadProcessMemory(ph,(void*)ADDR_ACTIVEFLAG,&a,4,NULL);
		if(a==0){
			return;
		}
	}
	SetActiveWindow2(my_wh);
}


void change_ini(char *fn,char *name,char *param){
	FILE *rfp,*wfp;
	char temp_file[] = "th105_ai.tmp0123";
	char temp[256];
	int flag = FALSE;
	char dir[256];

	GetCurrentDirectory(256,dir);
	const bool set_dir_result = org::click3::Utility::SetAppDir();
	BOOST_ASSERT(set_dir_result);
	rfp = fopen(fn,"r");
	if((rfp==NULL) || (wfp=fopen(temp_file,"w"))==NULL){
		SetCurrentDirectoryA(dir);
		return;
	}
	while(fgets(temp,256,rfp)!=NULL){
		if(flag == FALSE && strncmp(temp,name,strlen(name))==0){
			fprintf(wfp,"%s=\"%s\"\n",name,param);
			flag = TRUE;
		} else {
			fwrite(temp,1,strlen(temp),wfp);
		}
	}
	fclose(rfp);
	if(flag==FALSE){
		fprintf(wfp,"%s=\"%s\"\n",name,param);
	}
	fclose(wfp);
	remove(fn);
	rename(temp_file,fn);
	SetCurrentDirectoryA(dir);
}

char *GetTouhouPath(const char *regkey, const char *name, const char *exe_name,char *ini_name){
	HKEY hKey;
	static char str[256]="";
	char temp[256];
	DWORD size = 256;
	OPENFILENAME ofn;
	FILE *fp;
	int i;
	int ret;

	if(ini_value(ini_name) != NULL){
		strcpy(str,ini_value("StartupTH105"));
		fp = fopen(str,"rb");
		if(fp!=NULL){
			fclose(fp);
			return str;
		}
		str[0] = '\0';
	}

	if(ERROR_SUCCESS==RegOpenKeyEx(HKEY_LOCAL_MACHINE,regkey,0,KEY_READ,&hKey)){
		if(ERROR_SUCCESS==RegQueryValueEx(hKey,"Inno Setup: App Path",NULL,NULL,reinterpret_cast<LPBYTE>(temp),&size)){
			RegCloseKey(hKey);
			snprintf(str, sizeof(str), "%s\\%s",temp,exe_name);
			fp = fopen(str,"rb");
			if(fp!=NULL){
				fclose(fp);
				return str;
			}
			str[0] = '\0';
		}
		RegCloseKey(hKey);
	}

	std::string dir_str;
	org::click3::Utility::GetAppDir(dir_str);
	const boost::filesystem::path dir(dir_str);
	memset(&ofn,0,sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	snprintf(temp, sizeof(temp), "%s実行ファイル(%s)\n%s\nAll files(*.*)\n*.*\n\n",name, exe_name, exe_name);
	i = strlen(temp)-1;
	while(i > 0){
		if(temp[i]=='\n'){
			temp[i] = '\0';
		}
		i--;
	}
	ofn.lpstrFilter = temp;
	ofn.lpstrFile = str;
	ofn.nMaxFile = 256;
	ofn.lpstrInitialDir = dir.string().c_str();
	ofn.Flags = OFN_FILEMUSTEXIST;
	ofn.lpstrTitle = "ファイルを開く";
	ofn.lpstrDefExt = "exe";

	changeLoadLibraryExW(1);
	ret = GetOpenFileName(&ofn);
	changeLoadLibraryExW(0);
	if(ret == 0){
		return NULL;
	}
	return str;
}
char *GetTh105Path(void){
	return GetTouhouPath("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{F9942587-59C1-43CC-8B6A-A5DB09CBA735}_is1", "緋想天", "th105.exe","StartupTH105");
}
char *GetTh123Path(void){
	return GetTouhouPath("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{8E5CFA2B-8CC5-4C8D-88CB-C4A1D4AD9790}_is1", "非想天則", "th123.exe","StartupTH123");
}

HANDLE WindowName2Handle(const char *name, const char *text){
	DWORD pid;
	HANDLE ph;

	wh = FindWindow(name, text);
	if(wh == NULL){
		return NULL;
	}
	pid = 0;
	GetWindowThreadProcessId(wh,&pid);
	if(pid == 0){
		return NULL;
	}
	ph = OpenProcess(PROCESS_ALL_ACCESS,FALSE,pid);
	if(ph == 0){
		return NULL;
	}
	return ph;
}

HANDLE GetSWRHandle(void){
	HANDLE ph;

	ph = WindowName2Handle(ini_value("SWR_WINDOW_CLASS"),ini_value("SWR_WINDOW_TEXT"));
	if(ph == NULL){
		return NULL;
	}
	return ph;
}

HANDLE GetSOKUHandle(void){
	HANDLE ph;

	ph = WindowName2Handle(ini_value("SWRS_WINDOW_CLASS"),ini_value("SWRS_WINDOW_TEXT"));
	if(ph == NULL){
		return NULL;
	}
	return ph;
}

void AutoStartTouhouExe(bool swr, bool soku){
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	char *s = NULL;

	changeLoadLibraryExW(1);
	if(soku){
		s = GetTh123Path();
	}
	if(s==NULL && swr){
		s = GetTh105Path();
		soku = false;
	}
	changeLoadLibraryExW(0);
	if(s == NULL){
		printf("自動起動に失敗しました\n");
	} else {
		if(soku){
			printf("非想天則を自動起動します(%s)\n",s);
		} else if(swr){
			printf("緋想天を自動起動します(%s)\n",s);
		}
		memset(&si,0,sizeof(si));
		memset(&pi,0,sizeof(pi));
		si.cb = sizeof(si);
		changeLoadLibraryExW(1);
		if(CreateProcess(s,NULL,NULL,NULL,FALSE,NORMAL_PRIORITY_CLASS,NULL,NULL,&si,&pi)){
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			//change_ini("th105_ai.ini","StartupTH105",s);
		} else {
			char message[2048];
			::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, ::GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), message, sizeof(message), NULL);
			printf("CreateProcessError:%s\n", message);
		}
		changeLoadLibraryExW(0);
	}
} 

HANDLE GetProcessHandle(void){
	static bool flag = true;
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;

	//終了時を検出する
	if(ini_int2("AppDownExit",0)==0 && (is_soku || is_swr)){
		exit(0);
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
	while(1){
		changeLoadLibraryExW(1);
		ph = GetSOKUHandle();
		changeLoadLibraryExW(0);
		if(ph!=NULL){
			is_soku = true;
			is_swr = false;
			g_ini.SetType(th_ini::TYPE_TH123_SOKU);
			break;
		}
		changeLoadLibraryExW(1);
		ph = GetSWRHandle();
		changeLoadLibraryExW(0);
		if(ph!=NULL){
			is_soku = false;
			is_swr = true;
			g_ini.SetType(th_ini::TYPE_TH105_SWR);
			break;
		}
		reload_check();
		Sleep(50);
		if(flag || GetKeyState(VK_F10)<0){
			flag = false;
			while(GetKeyState(VK_F10)<0)Sleep(100);
			AutoStartTouhouExe(true,true);
		} else if(GetKeyState(VK_F9)<0){
			while(GetKeyState(VK_F9)<0)Sleep(100);
			AutoStartTouhouExe(true,false);
		}
	}
	flag = false;
	my_data.SetProcessHandle(ph);
	enemy_data.SetProcessHandle(ph);
	return ph;
}

int EnumProcess(char *name){
	int i = 0;
	// スナップショットハンドル
	HANDLE hSnapShot;
	// プロセスエントリ構造体(第1要素をサイズで初期化)
	PROCESSENTRY32 p32 = {sizeof(PROCESSENTRY32)};

	// スナップショットの作成
	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
	if(INVALID_HANDLE_VALUE != hSnapShot){
		// スナップショットの先頭からエントリの取得
		Process32First(hSnapShot, &p32);
		do{
			if(strcmp(p32.szExeFile,name)==0)i++;
		}while(Process32Next(hSnapShot, &p32));
	}
	return i;
}

void load_inis(void){
	load_ini("SwrAddr.ini");
	load_ini("SWRSAddr.ini");
	load_ini("th105_ai.ini");
	load_ini("th123_ai.ini");
	load_ini("dev.ini");
}

void change_player(void){
	static HANDLE h = NULL;
	int n = ini_int2("Player",1);

	if(h){
		ReleaseMutex(h);
		CloseHandle(h);
		h = NULL;
	}
	if(n!=2){
		h = CreateMutex(NULL,FALSE,"TH105AI_1P");
	} else {
		h = CreateMutex(NULL,FALSE,"TH105AI_2P");
	}
	if(WAIT_OBJECT_0 != WaitForSingleObject(h,0)){
		if(n!=2){
			ini_add("default","Player","2");
			n = 2;
		} else {
			ini_add("default","Player","1");
			n = 1;
		}
		CloseHandle(h);
		h = NULL;
		if(n!=2){
			h = CreateMutex(NULL,FALSE,"TH105AI_1P");
		} else {
			h = CreateMutex(NULL,FALSE,"TH105AI_2P");
		}
		if(WAIT_OBJECT_0 != WaitForSingleObject(h,0)){
			CloseHandle(h);
			h = NULL;
			exit(0);
		}
	}
}

void init(void){
	const bool set_dir_result = org::click3::Utility::SetAppDir();
	BOOST_ASSERT(set_dir_result);
	create_ini();
	g_ini.Add("default","SWR_WINDOW_CLASS",		"th105_106");
	g_ini.Add("default","SWR_WINDOW_TEXT",		"東方緋想天 Ver1.06");
	g_ini.Add("default","SWRS_WINDOW_CLASS",	"th123_110");//
	g_ini.Add("default","SWRS_WINDOW_TEXT",		"東方非想天則 ～ 超弩級ギニョルの謎を追え Ver1.10");//
	g_ini.Add("default","StartupAI","dialog");
	g_ini.Add("default","Player","1");
	g_ini.Add("default","AppDownExit","0");
	addr.init();
	load_inis();
	change_player();
}

char *id2char(int id){
	static char db[][11] = {
		{"霊夢"},{"魔理沙"},{"咲夜"},{"アリス"},
		{"パチュリー"},{"妖夢"},{"レミリア"},{"幽々子"},
		{"紫"},{"萃香"},{"優曇華"},{"文"},
		{"小町"},{"衣玖"},{"天子"},{"早苗"},
		{"チルノ"},{"美鈴"},{"空"},{"諏訪子"}};
	if( (is_swr && id>15) || id>20){
		printf("Error:id2char 範囲外のID\n");
		BOOST_ASSERT(false);
		return db[0];
	}
	return db[id];
}
char *id2char2(int id){
	static char db[][11] = {
		{"reimu"},{"marisa"},{"sakuya"},{"alice"},
		{"patchouli"},{"youmu"},{"remilia"},{"yuyuko"},
		{"yukari"},{"suika"},{"udonge"},{"aya"},
		{"komachi"},{"iku"},{"tenshi"},{"sanae"},
		{"chirno"},{"meirin"},{"utsuho"},{"suwako"}};
	if( (is_swr && id>15) || id>20){
		printf("Error:id2char 範囲外のID\n");
		BOOST_ASSERT(false);
		return db[0];
	}
	return db[id];
}
void title_message(void){
	unsigned int root;
	char str[256] = "",s[256];

	switch(seen){
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
			ReadProcessMemory(ph,(void *)ADDR_PNETOBJECT,&root,4,NULL);
			ReadProcessMemory(ph,(void *)(root+ADDR_LPROFOFS),my_data.prof,20,NULL);
			ReadProcessMemory(ph,(void *)(root+ADDR_RPROFOFS),enemy_data.prof,20,NULL);
			sprintf(str,"Net %s(%s:%d) VS %s(%s:%d)",my_data.prof,id2char(my_data.char_id),my_data.win_count,enemy_data.prof,id2char(enemy_data.char_id),enemy_data.win_count);
			SetConsoleTitle2(str);
			break;
		case 0x08:
		case 0x09:
			ReadProcessMemory(ph,(void *)ADDR_PNETOBJECT,&root,4,NULL);
			ReadProcessMemory(ph,(void *)(root+ADDR_LPROFOFS),my_data.prof,20,NULL);
			ReadProcessMemory(ph,(void *)(root+ADDR_RPROFOFS),enemy_data.prof,20,NULL);
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

int key[10] = {DIK_UP,DIK_DOWN,DIK_LEFT,DIK_RIGHT,DIK_Z,DIK_X,DIK_C,DIK_A,DIK_S,DIK_D};
int muki,gyaku,dis,dfront,dback,ufront,uback,dis_y,dis2,is_dir_front;

void search_key(void){
	int map[2][10];
	int i;
	int p;

	p = 0;

	//ゲームパッドがいるなら逆側を使う
	ReadProcessMemory(ph,(void *)ADDR_1PKEYMAP,map[0],10*4,NULL);
	ReadProcessMemory(ph,(void *)ADDR_2PKEYMAP,map[1],10*4,NULL);
	if(map[0][0]==map[0][1] && map[0][1]==map[0][2] && map[0][2]==map[0][3]){
		p = 2;
	} else if(map[1][0]==map[1][1] && map[1][1]==map[1][2] && map[1][2]==map[1][3]){
		p = 1;
	}

	//AIManager指定キャラがいるならそちらを使う
	if(p==0){
		int char_id[2];
		ReadProcessMemory(ph,(void *)ADDR_LCHARID,&char_id[0],4,NULL);
		ReadProcessMemory(ph,(void *)ADDR_RCHARID,&char_id[1],4,NULL);
		if(char_id[0]==operation_char && char_id[1]!=operation_char){
			p = 1;
		} else if(char_id[1]==operation_char && char_id[0]!=operation_char){
			p = 2;
		}
	}

	//キーコンフィグがデフォルトに近いか、最後に入力されたキーをキーコンフィグに含んでいるなら逆側を使う
	if(p==0){
		const unsigned char def[10] = {DIK_UP,DIK_DOWN,DIK_LEFT,DIK_RIGHT,DIK_Z,DIK_X,DIK_C,DIK_A,DIK_S,DIK_D};
		int count[2];
		int i = 0;
		while(i < 2){
			int l = 0;
			count[i] = 0;
			while(l < 10){
				if(map[i][l] == last_on_key){
					count[i] += 10;
				}
				if(map[i][l] == def[l]){
					count[i]++;
				}
				l++;
			}
			i++;
		}
		if(count[0] < count[1]){
			i = 1;
		} else {
			i = 0;
		}
		if(count[i] > 5){
			if(i==0){
				p = 2;
			} else {
				p = 1;
			}
		}
	}

	//全ての条件で検出できなかったらINIの設定を使用する。
	if(p==0){
		p = ini_int2("Player",1);
		if(p!=2){
			p = 1;
		}
	}
	i = ini_int2("Player",1);
	if(i!=2){
		i = 1;
	}
	if(p!=i){
		if(p==1){
			ini_add("default","Player","1");
		} else {
			ini_add("default","Player","2");
		}
		change_player();
	}

	p = ini_int2("Player",1);
	if(p!=2){
		p = 1;
	}
	i = 0;
	while(i < 10){
		key[i] = map[p-1][i];
		if(key[i]==0x90){
			key[i] = MapVirtualKey(0xDE,0);
		} else if(key[i]==0x91){
			key[i] = MapVirtualKey(0xC0,0);
		} else if(key[i]==0x1A){
			key[i] = MapVirtualKey(0xDB,0);
		} else if(key[i]==0x1B){
			key[i] = MapVirtualKey(0xDD,0);
		} else if(key[i]==0x92){
			key[i] = MapVirtualKey(0xBA,0);
		} else if(key[i]==0x2B){
			key[i] = MapVirtualKey(0xE2,0);
		}
		i++;
	}
}

//ACT_DLEFT以降に対応していないため、key_onとkey_off以外から呼ばないこと
void on_check(int n){
	if(on[n]){
		keybd_event2(key[n],KEYEVENTF_KEYUP);
		on[n] = 0;
	}
	if(n == 2 && on[3]){
		keybd_event2(key[3],KEYEVENTF_KEYUP);
		on[3] = 0;
	} else if(n == 3 && on[2]){
		keybd_event2(key[2],KEYEVENTF_KEYUP);
		on[2] = 0;
	} else if(n == 0 && on[1]){
		keybd_event2(key[1],KEYEVENTF_KEYUP);
		on[1] = 0;
	} else if(n == 1 && on[0]){
		keybd_event2(key[0],KEYEVENTF_KEYUP);
		on[0] = 0;
	}
}

void key_on(int n){
	if(n >= ACT_DLEFT){
		switch(n){
			case ACT_DLEFT:
				key_on(ACT_DOWN);
				key_on(ACT_LEFT);
				break;
			case ACT_DRIGHT:
				key_on(ACT_DOWN);
				key_on(ACT_RIGHT);
				break;
			case ACT_ULEFT:
				key_on(ACT_UP);
				key_on(ACT_LEFT);
				break;
			case ACT_URIGHT:
				key_on(ACT_UP);
				key_on(ACT_RIGHT);
				break;
		}
	} else if(!on[n]){
		on_check(n);
		keybd_event2(key[n],0);
		on[n] = 1;
	}
}
void key_off(int n){
	if(n >= ACT_DLEFT){//実質的に十字キー全OFFと同義なので
		key_off(ACT_DOWN);
		key_off(ACT_LEFT);
	} else {
		on_check(n);
	}
}
void key_reset(void){
	int i;
	i = 0;
	while(i < 10){
		key_off(i);
		i++;
	}
}
int key_check(int n){
	int a,b;
	if(n >= ACT_DLEFT){
		switch(n){
			case ACT_DLEFT:
				a=key_check(ACT_DOWN);
				b=key_check(ACT_LEFT);
				break;
			case ACT_DRIGHT:
				a=key_check(ACT_DOWN);
				b=key_check(ACT_RIGHT);
				break;
			case ACT_ULEFT:
				a=key_check(ACT_UP);
				b=key_check(ACT_LEFT);
				break;
			case ACT_URIGHT:
				a=key_check(ACT_UP);
				b=key_check(ACT_RIGHT);
				break;
		}
		if(a&&b)return 1;
		if(a)return 2;
		if(b)return 3;
	} else if(on[n]){
		return 1;
	}
	return 0;
}
int is_CardUse(void){
	int p,temp,temp2,temp3;
	short a;

	if(weather==11){
		return FALSE;
	}

	p = GetPlayerAddr(0);
	ReadProcessMemory(ph,(void *)(p+ADDR_CARDCOUNT2OFS),&temp,4,NULL);
	if(temp<1){
		return FALSE;
	}

	ReadProcessMemory(ph,(void *)(p+ADDR_SELECTCARDOFS),&temp,4,NULL);
	ReadProcessMemory(ph,(void *)(p+ADDR_HANDCARDBASEOFS),&temp2,4,NULL);
	ReadProcessMemory(ph,(void *)(temp2+temp*4),&temp3,4,NULL);
	ReadProcessMemory(ph,(void *)(temp3+2),&a,2,NULL);
	if(weather==2 && a>1)a--;
	if(a <= my_data.gauge/500 && a>=1)return TRUE;
	else return FALSE;
}

void set_key_delay(int delay){
	key_delay = delay;
	engine->setScriptValue("key_delay",key_delay);
}

void set_data_delay(int delay){
	get_delay = delay;
	engine->setScriptValue("data_delay",get_delay);
}
void set_weather_delay(int n){
	weather_delay = n;
	engine->setScriptValue("weather_delay",weather_delay);
}


void check_th105(void){
	HMODULE mh;
	DWORD m_num;

	while(TRUE){
		while(0 == ReadProcessMemory(ph,(void *)ADDR_SCENEID,&seen,1,NULL)){
			CloseHandle(ph);
			ph = GetProcessHandle();
			if(!EnumProcessModules(ph,&mh,sizeof(HMODULE),&m_num)){
				//printf("Error:EnumProcessModules\n");
			} else {
				GetModuleFileNameEx(ph,mh,exe_path,256);
			}
		}
		if(seen!=0x08 && seen!=0x09 && seen!=0x0E && seen!=0x0D){
			break;
		}
		title_message();
		Sleep(50);
	}
}

void check_lastkey(){
	if(seen==0x05 || seen==0x0D || seen==0x0E){
		return;
	}
	int i=0;
	short t;
	while(i < 224){
		t = GetAsyncKeyState(i);
		if(t<0 && MapVirtualKey(i,0)!=0){
			last_on_key = MapVirtualKey(i,0);
			break;
		}
		i++;
	}
}

void get_th105param(void){
	unsigned int root,l,r,temp=0,temp2,temp3;
	float ftemp,ftemp2;
	int time,time2;
	char s[256];

	check_th105();
	switch(seen){
		case 0x03:
		case 0x06:
		case 0x0a:
		case 0x0b:
		case 0x0c:
			if(mode != seen){
				change_icon(DOLL_ICON);
				key_reset();
				kev_clear();
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
			if(mode != seen){
				change_icon(GEAR_ICON);
				search_key();
				if(engine->scriptName[0] != '\0'){
					mode = seen;
					reload();
				}
			}
			ReadProcessMemory(ph,(void *)ADDR_STAGENUMBER,&stage_num,1,NULL);
			ReadProcessMemory(ph,(void *)ADDR_BGMNUMBER,&bgm_num,1,NULL);
			ReadProcessMemory(ph,(void *)ADDR_TIMECOUNT,&battle_time,4,NULL);
			ReadProcessMemory(ph,(void *)ADDR_WEATHER,&weather,4,NULL);
			ReadProcessMemory(ph,(void *)ADDR_DISPLAYWEATHER,&weather2,4,NULL);
			ReadProcessMemory(ph,(void *)ADDR_WEATHERCOUNTER,&weather_time,2,NULL);

			ReadProcessMemory(ph,(void *)ADDR_PBATTLEMGR,&root,4,NULL);
			my_data.SetRootAddress(root);
			enemy_data.SetRootAddress(root);
			my_data.Reload(ini_int2("Player",1));
			enemy_data.Reload(ini_int2("Player",1));
			is_bullethit();
			if(my_data.x<enemy_data.x || (my_data.x==enemy_data.x && my_data.dir==1)){
				muki = ACT_RIGHT;
				gyaku = ACT_LEFT;
				dfront = ACT_DRIGHT;
				dback = ACT_DLEFT;
				ufront = ACT_URIGHT;
				uback = ACT_ULEFT;
				is_dir_front = (my_data.dir==1);
			} else if(my_data.x>enemy_data.x  || (my_data.x==enemy_data.x && my_data.dir==-1)){
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

void reload_check(void){
	int a;
	char b;
	HMODULE hm;
/*
	static BOOL (WINAPI *ReadProcessMemory)(HANDLE,LPCVOID,LPVOID,DWORD,LPDWORD) = NULL;

	//不正対策
	if(ReadProcessMemory==NULL){
		a = 1;
		hm = GetModuleHandle(&"\0Kernel32.dll"[a]);
		ReadProcessMemory = GetProcAddress(hm,&"\0ReadProcessMemory"[a]);
	}
	a = 0x006ECE7C*2;
	ReadProcessMemory(ph,(void*)(a/2),&b,1,NULL);
	if(b==0x0E || b==0x0D || b==0x08 || b==0x09){
		__asm int 3
	}
*/

	if(GetAsyncKeyState(VK_F12)<0){
		a = VK_F12;
	} else if(GetAsyncKeyState(VK_F11)<0){
		a = VK_F11;
	} else {
		return;
	}

	while(GetAsyncKeyState(a)<0)Sleep(100);
	printf("reload script\n");
	if(a==VK_F12)reload();
	else reload2();
}
void yield(void){
	static unsigned int t1 = 0;
	unsigned int t2;
	static short *act = NULL;
	static short *act_block = NULL;
	static int *frame = NULL;
	static int delay = 0;
	static int bt=0;
	int a;
	key_delay = engine->getScriptValue("key_delay");
	get_delay = engine->getScriptValue("data_delay");
	weather_delay = engine->getScriptValue("weather_delay");

	if(ph == NULL) {
		ph = GetProcessHandle();
	}

	while(wh && !is_th105_active()){
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
		if(a){
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

	while(bt==battle_time){
		Sleep(1);
		get_th105param();
		if(timeGetTime()-t1>320){
			key_reset();
			kev_clear();
			break;
		}
	}
	kev_call();
	key_frame++;
	bt = battle_time;

	if(get_delay != delay){
		if(act != NULL){
			free(act);
			free(act_block);
			free(frame);
			act = NULL;
			act_block = NULL;
			frame = NULL;
		}
		if(get_delay == 0){
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

	while(1){
		if(seen != 0x05){
			reload_check();
			Sleep(50);
		} else {
			break;
		}
		get_th105param();
	}
	if(t1 == 0){
		//Sleep(16);
		t1 = battle_time;//同期ズレし難くなる様に
		while(t1 == battle_time){
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
	if(((is_swr && weather2 == 14) || (is_soku && weather2==19)) && 1000-weather_delay<weather_time){
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
	if(delay == 0){
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

	engine->setScriptValueBool("is_th105",is_swr);
	engine->setScriptValueBool("is_th123",is_soku);
	engine->loadable = true;
}

int GetPlayerAddr(int player){
	int p;
	if(player == MY){
		p = my_data.base_addr;
	} else {
		p = enemy_data.base_addr;
	}
	return p;
}

char GetSkillLv(int player,int n){
	if(n < 0 || n > 14 || (player!=MY && player!=ENEMY))return -1;
	if(player==MY){
		return my_data.skill[n];
	} else {
		return enemy_data.skill[n];
	}
}

short GetCardId(int player,int n){
	int p,temp,temp2,temp3;
	int a;

	if(weather==11 || (player!=MY && player!=ENEMY) || n<0 || n>5)return -1;
	if(player==MY){
		return my_data.card[n].id;
	} else {
		return enemy_data.card[n].id;
	}
}
int GetCardCost(int player,int n){
	int p,temp,temp2,temp3;
	short a;

	if(weather==11 || (player!=MY && player!=ENEMY) || n<0 || n>5)return -1;
	if(player==MY){
		return my_data.card[n].cost;
	} else {
		return enemy_data.card[n].cost;
	}
}
int GetCardCost2(int player,int n){
	int a;

	a = GetCardCost(player,n);
	if(a==-1){
		return -1;
	}
	if(weather==2 && a>1)a--;
	return a;
}

short *GetDeckList(int player){
	static short deck[256];
	int c;
	FILE *fp;
	char fn[256],s[256],path[256];
	int i;
	unsigned char n;

	if(player==0){
		c = my_data.char_id;
		ReadProcessMemory(ph,(void *)ADDR_LPROFNAME,s,20,NULL);
	} else {
		c = enemy_data.char_id;
		ReadProcessMemory(ph,(void *)ADDR_RPROFNAME,s,20,NULL);
	}

	strcpy(fn,exe_path);
	i = strlen(fn);
	while(i>0 && fn[i]!='/' && fn[i]!='\\')i--;
	fn[i] = '\0';
	sprintf(path,"%s/Profile/%s.dat",fn,s);
	fp = fopen(path,"r");
	if(fp==NULL){
		i = *(int *)s;
		ReadProcessMemory(ph,(void *)i,s,256,NULL);
		sprintf(path,"%s/Profile/%s.dat",fn,s);
		fp = fopen(path,"r");
		if(fp==NULL)return NULL;
	}
	fseek(fp,106,SEEK_SET);
	i = 0;
	while(i <= c){
		fread(&n,1,1,fp);
		if(i==c){
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


void is_bullethit(void){
	int i,min,temp,l;

	obj_dis = 10000;
	obj_dis2 = 10000;

	l = 0;
	while(l < enemy_data.object.size()){
		obj *a = enemy_data.GetObject(l);
		temp = abs(my_data.x - a->x);
		if(temp<obj_dis2){
			obj_dis2 = temp;
		}

		if(a->attackarea_n < 16){
			i = 0;
			while(i < a->attackarea_n){
				if(a->attackarea[i].left<my_data.x && a->attackarea[i].right>my_data.x){
					obj_dis = 1;
				} else {
					min = abs(a->attackarea[i].left - my_data.x);
					if(min > abs(a->attackarea[i].right - my_data.x)){
						min = abs(a->attackarea[i].right - my_data.x);
					}
					if(min < obj_dis){
						obj_dis = min;
					}
				}
				i++;
			}
		}
		l++;
	}
}

BOX *get_hitarea(int player,int n){
	if((player!=MY && player!=ENEMY) || n<0 || (player==MY && n>=my_data.hitarea_n) || (player==ENEMY && n>=enemy_data.hitarea_n)){
		return NULL;
	}
	if(player==MY){
		return &my_data.hitarea[n];
	} else {
		return &enemy_data.hitarea[n];
	}
}
BOX *get_attackarea(int player,int n){
	if((player!=MY && player!=ENEMY) || n<0 || (player==MY && n>=my_data.attackarea_n) || (player==ENEMY && n>=enemy_data.attackarea_n)){
		return NULL;
	}
	if(player==MY){
		return &my_data.attackarea[n];
	} else {
		return &enemy_data.attackarea[n];
	}
}

int get_correction(int player,int flag){
	int a;
	if(player==0){
		a = my_data.correction;
	} else {
		a = enemy_data.correction;
	}
	return a&flag;
}

int GetSpecialData(int player,int n){
	int data;
	if((player!=MY && player!=ENEMY) || n<0 || n>SPECIALDATA_MAX){
		return -1;
	}
	//緋想緋緋想だけ逆
	if(n==9){
		if(player==MY){
			player = ENEMY;
		} else {
			player = MY;
		}
	}
	if(player==MY){
		data = my_data.sp_data[n];
	} else {
		data = enemy_data.sp_data[n];
	}
	return data;
}

void changeDbgBreakPoint(void){
	HMODULE hm;
	DWORD old;
	int paddr,a=1;
#ifdef DEBUG2
	return;
#endif

	hm = GetModuleHandle(&"\0ntdll.dll"[a]);
	paddr = (int)GetProcAddress(hm,&"\0DbgBreakPoint"[a]);
	if(*(unsigned char *)paddr != 0xCC){
//		printf("Error\n");//機械語が想定しているものと違うため中断
		return;
	}
	if(VirtualProtect((void *)hm,(int)paddr-(int)hm+1,PAGE_EXECUTE_READWRITE,&old)){//勝手に範囲を決めてくれるのでアバウドでよい
		*(char *)paddr = 0xC3;
		VirtualProtect((void *)hm,paddr-(int)hm+1,old,&old);//変更前の権限に戻す
	}
}
void changeLoadLibraryExW(int flag){
	HMODULE hm;
	DWORD old;
	static unsigned char code[15];
	int paddr,a=1;
#ifdef DEBUG2
	return;
#endif
	hm = GetModuleHandle("Kernel32.dll");
	paddr = (int)GetProcAddress(hm,&"\0LoadLibraryExW"[a]);
	if(flag == 0){
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
	if(*(int *)code != *(int *)paddr || *(int *)&code[4] != *(int *)(paddr+4)){
		//printf("Error\n");//機械語が想定しているものと違うため中断
		return;
	}
	if(VirtualProtect((void *)hm,(int)paddr-(int)hm+7,PAGE_EXECUTE_READWRITE,&old)){//勝手に範囲を決めてくれるのでアバウドでよい
		if(flag == 0){
			((char*)paddr)[0] = 0xb8;
			*(int*)&((char*)paddr)[1] = 0x00000000;
			((char*)paddr)[5] = 0xc2;
			*(short*)&((char*)paddr)[6] = 0x000c;
		} else {
			((char*)paddr)[0] = 0x6A;//code[0]~code[6]まで上書きで消してしまう機械語の代わり
			((char*)paddr)[1] = 0x34;
			((char*)paddr)[2] = 0x68;
			*(int *)&((char*)paddr)[3] = 0x7C80E0E8;
			((char*)paddr)[7] = 0xE8;
		}
		VirtualProtect((void *)hm,paddr-(int)hm+5,old,&old);//変更前の権限に戻す
	}
/*
	if(LoadLibraryA("user32.dll") == NULL){//成功していれば必ず存在するdll相手でも失敗するはず
		printf("OK\n");
	} else {
		printf("NG\n");
	}
*/
}

#ifdef DEBUG
int main(int argc,char *_argv[]){
#else
int main2(int argc,char *_argv[]){
#endif
	char s[256],script[8192];
	hInst = GetModuleHandle(NULL);

	SetUnhandledExceptionFilter(exep);
	my_wh = GetForegroundWindow();
	init();
	timeBeginPeriod(1);
	engine = scriptLua::getCurrentInstance();
	engine->init();
	changeDbgBreakPoint();
	changeLoadLibraryExW(0);

	char *argv[64];
	{
		int i = 0;
		while(i < argc){
			argv[i] = _argv[i];
			i++;
		}
		i = 0;
		while(i < argc){
			if(argv[i][0]=='-'){
				if(strcmp(argv[i],"-signature")==0){
					strcpy(aimanager_signature,argv[i+1]);
					memmove(&argv[i],&argv[i+2],sizeof(char*)*(argc-i-2));
					argc -= 2;
				} else if(strcmp(argv[i],"-char")==0){
					int n = atoi2(argv[i+1],-1);
					if(n==-1){
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

	if(argc == 2){
		OpenAI(argv[1]);
	} else if(ini_value("StartupAI") != NULL){
		strcpy(s,ini_value("StartupAI"));
		to_lowstring(s);
		OpenAI(s);
	} else {
		OpenAI(NULL);
	}
	timeEndPeriod(1);
	return 0;
}
