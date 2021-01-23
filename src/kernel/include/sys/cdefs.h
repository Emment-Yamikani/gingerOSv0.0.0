#ifndef _SYS_CDEFS_H
#define _SYS_CDEFS_H 1

#define __packed __attribute__((packed))
#define __align(i) __attribute__((align(i)))


typedef int pid_t;
typedef int tid_t;
typedef unsigned int off_t;
typedef unsigned char itype_t;
typedef unsigned char devid_t;
typedef int dev_t;
typedef int mode_t;
typedef int gid_t;
typedef int uid_t;
typedef int id_t;
#endif