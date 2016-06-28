struct stat;
struct rtcdate;
struct RGBA;
struct RGB;
struct Window;
struct Point;
struct Size;

// system calls
int fork(void);
int exit(void) __attribute__((noreturn));
int wait(void);
int pipe(int*);
int write(int, void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(char*, char**);
int open(char*, int);
int mknod(char*, short, short);
int unlink(char*);
int fstat(int fd, struct stat*);
int link(char*, char*);
int mkdir(char*);
int chdir(char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);
void hello(struct RGB *, int, int);

// system calls for gui
int createwindow(int, int, int, int, char *, struct RGB*);
int repaintwindow(int);

// ulib.c
int stat(char*, struct stat*);
char* strcpy(char*, char*);
void *memmove(void*, void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
void printf(int, char*, ...);
char* gets(char*, int max);
uint strlen(char*);
void* memset(void*, int, uint);
void* malloc(uint);
void free(void*);
int atoi(const char*);

// bitmap.c
int readBitmapFile(char *, struct RGBA *, int *, int *);
int read24BitmapFile(char *, struct RGB *, int *, int *);

// xv6_api.c
int api_createwindow(struct Window *);
int api_paint24Bitmap(struct Window *, struct RGB*, struct Point, struct Size);
int api_paint24BitmapToContent(struct Window*, struct RGB*,struct Point,struct Point,struct Size, struct Size);
int api_repaint(struct Window *);
