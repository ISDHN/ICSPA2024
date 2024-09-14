#include <elf.h>
#include <utils.h>

static FILE *elf = NULL;
static Elf32_Ehdr ehdr;
static Elf32_Shdr *shdrs = NULL;
static Elf32_Sym *syms = NULL;
static char **shstrs = NULL;
static char *shstr_buffer = NULL;
static char **symstrs = NULL;
static char *symstr_buffer = NULL;

int readbyte(FILE *fd, void *buf, size_t start, size_t count) {
	fseek(fd, start, SEEK_SET);
	return fread(buf, 1, count, fd);
}

int find_tab(const char *name) {
	for (int i = 0; i < ehdr.e_shnum; i++) {
		if (strcmp(shstrs[shdrs[i].sh_name], name) == 0) {
			return i;
		}
	}
	return -1;
}

int open_elf(const char *elf_file) {
	elf = fopen(elf_file, "rb");
	if (elf == NULL) {
		panic("Can't open %s", elf_file);
		return -1;
	}
	unsigned char ident[EI_NIDENT] = {0};
	readbyte(elf, ident, 0, EI_NIDENT);
	if (ident[EI_MAG0] != ELFMAG0 || ident[EI_MAG1] != ELFMAG1 || ident[EI_MAG2] != ELFMAG2 || ident[EI_MAG3] != ELFMAG3) {
		panic("Not an ELF file");
		return -1;
	}
	if (ident[EI_CLASS] != ELFCLASS32) {
		panic("Unsupported ELF file class");
		return -1;
	}
	return 0;
}

void read_shdrs() {
	shdrs = calloc(ehdr.e_shnum, sizeof(Elf32_Shdr));
	readbyte(elf, shdrs, ehdr.e_shoff, ehdr.e_shnum * ehdr.e_shentsize);
}

void read_strtab(int tabindex, char **_buffer, char ***_strtab) {
	if (shdrs[tabindex].sh_type != SHT_STRTAB) {
		panic("The index %d doesn't refer to a valid strtab", tabindex);
		return;
	}
	Elf32_Shdr tab = shdrs[tabindex];
	*_buffer = malloc(tab.sh_size);
	readbyte(elf, *_buffer, tab.sh_offset, tab.sh_size);
	int strcnt = 0;
	for (int i = 0; i < tab.sh_size; i++) {
		if ((*_buffer)[i] == 0) {
			strcnt++;
		}
	}
	*_strtab = calloc(strcnt, sizeof(char *));
	(*_strtab)[0] = *_buffer;
	for (int i = 1, index = 1; i < tab.sh_size; i++) {
		if ((*_buffer)[i - 1] == 0) {
			(*_strtab)[index] = &(*_buffer)[i];
			index++;
		}
	}
}

void read_symtab() {
	int symtab_index = find_tab(".symtab");
	if (symtab_index == -1) {
		panic("No symbol table found");
		return;
	}
	Elf32_Shdr symtab = shdrs[symtab_index];
	syms = malloc(symtab.sh_size);
	readbyte(elf, syms, symtab.sh_offset, symtab.sh_size);
}

void init_elf(const char *exec_file) {

	if (open_elf(exec_file)) {
		return;
	}

	readbyte(elf, &ehdr, 0, sizeof(Elf32_Ehdr));

	read_shdrs();

	read_strtab(ehdr.e_shstrndx, &shstr_buffer, &shstrs);

	read_strtab(find_tab(".strtab"), &symstr_buffer, &symstrs);

	read_symtab();
}