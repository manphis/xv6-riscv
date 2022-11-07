#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

char* get_filename(char *path) {
  char *p;
  for (p=path+strlen(path); p>=path && *p != '/'; p--) {}
    p++;
  return p;
}

void find(char *path, char *name) {
	int fd;
	char buf[512], *p;
	struct dirent de;
	struct stat st;

	// printf("find: %s, name = %s\n", path, name);

	if ((fd = open(path, 0)) < 0) {
    	fprintf(2, "ls: cannot open %s\n", path);
    	return;
  	}
  	if (fstat(fd, &st) < 0) {
    	fprintf(2, "ls: cannot stat %s\n", path);
    	close(fd);
    	return;
  	}

  	switch (st.type) {
  		case T_FILE:
  			if (strcmp(get_filename(path), name) == 0) {
  				printf("%s\n", path);
  			}
        close(fd);
  			break;

  		case T_DIR:
  			if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf) {
      			printf("ls: path too long\n");
      			break;
    		}
    		strcpy(buf, path);
    		p = buf+strlen(buf);
    		*p++ = '/';

    		while(read(fd, &de, sizeof(de)) == sizeof(de)){
      			if (de.inum == 0)
        			continue;
        		if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        			continue;

      			memmove(p, de.name, DIRSIZ);
      			p[DIRSIZ] = 0;
      			if(stat(buf, &st) < 0){
        			printf("ls: cannot stat %s\n", buf);
        			continue;
      			}
      			find(buf, name);
    		}
        close(fd);
  			break;
  	}
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(2, "Usage: find <path> <name>\n");
    exit(1);
  }
	
  find(argv[1], argv[2]);

	exit(0);
}