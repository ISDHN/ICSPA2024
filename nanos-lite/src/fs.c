#include <fs.h>

typedef size_t (*ReadFn)(void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn)(const void *buf, size_t offset, size_t len);

typedef struct {
	char *name;
	size_t size;
	size_t disk_offset;
	size_t open_offset;
	ReadFn read;
	WriteFn write;
} Finfo;

enum {
	FD_STDIN = 0,
	FD_STDOUT,
	FD_STDERR,
	FD_EVT,
	FD_DSPINFO,
	FD_FB,
	FD_SBCTL,
	FD_SB,
};

size_t invalid_read(void *buf, size_t offset, size_t len) {
	panic("This file is read only");
	return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
	panic("This file is write only");
	return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
	[FD_STDIN] = {"stdin", 0, 0, 0, invalid_read, invalid_write},
	[FD_STDOUT] = {"stdout", 0, 0, 0, invalid_read, serial_write},
	[FD_STDERR] = {"stderr", 0, 0, 0, invalid_read, serial_write},
	[FD_EVT] = {"/dev/events", 0, 0, 0, events_read, invalid_write},
	[FD_DSPINFO] = {"/proc/dispinfo", 128, 0, 0, dispinfo_read, invalid_write},
	[FD_FB] = {"/dev/fb", 0, 0, 0, invalid_read, fb_write},
	[FD_SBCTL] = {"/dev/sbctl", 0, 0, 0, sbctrl_read, sbctrl_write},
	[FD_SB] = {"/dev/sb", 0, 0, 0, invalid_read, sb_write},

#include "files.h"
};

void init_fs() {
	file_table[FD_FB].size = io_read(AM_GPU_CONFIG).width * io_read(AM_GPU_CONFIG).height * sizeof(uint32_t);

	for (int i = 0; i < sizeof(file_table) / sizeof(file_table[0]); i++) {
		if (file_table[i].read == NULL) {
			file_table[i].read = ramdisk_read;
			file_table[i].write = ramdisk_write;
		}
	}
}

int fs_open(const char *pathname, int flags, int mode) {
	for (int i = 0; i < sizeof(file_table) / sizeof(file_table[0]); i++) {
		if (strcmp(pathname, file_table[i].name) == 0) {
			return i;
		}
	}
	panic("file not found");
}

int fs_close(int fd) {
	return 0;
}

size_t fs_lseek(int fd, size_t offset, int whence) {
	Finfo *f = &file_table[fd];
	switch (whence) {
		case SEEK_SET:
			f->open_offset = offset;
			break;
		case SEEK_CUR:
			f->open_offset += offset;
			break;
		case SEEK_END:
			f->open_offset = f->size + offset;
			break;
		default:
			panic("Invalid whence");
	}
	return f->open_offset;
}

size_t fs_read(int fd, void *buf, size_t len) {
	Finfo *f = &file_table[fd];
	size_t count = f->read(buf, f->disk_offset + f->open_offset, len);
	f->open_offset += count;
	return count;
}

size_t fs_write(int fd, const void *buf, size_t len) {
	Finfo *f = &file_table[fd];
	size_t count = f->write(buf, f->disk_offset + f->open_offset, len);
	f->open_offset += count;
	return count;
}