#include <proc.h>
#include <common.h>
#include <klib.h>
#include <elf.h>
#include <fs.h>

#ifdef __LP64__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#define Elf_Class ELFCLASS64
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#define Elf_Class ELFCLASS32
#endif

size_t readbytes(int fd, void *buf, size_t offset, size_t len) {
	fs_lseek(fd, offset, SEEK_SET);
	return fs_read(fd, buf, len);
}

uintptr_t loader(PCB *pcb, const char *filename) {
	unsigned char ident[EI_NIDENT] = {0};
	int fd = fs_open(filename, 0, 0);
	if (fd == -1) {
		return 0; // null
	}
	readbytes(fd, ident, 0, EI_NIDENT);
	if (ident[EI_MAG0] != ELFMAG0 || ident[EI_MAG1] != ELFMAG1 || ident[EI_MAG2] != ELFMAG2 || ident[EI_MAG3] != ELFMAG3) {
		Log("Not an ELF file");
		return 0;
	}
	if (ident[EI_CLASS] != Elf_Class) {
		Log("Unsupported ELF file class");
		return 0;
	}

	Elf_Ehdr ehdr;
	readbytes(fd, &ehdr, 0, sizeof(Elf_Ehdr));
	for (int i = 0; i < ehdr.e_phnum; i++) {
		Elf_Phdr phdr;
		readbytes(fd, &phdr, ehdr.e_phoff + i * ehdr.e_phentsize, ehdr.e_phentsize);
		if (phdr.p_type != PT_LOAD) {
			continue;
		}
		readbytes(fd, (void *)phdr.p_vaddr, phdr.p_offset, phdr.p_filesz);
		memset((void *)(phdr.p_vaddr + phdr.p_filesz), 0, phdr.p_memsz - phdr.p_filesz);
	}
	Log("%s entry point: %p", filename, ehdr.e_entry);
	return ehdr.e_entry;
}

int naive_uload(PCB *pcb, const char *filename) {
	uintptr_t entry = loader(pcb, filename);
	if (!entry) {
		return 1;
	}
	Log("Jump to entry = %p", entry);
	((void (*)())entry)();

	return 0;
}
