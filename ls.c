#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

void fmtname(char* path, int type, int ino, int size, int flag);
void ls(char *path);


void fmtname(char* path, int type, int ino, int size, int flag)
{
  static char buf[DIRSIZ+1];
  char *p;


  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  if (*p == '.' && flag == 0) {
    return;
  }

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    printf(1, "%s %d %d %d\n", p, type, ino, size);
  memmove(buf, p, strlen(p));
  if (type == 1) {
    memset(buf+strlen(p), '/', 1);
    memset(buf+strlen(p)+1, ' ', DIRSIZ-strlen(p)-1);
  } 
  else
    memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  printf(1, "%s %d %d %d\n", buf, type, ino, size);
}



void ls(char *path)
{
  // Check flag
  int flag = 0;
  if (*path == '-' && *(path+1) == 'a' && *(path+2) == '\0') {
    flag = 1;
  }
  if (*path == '-' && *(path+1) != 'a') {
    printf(1, "usage: ls [-a]\n");
    exit();
  }
  path = ".";

  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    printf(2, "ls: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    printf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    fmtname(path, st.type, st.ino, st.size, 1);
    break;
  case T_EXTENT:
    fmtname(path, st.type, st.ino, st.size, 1);
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf(1, "ls: path too long\n");
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
        printf(1, "ls: cannot stat %s\n", buf);
        continue;
      }
      fmtname(buf, st.type, st.ino, st.size, flag);
    }
    break;
  }
  close(fd);
}



int main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    ls(".");
    exit();
  }
  if (argc > 2) {
    printf(1, "usage: ls [-a]\n");
    exit();
  }
  for(i=1; i<argc; i++)
    ls(argv[i]);
  exit();
}
