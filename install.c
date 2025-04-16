#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int *CheckArgv(char **argv, char *flag);
char *ExecuteCmd(const char *cmd);

void *Libs[][2] = {
    {"str", {
        "git clone https://github.com/clibplus/str.git",
        "cd str",
        "sudo gcc -c str.c utils.c -g -g3 -ggdb",
        "sudo ar rcs str.a *.o; sudo mv str.a /usr/local/lib/libstr.a; sudo rm *.o",
        "cp str.h /usr/local/include",
        "cd ..",
        "rm -r str",
        NULL
    }},
    {"arr", {
        "git clone https://github.com/clibplus/arr.git",
        "cd arr",
        "sudo gcc -c arr.c -g -g3 -ggdb",
        "sudo ar rcs arr.a *.o;sudo  mv arr.a /usr/local/lib/libarr.a; sudo  rm *.o",
        "cp arr.h /usr/local/include",
        "cd ..",
        "rm -r arr",
        NULL
    }},
    {"map", {
        "git clone https://github.com/clibplus/map.git",
        "cd map",
        "sudo gcc -c map.c json.c -g -g3 -ggdb",
        "sudo ar rcs map.a *.o; sudo mv map.a /usr/local/lib/libmap.a; sudo rm *.o",
        "cp map.h /usr/local/include",
        "cd ..",
        "rm -r map",
        NULL
    }},
    {"OS", {
        "git clone https://github.com/clibplus/OS.git",
        "cd OS",
        "gcc -c file.c -g -g3 -ggdb",
        "ar rcs file.a *.o; mv file.a /usr/local/lib/libfile.a; rm *.o",
        "mkdir -p /usr/local/include/OS",
        "cp file.h /usr/local/include/OS/",

        "gcc -c utils.c -g -g3 -ggdb",
        "ar rcs utils.a *.o; mv utils.a /usr/local/lib/libutils.a; rm *.o",
        "cp utils.h /usr/local/include/OS/",
        "cd ..",
        "rm -r OS",
        NULL
    }},
    {"Net", {
        "git clone https://github.com/clibplus/Net.git",
        "cd Net",
        "gcc -c socket.c -lpthread",
        "ar rcs socket.a *.o; rm *.o; mv socket.a /usr/local/lib/libsocket.a",
        "mkdir -p /usr/local/include/Net",
        "cp socket.h /usr/local/include/Net",

        "gcc -c ssl.c -lssl -lcrypto",
        "ar rcs ssl.a *.o; rm *.o; mv ssl.a /usr/local/lib/libssl.a",
        "cp ssl.h /usr/local/include/Net",

        "gcc -c request.c -lssl -lcrypto -lstr -larr",
        "ar rcs request.a *.o; rm *.o; mv request.a /usr/local/lib/librequest.a",
        "cp request.h /usr/local/include/Net",

        "sudo gcc -c web.c web_config.c web_route.c websign/*.c -lstr -larr -lmap -lpthread -g -g1",
        "sudo ar rcs websign.a *.o; sudo rm *.o; sudo mv websign.a /usr/local/lib/libwebsign.a",
        "cp web.h /usr/local/include/Net",
        NULL
        
    }},
    NULL
};

int main(int argc, int argv) {
    if(!argc < 2) {
        printf("[ x ] Error, Invalid arguments provided....\r\nUsage: %s <opt>...!\n");
        exit(0);
    }

    if(CheckArgv(argv, "--install")) {

    }

    return 0;
}

int *CheckArgv(char **argv, char *flag) {
    if(!argv || !flag)
        return -1;

    for(int i = 0; argv[i] != NULL; i++) {
        if(!strcmp(argv[i], flag))
            return i;
    }

    return -1;
}

void RunInstall() {

}

char *ExecuteCmd(const char *cmd) {
	if (!cmd)
		return NULL;

	FILE *fd = popen(cmd, "r");
	if (!fd)
		return NULL;

	char BUFFER[1024];
	size_t size = 1; 
	char *data = malloc(size);
	if (!data) {
		fclose(fd);
		return NULL;
	}
	data[0] = '\0';

	while (fgets(BUFFER, sizeof(BUFFER), fd)) {
		size_t len = strlen(BUFFER);
		char *temp = realloc(data, size + len);
		if (!temp) {
			free(data);
			fclose(fd);
			return NULL;
		}
		data = temp;
		strcpy(data + size - 1, BUFFER);
		size += len;
	}

	fclose(fd);
	return data;
}