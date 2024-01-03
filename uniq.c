#include "types.h"
#include "stat.h"
#include "user.h"

char buf[8192];

char tolowercase(char ch) {
    if (ch >= 65 && ch <= 122) {
        if (ch >= 65 && ch <= 90) {
            ch = ch + 32;
        }
        return ch;
    }
    return ch;
}

char *nextlinep(char *buffer) {
    while (*buffer != '\n') {
        buffer++;
    }
    buffer++;
    return buffer;
}

void writeline(char *p) {
    while (*p != '\n') {
        write(1, p, 1);
        p++;
    }
    printf(1, "\n");
}

int compareline(char *p, char *q) {
    if (*p == '\n' || *q == '\n') 
        return 0; //false

    while (1) {
        if (*p == *q) {
            if (*p == '\n') {
                return 1;
            } else {
                p++; q++;
            }
        } else {
            return 0;
        }
    }
}

int compareline_ignorecase(char *p, char *q) {
    if (tolowercase(*p) == '\n' || tolowercase(*q) == '\n') 
        return 0; //false

    while (1) {
        if (tolowercase(*p) == tolowercase(*q)) {
            if (tolowercase(*p) == '\n') {
                return 1;
            } else {
                p++; q++;
            }
        } else {
            return 0;
        }
    }
}

void uniq(int fd, int flag) {
    int n;
    char *start;
    char *line;
    char *nextline;
    n = read(fd, buf, sizeof(buf));
    if (n < 0) {
        printf(1, "uniq: read error\n");
        exit();
    }
    start = buf;
    line = buf;
    nextline = nextlinep(buf);
    if (nextline-start == n) {
	    if (flag == 0 || flag == 2)
		    writeline(line);
	    if (flag == 1 || flag == 4) {
		    printf(1, "1");
		    writeline(line);
	    } // if flag == 3 then returns
	    return;
    }
    if (flag == 0 || flag == 2) {
        while (1) {
            int predicate;
            if (flag == 0)
                predicate = compareline(line, nextline) == 1;
            else
                predicate = compareline_ignorecase(line, nextline) == 1;

            if (predicate) {
                line = nextline;
                nextline = nextlinep(nextline);
            } else {
                writeline(line);
                line = nextline;
                nextline = nextlinep(nextline);
            }
            if (nextline-start >= n) {
                writeline(line);
                break;
            }
        }
        //printf(1, "%d %d\n", nextline-start, n);
    }
    if (flag == 1 || flag == 4) {
        int occur = 1;
        while (1) {
	    int predicate;
            if (flag == 1) 
                predicate = compareline(line, nextline) == 1;
            else 
                predicate = compareline_ignorecase(line, nextline) == 1;
            if (predicate) {
                occur++;
		line = nextline;
		nextline = nextlinep(nextline);
            } else {
		    printf(1, "%d ", occur);
		    occur = 1;
		    writeline(line);
		    line = nextline;
		    nextline = nextlinep(nextline);
	    }
	    if (nextline-start >= n) {
		    printf(1, "%d ", occur);
		    writeline(line);
		    break;
	    }
        }
        
    }
    if (flag == 3 || flag == 5) {
        int occur = 1;
        while (1) {
            int predicate;
	    if (flag == 3)
		    predicate = compareline(line, nextline) == 1;
	    else
		    predicate = compareline_ignorecase(line, nextline) ==1;
            if (predicate) {
                occur++;
                line = nextline;
                nextline = nextlinep(nextline);
            } else {
                if (occur != 1)
                    writeline(line);
                occur = 1;
                line = nextline;
                nextline = nextlinep(nextline);
            }
            if (nextline-start >= n) {
                if (occur != 1)
                    writeline(line);
                break;
            }
        }
        
    }

}

int main(int argc, char* argv[]) {
    int fd;
    struct stat st;
    if (argc < 2) {
	    if(fstat(0,&st) == -1){
		    uniq(0,0);
	} else {
        	printf(1, "usage: uniq [-c/-d] [-i] <filename>\n");
		exit();
	}
 
    }
    if (argc == 2) { 
        if((fd = open(argv[1], 0)) < 0){
          printf(1, "uniq: cannot open %s\n", argv[1]);
          exit();
        }
        uniq(fd, 0);
        close(fd);
        exit();
    }
    if (argc == 3) {
        if((fd = open(argv[2], 0)) < 0){
          printf(1, "uniq: cannot open %s\n", argv[2]);
          exit();
        }
        if (*(argv[1]) == '-') {
            if (*(argv[1]+1) == 'c') {
                uniq(fd, 1);
            } else if (*(argv[1]+1) == 'i') {
                uniq(fd, 2);
            } else if (*(argv[1]+1) == 'd') {
                uniq(fd, 3);
            } else {
                printf(1, "usage: uniq [-c/-d] [-i] <filename>\n");
            }
        
        } else {
            printf(1, "usage: uniq [-c/-d] [-i] <filename>\n");
        }
        close(fd);
    }
    if(argc == 4){
	    if((fd = open(argv[3],0)) < 0){
		    printf(1, "uniq: cannot open %s\n", argv[3]);
		    exit();
	}
	if(*(argv[1]) == '-' && *(argv[2]) == '-') {
		if (*(argv[1]+1) == 'c' && *(argv[2]+1) == 'i' &&
			    	*(argv[1]+2) == '\0' && *(argv[2]+2) == '\0') {
			                uniq(fd, 4);
		   } else if (*(argv[1]+1) == 'i' && *(argv[2]+1) == 'c' &&
			     *(argv[1]+2) == '\0' && *(argv[2]+2) == '\0') {
		   uniq(fd,4);
		    } else if (*(argv[1]+1) == 'd' && *(argv[2]+1) == 'i' &&
			      *(argv[1]+2) == '\0' && *(argv[2]+2) == '\0') {
		   uniq(fd, 5);
		   } else if (*(argv[1]+1) == 'i' && *(argv[2]+1) == 'd' &&
			      *(argv[1]+2) == '\0' && *(argv[2]+2) == '\0') {
		   uniq(fd, 5);
		   } else {
		             printf(1, "usage: uniq [-c/-d] [-i] <filename>\n");
		   }
            } else {
	                printf(1, "usage: uniq [-c/-d] [-i] <filename>\n");
			        }
		    close(fd);
	     }
	         if (argc > 4) {
			         printf(1, "usage: uniq [-c/-d] [-i] <filename>\n");
				         exit();
					     }
					
		   

    exit();
}
