#ifndef FAT12_H
# define FAT12_H
#include <sys\cdefs.h>
#include <stddef.h>
#include <fs\vfs.h>

typedef struct _bs{
    uint8_t _jmp[3];
    uint8_t oem[8];
    uint16_t bytes_per_sect;
    uint8_t sects_per_cluster;
    uint16_t reserved_sects;
    uint8_t no_fats;
    uint16_t max_root_entries;
    uint16_t total_sects;
    uint8_t media;
    uint16_t fat_sz;
    uint16_t sects_per_track;
    uint16_t heads;
    uint32_t hidden_sects;
    uint32_t _tot_sects;
    uint8_t drive_no;
    uint8_t unused;
    uint8_t ext_boot_sign;
    uint32_t serial_no;
    uint8_t vol_lable[11];
    uint8_t filesys[8];
    uint8_t _[448];
    uint16_t boot_magic;
}__packed bs_t;

typedef union fat12_media_desc{
    struct {
        uint8_t sides: 1;
        uint8_t size: 1;
        uint8_t density: 1;
        uint8_t type: 1;
        uint8_t _unsed:4;
    };
    uint8_t raw;
}__packed fat12_media_desc_t;

typedef union _attrib
{
    struct
    {
        uint8_t read_only : 1;
        uint8_t hidden : 1;
        uint8_t system : 1;
        uint8_t vollab : 1;
        uint8_t subdir : 1;
        uint8_t archive : 1;
        uint8_t dev : 1;
        uint8_t _ : 1;
    }__packed structure;
    uint8_t raw;
}__packed attrib_t;

typedef struct fat12_dirent{
    uint8_t  name[8];
    uint8_t  ext[3];
    uint8_t  attrib;
    uint8_t  _unused;
    uint8_t  ctime;
    uint16_t ctime16;
    uint16_t cdate;
    uint16_t adate;
    uint16_t ea_index;
    uint16_t mtime;
    uint16_t mdate;
    uint16_t first_cluster;
    size_t   size;
}__packed fat12_dirent_t;

extern fs_t *fat12;

extern void fat12_init();
extern int fat12_load(inode_t *dev, inode_t **super);
extern int fat12_mount(const char *dir, int flags, void *data);

extern size_t fat12_iread(inode_t *inode, off_t offset, size_t size, void *buf);
extern size_t fat12_iwrite(inode_t *inode, off_t offset, size_t size, void *buf);
extern int fat12_iclose(inode_t *inode);

extern int fat12_creat(file_t *file);
extern int fat12_open (file_t *file);
extern int fat12_close(file_t *file);
extern int fat12_read (file_t *file, char *buf, size_t size);
extern int fat12_write(file_t *file, char *buf, size_t size);
extern int fat12_lseek(file_t *file, off_t off, int whence);
/*extern fat12_readdir();*/

#endif