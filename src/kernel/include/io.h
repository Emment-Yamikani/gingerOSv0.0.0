#ifndef IO_H
# define IO_H

extern int chdir(const char *);
extern char *getcwd(char *, int);
extern int mkdir(const char *);
extern char *mktemp(char *);
extern int rmdir(const char *);
extern int chmod(const char *, int);

extern int creat(const char *, int);
extern int open(const char *, int);
extern long lseek(int, long, int);
extern int read(int, void *, unsigned int);
extern int write(int, const void *, unsigned int);
extern int close(int);
extern int remove(const char *);
extern int rename(const char *, const char *);

#endif