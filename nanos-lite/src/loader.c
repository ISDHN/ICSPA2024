#include <proc.h>
#include <common.h>
#include <elf.h>
#include <ramdisk.h>

#ifdef __LP64__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#define Elf_Class ELFCLASS64
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#define Elf_Class ELFCLASS32
#endif

static uintptr_t loader(PCB *pcb, const char *filename) {
	unsigned char ident[EI_NIDENT] = {0};
	ramdisk_read(ident, 0, EI_NIDENT);
	if (ident[EI_MAG0] != ELFMAG0 || ident[EI_MAG1] != ELFMAG1 || ident[EI_MAG2] != ELFMAG2 || ident[EI_MAG3] != ELFMAG3) {
		Log("Not an ELF file");
		return 0;
	}
	if (ident[EI_CLASS] != Elf_Class) {
		Log("Unsupported ELF file class");
		return 0;
	}

	Elf_Ehdr ehdr;
	ramdisk_read(&ehdr, 0, sizeof(Elf_Ehdr));

	Elf_Phdr phdrs[ehdr.e_phnum];
	ramdisk_read(phdrs, ehdr.e_phoff, ehdr.e_phnum * ehdr.e_phentsize);
	for (int i = 0; i < ehdr.e_phnum; i++) {
		Elf_Phdr phdr = phdrs[i];
		if (phdr.p_type != PT_LOAD) {
			continue;
		}
		ramdisk_read((void *)phdr.p_vaddr, phdr.p_offset, phdr.p_filesz);
		memset((void *)(phdr.p_vaddr + phdr.p_filesz), 0, phdr.p_memsz - phdr.p_filesz);
	}
	return ehdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
	uintptr_t entry = loader(pcb, filename);
	Log("Jump to entry = %p", entry);
	((void (*)())entry)();
}
