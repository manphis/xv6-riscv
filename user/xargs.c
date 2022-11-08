#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

void sub_routine(char *argv[]) {
	if (fork() == 0) {
		exec(argv[0], argv);
		exit(0);
	} else {
		wait(0);
	}
	return;
}

int main(int argc, char *argv[]) {
	char buf[64];
	char ch;
	char *_argv[MAXARG];
	int i;

	if (argc < 2) {
    	fprintf(2, "Usage: xargs <cmd> <params>\n");
    	exit(1);
  	}

  	for (i = 1; i < argc; i++) {
  		_argv[i-1] = argv[i];
  	}
  	_argv[argc-1] = buf;
  	memset(buf, 0, sizeof(buf));
	
	int j = 0;
 	while (read(0, &ch, 1) != 0) {
 		if (ch == '\n') {
 			buf[j] = 0;
 			sub_routine(_argv);
 			j = 0;
 		} else {
 			buf[j] = ch;
 			j++;
 		}
 	}

	exit(0);
}