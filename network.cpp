
#include "stdafx.h"

#pragma comment(lib,"winhttp.lib")

namespace {

void ErrorPrint(const std::string &func_name, unsigned int error_code) {
	printf("HttpClient:Error %s(%d)\n", func_name.c_str(), error_code);
}

typedef boost::shared_ptr<void> HINTERNET_PTR;

HINTERNET ToHINTERNET(HINTERNET_PTR ptr) {
	return reinterpret_cast<HINTERNET>(ptr.get());
}

void CloseHInternet(void *ptr) {
	HINTERNET handle = reinterpret_cast<HINTERNET>(ptr);
	if(ptr != NULL) {
		BOOL result = ::WinHttpCloseHandle(handle);
		if(result == FALSE) {
			ErrorPrint("WinHttpCloseHandle", ::GetLastError());
		}
	}
}

HINTERNET_PTR OpenSession(const std::wstring &user_agent, DWORD access_type, const std::wstring &proxy_name, const std::wstring &proxy_bypass, DWORD flags) {
	HINTERNET_PTR result;
	HINTERNET handle = ::WinHttpOpen(user_agent.c_str(), access_type, proxy_name.c_str(), proxy_bypass.c_str(), flags);
	if(handle == NULL) {
		ErrorPrint("WinHttpOpen", ::GetLastError());
		return result;
	}
	result.reset((void*)handle, CloseHInternet);
	return result;
}

HINTERNET_PTR OpenConnect(HINTERNET_PTR session, const std::wstring &host, unsigned int port, DWORD flags) {
	HINTERNET_PTR result;
	HINTERNET handle = ::WinHttpConnect(ToHINTERNET(session), host.c_str(), port, flags);
	if(handle == NULL) {
		ErrorPrint("WinHttpConnect", ::GetLastError());
		return result;
	}
	result.reset((void*)handle, CloseHInternet);
	return result;
}

HINTERNET_PTR OpenRequest(HINTERNET_PTR connection,
			const std::wstring &method,
			const std::wstring &path,
			const boost::optional<std::wstring> &version,
			const boost::optional<std::wstring> &referrer,
			const boost::optional<std::vector<wchar_t*> > &accept_list,
			DWORD flags)
{
	HINTERNET_PTR result;
	const wchar_t *version_impl = (version ? (*version).c_str() : NULL);
	const wchar_t *referrer_impl = (referrer ? (*referrer).c_str() : WINHTTP_NO_REFERER);
	const wchar_t **accept_list_impl = (accept_list ? (const wchar_t**)&(*accept_list).front() : WINHTTP_DEFAULT_ACCEPT_TYPES);
	HINTERNET handle = ::WinHttpOpenRequest(ToHINTERNET(connection), method.c_str(), path.c_str(), version_impl, referrer_impl, accept_list_impl, flags);
	if(handle == NULL) {
		ErrorPrint("WinHttpOpenRequest", ::GetLastError());
		return result;
	}
	result.reset((void*)handle, CloseHInternet);
	return result;
}

HINTERNET_PTR CreateSession(const std::wstring &user_agent) {
	WINHTTP_CURRENT_USER_IE_PROXY_CONFIG ieProxyConfig = { 0 };

	if (!::WinHttpGetIEProxyConfigForCurrentUser(&ieProxyConfig)){
		ErrorPrint("ProxyConfigGet", ::GetLastError());
		return HINTERNET_PTR();
	}
	HINTERNET_PTR session;
	if(ieProxyConfig.lpszProxy!=NULL){
		session = OpenSession(
			user_agent,
			WINHTTP_ACCESS_TYPE_NAMED_PROXY,
			ieProxyConfig.lpszProxy,
			ieProxyConfig.lpszProxyBypass,
			0);
	} else {
		session = OpenSession(
			user_agent,
			WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
			WINHTTP_NO_PROXY_NAME,
			WINHTTP_NO_PROXY_BYPASS,
			0);
	}
	if(ieProxyConfig.lpszAutoConfigUrl!=NULL){
		GlobalFree(ieProxyConfig.lpszAutoConfigUrl);
	}
	if(ieProxyConfig.lpszProxy!=NULL){
		GlobalFree(ieProxyConfig.lpszProxy);
	}
	if(ieProxyConfig.lpszProxyBypass!=NULL){
		GlobalFree(ieProxyConfig.lpszProxyBypass);
	}

	return session;
}

boost::optional<boost::tuple<HINTERNET_PTR, HINTERNET_PTR, HINTERNET_PTR> > SendRequest(
	const std::wstring &user_agent,
	const std::wstring &method,
	bool is_post,
	const std::wstring &url,
	const char *post_data,
	unsigned int post_data_size,
	DWORD *code)
{
	BOOST_ASSERT(!user_agent.empty());
	BOOST_ASSERT(!method.empty());
	BOOST_ASSERT(method == L"GET" || method == L"POST");
	BOOST_ASSERT(!url.empty());
	BOOST_ASSERT(post_data != NULL || post_data_size == 0);
	BOOST_ASSERT(code != NULL);

	URL_COMPONENTS url_components = {sizeof(URL_COMPONENTS)};
	url_components.dwSchemeLength    = -1;
	url_components.dwHostNameLength  = -1;
	url_components.dwUrlPathLength   = -1;
	url_components.dwExtraInfoLength = -1;
	if(::WinHttpCrackUrl(url.c_str(), url.size(), 0, &url_components) == FALSE){
		ErrorPrint("WinHttpCrackUrl", ::GetLastError());
		return boost::none;
	}
	const std::wstring host(url_components.lpszHostName, url_components.dwHostNameLength);
	const std::wstring path(url_components.lpszUrlPath);
	const std::wstring scheme(url_components.lpszScheme, url_components.dwSchemeLength);

	HINTERNET_PTR session = CreateSession(user_agent);
	if(!session) {
		return boost::none;
	}
	HINTERNET_PTR connection = OpenConnect(session, host, url_components.nPort, 0);
	if(!connection) {
		return boost::none;
	}

	DWORD flag = (scheme == L"https") ? WINHTTP_FLAG_SECURE : 0;
	HINTERNET_PTR request = OpenRequest(connection, method, path, boost::none, boost::none, boost::none, flag);
	if(!request){
		return boost::none;
	}

	{
		BOOL result = ::WinHttpAddRequestHeaders(ToHINTERNET(request), L"Accept-Language: ja", -1L, WINHTTP_ADDREQ_FLAG_ADD);
		if(!result){
			ErrorPrint("AddRequestHeaders", ::GetLastError());
			return boost::none;
		}
	}

	if(is_post){
		BOOL result = ::WinHttpAddRequestHeaders(ToHINTERNET(request),
			L"Content-Type: multipart/form-data; boundary=---------------------------12672135622879",
			-1L,
			WINHTTP_ADDREQ_FLAG_ADD);
		if(!result){
			ErrorPrint("AddRequestHeaders", ::GetLastError());
			return boost::none;
		}
	}

	{
		const char *data;
		unsigned int size;
		if(!is_post){
			data = WINHTTP_NO_REQUEST_DATA;
			size = 0;
		} else {
			data = post_data;
			size = post_data_size;
		}
		BOOL result = ::WinHttpSendRequest(ToHINTERNET(request), 
			WINHTTP_NO_ADDITIONAL_HEADERS,
			0, static_cast<void *>(const_cast<char *>(data)),
			size, size,
			NULL);//コールバック先に渡される
		if(!result){
			ErrorPrint("WinHttpSendRequest", ::GetLastError());
			return boost::none;
		}
	}

	{
		BOOL result = ::WinHttpReceiveResponse(ToHINTERNET(request), NULL);

		if(!result){
			ErrorPrint("WinHttpReceiveResponse", ::GetLastError());
			return boost::none;
		}
	}

	{
		DWORD size=4;
		BOOL result = ::WinHttpQueryHeaders(ToHINTERNET(request), WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &code, &size, WINHTTP_NO_HEADER_INDEX);
		ErrorPrint("WinHttpQueryHeaders", ::GetLastError());
	}

	if(*code / 100 != 2) {
		return boost::none;
	}

	return boost::make_tuple(session, connection, request);
}

bool GetResponseHeaderSelect(HINTERNET_PTR request, std::wstring *ptr, DWORD type) {
	DWORD size=0;
	BOOL result = ::WinHttpQueryHeaders(
		ToHINTERNET(request),
		type,
		WINHTTP_HEADER_NAME_BY_INDEX,
		NULL, 
		&size,
		WINHTTP_NO_HEADER_INDEX);
	if(result == TRUE || ::GetLastError() != ERROR_NOT_ENOUGH_MEMORY) {
		return false;
	}
	std::vector<wchar_t> data(size);
	result = ::WinHttpQueryHeaders( 
		ToHINTERNET(request),
		type,
		WINHTTP_HEADER_NAME_BY_INDEX,
		&data.front(),
		&size,
		WINHTTP_NO_HEADER_INDEX);
	if(result != TRUE){
		return false;
	}
	*ptr = std::wstring(&data.front());
	return true;
}

bool GetContentLength(HINTERNET_PTR request, int *ptr) {
	std::wstring str;
	bool ret;
	ret = GetResponseHeaderSelect(request, &str, WINHTTP_QUERY_CONTENT_LENGTH);
	if(ret){
		*ptr = boost::lexical_cast<int>(str);
	} else {
		*ptr = -1;
	}
	return ret;
}

void MyFClose(FILE *fp) {
	if(fp != NULL) {
		::fclose(fp);
	}
}

boost::shared_ptr<FILE> MyFOpen(const char *filename, const char *type) {
	BOOST_ASSERT(filename);
	BOOST_ASSERT(filename[0] != '\0');
	BOOST_ASSERT(type);
	BOOST_ASSERT(type[0] != '\0');
	return boost::shared_ptr<FILE>(::fopen(filename, type), &MyFClose);
}

} // anonymous



class HttpClient{
public:

	HttpClient() :
		user_agent(L"HttpClient")
	{
	}

	boost::optional<std::vector<char> > SimpleGet(const std::wstring &url, DWORD *code) {
		return HttpRequest(L"GET", false, url, NULL, 0, code);
	}
	boost::optional<std::vector<char> > SimplePost(const std::wstring &url, const char *post_data, unsigned int post_data_size, DWORD *code) {
		return HttpRequest(L"POST", true, url, post_data, post_data_size, code);
	}
	boost::optional<std::vector<char> > HttpRequest(const std::wstring &method, bool is_post, const std::wstring &url, const char *post_data, unsigned int post_data_size, DWORD *code){
		boost::optional<boost::tuple<HINTERNET_PTR, HINTERNET_PTR, HINTERNET_PTR> > handle_list = SendRequest(user_agent, method, is_post, url, post_data, post_data_size, code);
		if(!handle_list) {
			return boost::none;
		}
		HINTERNET_PTR session;
		HINTERNET_PTR connection;
		HINTERNET_PTR request;
		boost::tie(session, connection, request) = *handle_list;

		int size;
		//可変長の際は取得に失敗することがあるので成否は見ない
		GetContentLength(request, &size);
		//printf("ContentLength: %d\n",size);

		std::vector<char> result;
		if(size == -1){		//可変長データ
			size = 0;
			while(true){
				DWORD t_size;
				if (::WinHttpQueryDataAvailable(ToHINTERNET(request), &t_size) == FALSE) {
					ErrorPrint("WinHttpQueryDataAvailable", ::GetLastError());
					return boost::none;
				}
				result.resize(size + t_size);
				if(::WinHttpReadData(ToHINTERNET(request), &result[size], t_size, NULL) == FALSE) {
					ErrorPrint("WinHttpReadData", ::GetLastError());
					return boost::none;
				}
				size = result.size();
			}
		} else {		//固定長データ
			result.resize(size);
			char *s = &result.front();
			while(s <= &result.back()){
				DWORD t_size;
				if(::WinHttpQueryDataAvailable(ToHINTERNET(request), &t_size) == FALSE) {
					ErrorPrint("WinHttpQueryDataAvailable", GetLastError());
					return boost::none;
				}
				//printf("add %dbyte\n",t_size);
				if(t_size == 0 || s+t_size >= &result.back()) {
					return boost::none;
				}
				if(::WinHttpReadData(ToHINTERNET(request), s, t_size, NULL) == FALSE) {
					ErrorPrint("WinHttpReadData", ::GetLastError());
					return boost::none;
				}
				s += t_size;
			}
			//printf("ReadEnd %dbytes\n",s-data);
		}
		return result;
	}
protected:
	const std::wstring user_agent;
};



boost::optional<std::vector<char> > post(const std::wstring &uri, const char *data, int data_size=-1){
	if(data_size==-1){
		data_size = strlen(data);
	}
	HttpClient http;
	DWORD code;
	boost::optional<std::vector<char> > result = http.SimplePost(uri, data, data_size, &code);
	if(result && code/100==2){
		return result;
	} else {
		return boost::none;
	}
}

boost::optional<std::vector<char> > get(const std::wstring &uri){
	HttpClient http;
	DWORD code;
	boost::optional<std::vector<char> > result = http.SimpleGet(uri, &code);
	if(result && code/100==2){
		return result;
	} else {
		return boost::none;
	}
}

void add_param(char *data, const char *name, const char *value){
	::strcat(data, "-----------------------------12672135622879\r\n");
	::strcat(data, "Content-Disposition: form-data; name=\"");
	::strcat(data, name);
	::strcat(data, "\"\r\n\r\n");
	::strcat(data, value);
	::strcat(data, "\r\n");
}

void add_param(char *data,const char *name, int value){
	char temp[256];
	::sprintf(temp,"%d",value);
	add_param(data,name,temp);
}

std::wstring A2W(const char *str) {
	std::vector<wchar_t> wstr(::strlen(str)+1);
	setlocale(LC_ALL,"japanese");
	size_t wlen;
	errno_t error = ::mbstowcs_s(&wlen, &wstr.front(), wstr.size(), str, wstr.size()-1);
	if(error != 0) {
		return std::wstring();
	}
	return std::wstring(&wstr.front());
}

bool ResultCheck(const boost::optional<std::vector<char> >& result) {
	if(!result){
		return false;
	}
	if(strstr(&result.get().front(),"OK")==NULL){
		return false;
	} else {
		return true;
	}
}

void ErrorPost(const char *url,const char *s){
	char data[4096]="";
	add_param(data,"error",s);
	::strcat(data,"-----------------------------12672135622879--\r\n\r\n");
	post(A2W(url), data);
}

bool AIZipPost(const char *url, const char *fn, int char_id, const char *signature, const char *name, const char *description, const char *root){
	boost::shared_ptr<FILE> fp = MyFOpen(fn,"rb");
	if(!fp) {
		return false;
	}
	fseek(fp.get(), 0, SEEK_END);
	unsigned int file_size = ftell(fp.get());
	fseek(fp.get(), 0, SEEK_SET);
	unsigned int param_size = strlen(name) + strlen(description) + strlen(root) + strlen(fn);
	std::vector<char> data(sizeof(char)*(file_size + 2048 + param_size));
	char *s = &data.front();
	*s = '\0';

	add_param(s, "char_id",char_id);
	add_param(s, "signature",signature);
	add_param(s, "name",name);
	add_param(s, "description",description);
	add_param(s, "root_script",root);

	::strcat(s, "-----------------------------12672135622879\r\n");
	::sprintf(&s[strlen(s)], "Content-Disposition: form-data; name=\"uploaded_file\"; filename=\"%s\"\r\n",fn);
	::strcat(s, "Content-Type: application/x-zip-compressed\r\n");
	::strcat(s, "\r\n");
	s = &s[strlen(s)];
	fread(s, 1, file_size, fp.get());
	fp.reset();
	s += file_size;
	strcat(s, "-----------------------------12672135622879--\r\n\r\n");
	s += strlen(s);

	data.resize(s - &data.front());
	boost::optional<std::vector<char> > result = post(A2W(url), &data.front(), data.size());
	data.clear();

	return ResultCheck(result);
}

bool PostDeleteMethod(const char *url,int char_id,const char *signature,const char *name,const char *description,const char *root){
	char data[2048]="";
	bool ret;

	add_param(data,"char_id",char_id);
	add_param(data,"signature",signature);
	add_param(data,"method","delete");
	strcat(data,"-----------------------------12672135622879--\r\n\r\n");

	boost::optional<std::vector<char> > result = post(A2W(url), data, strlen(data));
	return ResultCheck(result);
}

bool PostScriptError(const char *url,const char *signature, const char *e){
	char data[4096]="",*s;
	bool ret;

	add_param(data,"signature",signature);
	add_param(data,"method","error_regist");
	add_param(data,"error",e);
	strcat(data,"-----------------------------12672135622879--\r\n\r\n");

	boost::optional<std::vector<char> > result = post(A2W(url), data, strlen(data));
	return ResultCheck(result);
}

