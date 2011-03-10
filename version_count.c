#include <stdio.h>
#include <stdlib.h>

main(){
	FILE *fp;
	char str[256],ver[256];
	int a,i,size;
	const char *template_text = 
"#define VERSION %d\n"
"#define OPEN_VERSION \"%s\"\n"
"#define API_SIZE %d\n";

	fp = fopen("history.txt","r");
	if(fp==NULL)return 1;

	fgets(str,256,fp);
	fclose(fp);
	if(str[strlen(str)-1] == '\n'){
		str[strlen(str)-1] = '\0';
	}
	i = 0;
	while(str[i] != '\t' && str[i] != '\0')i++;
	strcpy(ver,&str[i+1]);

	fp = fopen("version_impl.h","r");
	if(fp==NULL)return 1;

	fgets(str,256,fp);
	a = atoi(&str[16]);
	fclose(fp);

	fp = fopen("api.bin","rb");
	if(fp==NULL)return 1;
	fseek(fp,0,SEEK_END);
	size = ftell(fp);
	fclose(fp);

	fp = fopen("version_impl.h","w");
	if(fp==NULL) {
		return 1;
	}
	fprintf(fp,template_text, a+1, ver, size);
	fclose(fp);
	return 0;
}

