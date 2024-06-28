#include "ft_nm.h"

/*

2. Lire les en-têtes de section
Ces en-têtes vous indiqueront où se trouvent les sections importantes comme la table des symboles et la table des chaînes.

3. Lire la table des symboles
Chaque entrée de la table des symboles contient des informations telles que l'adresse du symbole, la taille, le type et l'indice de la table des chaînes où se trouve le nom du symbole.

4. Lire la table des chaînes
Les noms des symboles se trouvent ici, et vous devrez les extraire en utilisant les indices obtenus à partir de la table des symboles.
*/

Elf64_Ehdr *read_elf_header(FILE *file)
{
    Elf64_Ehdr *header = malloc(sizeof(Elf64_Ehdr));
    fread(header, sizeof(Elf64_Ehdr), 1, file);
    return (header);

}

Elf64_Shdr *read_section_headers(FILE *file, Elf64_Ehdr *header)
{
    Elf64_Shdr *section_headers = malloc(sizeof(Elf64_Shdr) * header->e_shnum);
    fseek(file, header->e_shoff, SEEK_SET);
    fread(section_headers, sizeof(Elf64_Shdr), header->e_shnum, file);
    return (section_headers);
}

char get_symbol_type(Elf64_Sym *symbol, Elf64_Shdr *section_headers) {
    if (ELF64_ST_BIND(symbol->st_info) == STB_GNU_UNIQUE) {
        return 'u';
    } else if (ELF64_ST_BIND(symbol->st_info) == STB_WEAK) {
        if (ELF64_ST_TYPE(symbol->st_info) == STT_OBJECT) {
            return (symbol->st_shndx == SHN_UNDEF) ? 'v' : 'V';
        } else {
            return (symbol->st_shndx == SHN_UNDEF) ? 'w' : 'W';
        }
    } else if (ELF64_ST_BIND(symbol->st_info) == STB_LOOS &&
               ELF64_ST_TYPE(symbol->st_info) == STT_GNU_IFUNC) {
        return 'i';
    } else if (symbol->st_shndx == SHN_UNDEF) {
        return 'U';
    } else if (symbol->st_shndx == SHN_ABS) {
        return 'A';
    } else if (symbol->st_shndx == SHN_COMMON) {
        return 'C';
    } else if (section_headers[symbol->st_shndx].sh_type == SHT_NOBITS &&
               section_headers[symbol->st_shndx].sh_flags == (SHF_ALLOC | SHF_WRITE)) {
        return 'B';
    } else if (section_headers[symbol->st_shndx].sh_type == SHT_PROGBITS &&
               section_headers[symbol->st_shndx].sh_flags == SHF_ALLOC) {
        return 'R';
    } else if (section_headers[symbol->st_shndx].sh_type == SHT_PROGBITS &&
               section_headers[symbol->st_shndx].sh_flags == (SHF_ALLOC | SHF_WRITE)) {
        return 'D';
    } else if (section_headers[symbol->st_shndx].sh_type == SHT_PROGBITS &&
               section_headers[symbol->st_shndx].sh_flags == (SHF_ALLOC | SHF_EXECINSTR)) {
        return 'T';
    } else {
        return '?';
    }
}

Elf64_Shdr *read_symbol_table(FILE *file, Elf64_Ehdr *header, Elf64_Shdr *section_headers)
{
    Elf64_Shdr *symtab = NULL;
    for (int i = 0; i < header->e_shnum; i++)
    {
        if (section_headers[i].sh_type == SHT_SYMTAB)
        {
            symtab = &section_headers[i];
            break ;
        }
    }
    if (!symtab)
    {
        printf("Error: no symbol table found\n");
        return (NULL);
    }
    Elf64_Shdr *strtab = &section_headers[symtab->sh_link];
    Elf64_Sym *symbols = malloc(symtab->sh_size);
    fseek(file, symtab->sh_offset, SEEK_SET);
    fread(symbols, symtab->sh_size, 1, file);
    char *strtab_data = malloc(strtab->sh_size);
    fseek(file, strtab->sh_offset, SEEK_SET);
    fread(strtab_data, strtab->sh_size, 1, file);
    for (long unsigned int i = 0; i < symtab->sh_size / sizeof(Elf64_Sym); i++)
    {
        printf("%016lx %c %s\n", symbols[i].st_value, get_symbol_type(&symbols[i], section_headers), strtab_data + symbols[i].st_name);
    }
    return (symtab);
}

void    ft_nm(FILE *file)
{
    Elf64_Ehdr *header = read_elf_header(file);
    if (header->e_ident[EI_MAG0] != ELFMAG0 || header->e_ident[EI_MAG1] != ELFMAG1 || header->e_ident[EI_MAG2] != ELFMAG2 || header->e_ident[EI_MAG3] != ELFMAG3)
    {
        printf("Error: not an ELF file\n");
        return ;
    }
    Elf64_Shdr *section_headers = read_section_headers(file, header);
    read_symbol_table(file, header, section_headers);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: ft_nm <file>\n");
        return (1);
    }
    FILE *file = fopen(argv[1], "r");
    if (!file)
    {
        printf("Error: could not open file\n");
        return (1);
    }
    ft_nm(file);
    return (0);
}