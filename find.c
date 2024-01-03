#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "fcntl.h"

/**
    Table of values:
    if type = f
    flag = 1, value = 0
    if type = d
    flag = 1, value = 1
    if size = 40
    flag = 2, value = 40
    if default:
    flag = value = 0
*/

void printusage() {
    printf(1, "usage: find <folder> -name <name>\n");
    printf(1, "usage: find <folder> -name <name> -type <f/d>\n");
    printf(1, "usage: find <folder> -name <name> -size [+/-]<size>\n");
}

char *getlast(char *a) {
    char *p;
    for(p=a+strlen(a); p >= a && *p != '/'; p--)
        ;
    p++;
    return p;
}
    

int matchname(char *a, char *b) {
    return strcmp(getlast(a), b); 
}

void find(char *path, char *name, int flag, int value) {
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    printf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    printf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    if (matchname(path, name) == 0) {
        if ((flag == 0 || flag == 1) && value == 0) {
            printf(1, "%s\n", path);
        }
            
    }
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf(1, "find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
        if(de.inum == 0)
            continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if(stat(buf, &st) < 0){
            printf(1, "find: cannot stat %s\n", buf);
            continue;
        }
      //fmtname(buf, st.type, st.ino, st.size, flag);
        if (matchname(buf, name) == 0) {
            if ((flag == 0 || flag == 1) && (st.type == 1 && value == 1)) {
                printf(1, "%s\n", buf);
            }
            if (flag == 2 && st.size == value) {
                printf(1, "%s\n", buf);
            } else if (flag == 3 && st.size > value) {
                printf(1, "%s\n", buf);
            } else if (flag == 4 && st.size < value) {
                printf(1, "%s\n", buf);
            }
        }
        if ((strcmp(getlast(buf), ".") != 0) &&
            (strcmp(getlast(buf), "..") != 0)) {
            find(buf, name, flag, value);
        }
    }
    break;
  }
  close(fd);

}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printusage();
        exit();
    }
    char *default_dir = ".";
    char *path;
    char *name;

    int argcCheck;

    if (strcmp(argv[1], "-name") == 0) {
        path = default_dir;
        name = argv[2];
        argcCheck = 3;
    } else if (strcmp(argv[2], "-name") == 0) {
        path = argv[1];
        name = argv[3];
        argcCheck = 4;
    } else {
        printusage();
        exit();
    }

    int flag = 0;
    int value = 0;

    if (argc > argcCheck) {
        if (strcmp(argv[argcCheck], "-type") == 0) {
            flag = 1;
            if (strcmp(argv[argcCheck+1], "f") == 0) {
                value = 0;
            } else if (strcmp(argv[argcCheck+1], "d") == 0) {
                value = 1;
            } else {
                printusage();
                exit();
            }
        } else if (strcmp(argv[argcCheck], "-size") == 0) {
            if (*argv[argcCheck+1] == '+') {
                flag = 3;
                value = atoi(argv[argcCheck+1]+1);
            } else if (*argv[argcCheck+1] == '-') {
                flag = 4;
                value = atoi(argv[argcCheck+1]+1);
            } else {
                flag = 2;
                value = atoi(argv[argcCheck+1]);
            }

        } else {
            printusage();
            exit();
        }
    }
    if (argc > argcCheck + 2) {
        printusage();
        exit();
    }
    find(path, name, flag, value);
    exit();
}
