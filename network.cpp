#include <stdio.h>
#include <time.h>
#include <windows.h>
#include <winhttp.h>
#include <atlbase.h>
#include <atlapp.h>
#include <atlstr.h>
#include "network.h"
#pragma comment(lib,"winhttp.lib")


class HttpClient{
	HINTERNET session;
	HINTERNET connection;
	HINTERNET request;

	CStringW UserAgent;

public:

	HttpClient(){
		session = connection = request = NULL;
		UserAgent = "HttpClient";
	}

	bool GetResponseHeaderSelect(CStringW &ptr, DWORD type) const{
		DWORD size=0;
		bool results = WinHttpQueryHeaders(
			request,
			type,
			WINHTTP_HEADER_NAME_BY_INDEX,
			NULL, 
			&size,
			WINHTTP_NO_HEADER_INDEX);
		wchar_t *data;
		data = static_cast<wchar_t*>(malloc(size));
		if(data==NULL){
			return false;
		}
		results = WinHttpQueryHeaders( 
			request,
			type,
			WINHTTP_HEADER_NAME_BY_INDEX,
			data,
			&size,
			WINHTTP_NO_HEADER_INDEX);
		if(!results){
			free(data);
			return false;
		}
		ptr = data;
		free(data);
		return true;
	}

	bool GetResponseHeaders(CStringW &ptr) const{
		return GetResponseHeaderSelect(ptr, WINHTTP_QUERY_RAW_HEADERS_CRLF);
	}
	bool GetDate(CStringW &ptr) const{
		return GetResponseHeaderSelect(ptr, WINHTTP_QUERY_DATE);
	}
	bool GetServer(CStringW &ptr) const{
		return GetResponseHeaderSelect(ptr, WINHTTP_QUERY_SERVER);
	}
	bool GetLastModified(CStringW &ptr) const{
		return GetResponseHeaderSelect(ptr, WINHTTP_QUERY_LAST_MODIFIED);
	}
	bool GetETag(CStringW &ptr) const{
		return GetResponseHeaderSelect(ptr, WINHTTP_QUERY_ETAG);
	}
	bool GetContentType(CStringW &ptr) const{
		return GetResponseHeaderSelect(ptr, WINHTTP_QUERY_CONTENT_TYPE);
	}
	bool GetLocation(CStringW &ptr) const{
		return GetResponseHeaderSelect(ptr, WINHTTP_QUERY_LOCATION);
	}
	bool GetSetCookie(CStringW &ptr) const{
		return GetResponseHeaderSelect(ptr, WINHTTP_QUERY_SET_COOKIE);
	}
	bool GetContentLength(DWORD &ptr) const{
		CStringW str;
		BOOL ret;
		ret = GetResponseHeaderSelect(str, WINHTTP_QUERY_CONTENT_LENGTH);
		if(ret==TRUE){
			ptr = _wtoi(str);
		} else {
			ptr = -1;
		}
		return ret;
	}

	void SetUserAgent(const CStringW &ua){
		UserAgent = ua;
	}

	bool CreateSession(void){
		WINHTTP_CURRENT_USER_IE_PROXY_CONFIG ieProxyConfig = { 0 };

		if (!::WinHttpGetIEProxyConfigForCurrentUser(&ieProxyConfig)){
			printf("Error:ProxyConfigGet(%d)\n",GetLastError());
			return false;
		}
		//wprintf(L"User-Agent: %s\n", UserAgent);
		if(ieProxyConfig.lpszProxy!=NULL){
			session = WinHttpOpen(
				UserAgent,
				WINHTTP_ACCESS_TYPE_NAMED_PROXY,
				ieProxyConfig.lpszProxy,
				ieProxyConfig.lpszProxyBypass,
				0);
		} else {
			session = WinHttpOpen(
				UserAgent,
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

		if(!session){
			printf("Error:Session %d has occurred.\n",GetLastError());
			return false;
		}
		return true;
	}

	char *SimpleGet(const CStringW &url, DWORD &code,DWORD &data_size){
		return HttpRequest("GET",false,url,NULL,code,data_size);
	}
	char *SimplePost(const CStringW &url, const char *post_data, DWORD &code,DWORD &data_size){
		return HttpRequest("POST",true,url,post_data,code,data_size);
	}
	char *HttpRequest(const CStringW method, bool is_post, const CStringW &url, const char *post_data, DWORD &code,DWORD &data_size){
		BOOL result;
		{
			CStringA s(url);
			//printf("HttpClient:Begin%s(%s)\n",method,s.GetBuffer());s.ReleaseBuffer();
		}

		URL_COMPONENTS url_components = {sizeof(URL_COMPONENTS)};
		url_components.dwSchemeLength    = -1;
		url_components.dwHostNameLength  = -1;
		url_components.dwUrlPathLength   = -1;
		url_components.dwExtraInfoLength = -1;
		if(!WinHttpCrackUrl(url, url.GetLength(), 0, &url_components)){
			printf("HttpClient:Error WinHttpCrackUrl(%d)\n",GetLastError());
			return NULL;
		}
		CStringW host(url_components.lpszHostName, url_components.dwHostNameLength);
		CStringW path(url_components.lpszUrlPath);
		CStringW scheme(url_components.lpszScheme, url_components.dwSchemeLength);

		if(session==NULL){
			CreateSession();
		}
		connection = ::WinHttpConnect(session,
			host,
			url_components.nPort,
			0);
		if(!connection){
			printf("Error:Connection %d has occurred.\n",GetLastError());
			return NULL;
		}
		DWORD flag = wcscmp(scheme,L"https")==0 ? WINHTTP_FLAG_SECURE : 0;
		request = ::WinHttpOpenRequest(connection,
			method,
			path,
			NULL,
			WINHTTP_NO_REFERER,
			WINHTTP_DEFAULT_ACCEPT_TYPES,
			flag);
		if(!request){
			printf("Error:request %d has occurred.\n",GetLastError());
			return NULL;
		}
		result = WinHttpAddRequestHeaders(request, 
			L"Accept-Language: ja",
			-1L,
			WINHTTP_ADDREQ_FLAG_ADD);
		if(!result){
			printf("Error:AddRequestHeaders %d has occurred.\n",GetLastError());
			return NULL;
		}
		if(is_post){
			result = WinHttpAddRequestHeaders(request, 
				L"Content-Type: multipart/form-data; boundary=---------------------------12672135622879",
				-1L,
				WINHTTP_ADDREQ_FLAG_ADD);
			if(!result){
				printf("Error:AddRequestHeaders %d has occurred.\n",GetLastError());
				return NULL;
			}
		}

		{
			const char *data;
			int size;
			if(!is_post){
				data = WINHTTP_NO_REQUEST_DATA;
				size = 0;
			} else {
				data = post_data;
				size = data_size;
			}
			data_size = -1;
			result = WinHttpSendRequest(request, 
				WINHTTP_NO_ADDITIONAL_HEADERS,
				0, static_cast<void *>(const_cast<char *>(data)),
				size, size,
				NULL);//コールバック先に渡される
		}

		if(!result){
			printf("Error:WinHttpSendRequest %d has occurred.\n",GetLastError());
			return NULL;
		}

		result = WinHttpReceiveResponse(request, NULL);

		if(!result){
			printf("Error:WinHttpReceiveResponse %d has occurred.\n",GetLastError());
			return NULL;
		}
		DWORD size=4;
		char *body;
		WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &code, &size, WINHTTP_NO_HEADER_INDEX);
		char *data;
		if(code/100==2){
			GetContentLength(size);
			//printf("ContentLength: %d\n",size);
			if(size==-1){
				size = 0;
				data = NULL;
				while(true){
					DWORD t_size;
					if (!WinHttpQueryDataAvailable(request, &t_size)) {
						printf("Error:WinHttpQueryDataAvailable %d has occurred.\n",GetLastError());
					}
					if(t_size==0){
						break;
					}
					body = static_cast<char*>(malloc(size+t_size+1));
					if(data!=NULL){
						memcpy(body,data,size);
						free(data);
					}
					WinHttpReadData(request,&body[size],t_size,NULL);
					size = size+t_size;
					body[size] = '\0';
					data = body;
				}
			} else {
				data = static_cast<char*>(malloc(size+1));
				char *s = data;
				while(s < data+size){
					DWORD t_size;
					if (!WinHttpQueryDataAvailable(request, &t_size)) {
						printf("Error:WinHttpQueryDataAvailable %d has occurred.\n",GetLastError());
					}
					//printf("add %dbyte\n",t_size);
					if(t_size==0 || s+t_size>data+size){
						break;
					}
					WinHttpReadData(request,s,t_size,NULL);
					s += t_size;
					*s = '\0';
				}
				//printf("ReadEnd %dbytes\n",s-data);
			}
			data_size = size;
		}
		WinHttpCloseHandle(request);
		WinHttpCloseHandle(connection);
		WinHttpCloseHandle(session);
		//printf("End HttpRequest\n");
		return data;
	}
};



char *post(const char *address,const char *param,int size=-1){
	if(size==-1){
		size = strlen(param);
	}
	HttpClient http;
	DWORD code,t_size;
	t_size = size;
	char *data = http.SimplePost(address,param,code,t_size);
	if(data!=NULL && code/100==2){
		return data;
	} else {
		return NULL;
	}
}

char *get(const char *address,int *data_len){
	HttpClient http;
	DWORD code,size;
	char *data = http.SimpleGet(address,code,size);
	//printf("code:%d\nsize:%d\nbody:%s",code,size,data);
	if(data!=NULL && code/100==2){
		if(data_len!=NULL){
			*data_len = size;
		}
		return data;
	} else {
		if(data_len!=NULL){
			data_len = 0;
		}
		return NULL;
	}
}

void add_param(char *data,const char *name,const char *value){
	strcat(data, "-----------------------------12672135622879\r\n");
	strcat(data, "Content-Disposition: form-data; name=\"");
	strcat(data, name);
	strcat(data, "\"\r\n\r\n");
	strcat(data, value);
	strcat(data, "\r\n");
	return;
}

void add_param(char *data,const char *name, int value){
	char temp[256];
	sprintf(temp,"%d",value);
	add_param(data,name,temp);
	return;
}

void ErrorPost(const char *url,const char *s){
	char data[4096]="";
	add_param(data,"error",s);
	strcat(data,"-----------------------------12672135622879--\r\n\r\n");
	free(post(url,data));
}

bool AIZipPost(const char *url,const char *fn,int char_id,const char *signature,const char *name,const char *description,const char *root){
	char *data,*s;
	FILE *fp;
	int size;
	bool ret;

	int param_size = strlen(name) + strlen(description) + strlen(root) + strlen(fn);
	fp = fopen(fn,"rb");
	fseek(fp,0,SEEK_END);
	size = ftell(fp);
	fseek(fp,0,SEEK_SET);
	data = static_cast<char *>(malloc(sizeof(char)*(size+2048+param_size)));
	data[0] = '\0';

	add_param(data,"char_id",char_id);
	add_param(data,"signature",signature);
	add_param(data,"name",name);
	add_param(data,"description",description);
	add_param(data,"root_script",root);

	sprintf(&data[strlen(data)],"-----------------------------12672135622879\r\n");
	sprintf(&data[strlen(data)], "Content-Disposition: form-data; name=\"uploaded_file\"; filename=\"%s\"\r\n",fn);
	sprintf(&data[strlen(data)], "Content-Type: application/x-zip-compressed\r\n");
	sprintf(&data[strlen(data)], "\r\n");
	s = &data[strlen(data)];
	fread(s,1,size,fp);
	fclose(fp);
	s += size;
	sprintf(s,"-----------------------------12672135622879--\r\n\r\n");
	s+=strlen(s);

	s = post(url,data,s-data);
	free(data);
	data = s;
	if(data==NULL){
		ret = false;
	} else {
		if(strstr(data,"OK")==NULL){
			ret = false;
		} else {
			ret = true;
		}
		//printf("data:%s",data);
		free(data);
	}
	return ret;
}

bool PostDeleteMethod(const char *url,int char_id,const char *signature,const char *name,const char *description,const char *root){
	char data[2048]="",*s;
	bool ret;

	add_param(data,"char_id",char_id);
	add_param(data,"signature",signature);
	add_param(data,"method","delete");
	strcat(data,"-----------------------------12672135622879--\r\n\r\n");

	s = post(url,data,strlen(data));
	//printf("post:%s\nresponse:%s\n",data,s);
	if(s==NULL){
		ret = false;
	} else {
		if(strstr(s,"OK")==NULL){
			ret = false;
		} else {
			ret = true;
		}
		free(s);
	}
	return ret;
}
bool PostScriptError(const char *url,const char *signature, const char *e){
	char data[4096]="",*s;
	bool ret;

	add_param(data,"signature",signature);
	add_param(data,"method","error_regist");
	add_param(data,"error",e);
	strcat(data,"-----------------------------12672135622879--\r\n\r\n");

	s = post(url,data,strlen(data));
	//printf("post:%s\nresponse:%s\n",data,s);
	if(s==NULL){
		ret = false;
	} else {
		if(strstr(s,"OK")==NULL){
			ret = false;
		} else {
			ret = true;
		}
		free(s);
	}
	return ret;
}


