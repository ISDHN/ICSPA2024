#include <elf.h>
#include <utils.h>

FILE *elf = NULL;
Elf32_Ehdr ehdr;
Elf32_Shdr *shdrs = NULL;
Elf32_Sym *syms = NULL;
char *shstrs = NULL;
char *symstrs = NULL;
int *func_list = NULL;
int func_num = 0;

int readbyte(FILE *fd, void *buf, size_t start, size_t count) {
	fseek(fd, start, SEEK_SET);
	return fread(buf, 1, count, fd);
}

int find_tab(const char *name) {
	for (int i = 0; i < ehdr.e_shnum; i++) {
		if (strcmp(&shstrs[shdrs[i].sh_name], name) == 0) {
			return i;
		}
	}
	return -1;
}

int open_elf(const char *elf_file) {
	if (elf_file == NULL) {
		Log("No ELF file specified.");
		return -1;
	} else {
		Log("Loading ELF file %s", elf_file);
	}
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

void read_strtab(int tabindex, char **_buffer) {
	Elf32_Shdr tab = shdrs[tabindex];
	if (tab.sh_type != SHT_STRTAB) {
		panic("The index %d doesn't refer to a valid strtab", tabindex);
		return;
	}
	*_buffer = malloc(tab.sh_size);
	readbyte(elf, *_buffer, tab.sh_offset, tab.sh_size);
}

void read_func() {
	int symtab_index = find_tab(".symtab");
	if (symtab_index == -1) {
		panic("No symbol table found");
		return;
	}
	Elf32_Shdr symtab = shdrs[symtab_index];
	int symnum = symtab.sh_size / sizeof(Elf32_Sym);
	syms = malloc(symtab.sh_size);
	readbyte(elf, syms, symtab.sh_offset, symtab.sh_size);

	for (int i = 0; i < symnum; i++) {
		if (ELF32_ST_TYPE(syms[i].st_info) == STT_FUNC) {
			func_num++;
		}
	}
	func_list = calloc(func_num, sizeof(int));
	for (int i = 0, j = 0; i < symnum; i++) {
		if (ELF32_ST_TYPE(syms[i].st_info) == STT_FUNC) {
			func_list[j] = i;
			j++;
		}
	}
}

char *find_func_name(vaddr_t inst_addr) {
	if (elf == NULL) {
		return "Unknown";
	}
	for (int i = 0; i < func_num; i++) {
		if (syms[func_list[i]].st_value <= inst_addr && inst_addr < syms[func_list[i]].st_value + syms[func_list[i]].st_size) {
			return &symstrs[syms[func_list[i]].st_name];
		}
	}
	return "Unknown";
}

void init_elf(const char *exec_file) {

	if (open_elf(exec_file)) {
		return;
	}

	readbyte(elf, &ehdr, 0, sizeof(Elf32_Ehdr));

	read_shdrs();

	read_strtab(ehdr.e_shstrndx, &shstrs);

	read_strtab(find_tab(".strtab"), &symstrs);

	read_func();
}