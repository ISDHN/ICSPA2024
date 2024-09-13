#include <elf.h>
#include <utils.h>

static FILE *elf = NULL;
static Elf32_Ehdr ehdr;

int read(FILE *fd, void *buf, size_t start, size_t count) {
	fseek(fd, start, SEEK_SET);
	return fread(buf, 1, count, fd);
}

int init_elf(const char *exec_file) {
	elf = fopen(exec_file, "rb");
	if (elf == NULL) {
		panic("Can't open %s", exec_file);
		return -1;
	}
	unsigned char ident[EI_NIDENT] = {0};
	read(elf, ident, 0, EI_NIDENT);
	if (ident[EI_MAG0] != ELFMAG0 || ident[EI_MAG1] != ELFMAG1 || ident[EI_MAG2] != ELFMAG2 || ident[EI_MAG3] != ELFMAG3) {
		panic("Not an ELF file");
		return -1;
	}
	if (ident[EI_CLASS] != ELFCLASS32) {
		panic("Unsupported ELF file class");
		return -1;
	}
	read(elf, &ehdr, 0, sizeof(Elf32_Ehdr));
	Elf32_Shdr shdrs[ehdr.e_shnum];
	for (int i = 0; i < ehdr.e_shnum; i++) {
		read(elf, &shdrs[i], ehdr.e_shoff + i * ehdr.e_shentsize, ehdr.e_shentsize);
	}
	Elf32_Shdr shstrtab = shdrs[ehdr.e_shstrndx];

	return shstrtab.sh_offset;
}