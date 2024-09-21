#include <elf.h>
#include <utils.h>

typedef struct __Elf_Info {
	FILE *elf;
	Elf32_Ehdr ehdr;
	Elf32_Shdr *shdrs;
	Elf32_Sym *syms;
	char *shstrs;
	char *symstrs;
	int *func_list;
	int func_num;
} Elf_Info;

Elf_Info elfs[8];
int elf_cnt = 0;

int readbyte(FILE *fd, void *buf, size_t start, size_t count) {
	fseek(fd, start, SEEK_SET);
	return fread(buf, 1, count, fd);
}

int find_tab(Elf_Info *elf, const char *name) {
	for (int i = 0; i < elf->ehdr.e_shnum; i++) {
		if (strcmp(&(elf->shstrs[elf->shdrs[i].sh_name]), name) == 0) {
			return i;
		}
	}
	return -1;
}

int open_elf(const char *elf_file, Elf_Info *elf_info) {
	if (elf_file == NULL) {
		Log("No ELF file specified.");
		return -1;
	} else {
		Log("Loading ELF file %s", elf_file);
	}
	FILE *elf = fopen(elf_file, "rb");
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
	elf_info->elf = elf;
	return 0;
}

void read_shdrs(Elf_Info *elf_info) {
	elf_info->shdrs = calloc(elf_info->ehdr.e_shnum, sizeof(Elf32_Shdr));
	readbyte(elf_info->elf, elf_info->shdrs, elf_info->ehdr.e_shoff, elf_info->ehdr.e_shnum * elf_info->ehdr.e_shentsize);
}

void read_strtab(Elf_Info *elf_info, int tabindex, char **_buffer) {
	Elf32_Shdr tab = elf_info->shdrs[tabindex];
	if (tab.sh_type != SHT_STRTAB) {
		panic("The index %d doesn't refer to a valid strtab", tabindex);
		return;
	}
	*_buffer = malloc(tab.sh_size);
	readbyte(elf_info->elf, *_buffer, tab.sh_offset, tab.sh_size);
}

void read_func(Elf_Info *elf_info) {
	int symtab_index = find_tab(elf_info, ".symtab");
	if (symtab_index == -1) {
		panic("No symbol table found");
		return;
	}
	Elf32_Shdr symtab = elf_info->shdrs[symtab_index];
	int symnum = symtab.sh_size / sizeof(Elf32_Sym);
	elf_info->syms = malloc(symtab.sh_size);
	readbyte(elf_info->elf, elf_info->syms, symtab.sh_offset, symtab.sh_size);

	for (int i = 0; i < symnum; i++) {
		if (ELF32_ST_TYPE(elf_info->syms[i].st_info) == STT_FUNC) {
			elf_info->func_num++;
		}
	}
	elf_info->func_list = calloc(elf_info->func_num, sizeof(int));
	for (int i = 0, j = 0; i < symnum; i++) {
		if (ELF32_ST_TYPE(elf_info->syms[i].st_info) == STT_FUNC) {
			elf_info->func_list[j] = i;
			j++;
		}
	}
}

char *find_func_name(vaddr_t inst_addr) {
	for (int i = 0; i < elf_cnt; i++) {
		Elf_Info *elf_info = &elfs[i];
		for (int j = 0; j < elf_info->func_num; j++) {
			Elf32_Sym sym = elf_info->syms[elf_info->func_list[j]];
			if (sym.st_value <= inst_addr && inst_addr < sym.st_value + sym.st_size) {
				return &elf_info->symstrs[sym.st_name];
			}
		}
	}
	return "Unknown";
}

void init_elf(const char *exec_file) {
	Elf_Info *elf = &elfs[elf_cnt];

	if (open_elf(exec_file, elf)) {
		return;
	}

	elf_cnt++;

	readbyte(elf->elf, &elf->ehdr, 0, sizeof(Elf32_Ehdr));

	read_shdrs(elf);

	read_strtab(elf, elf->ehdr.e_shstrndx, &elf->shstrs);

	read_strtab(elf, find_tab(elf, ".strtab"), &elf->symstrs);

	read_func(elf);
}