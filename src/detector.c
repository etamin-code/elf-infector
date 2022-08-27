#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <elf.h>
#include <sys/mman.h>

int main (int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "[!] Usage: %s <target> <payload>\n", argv[0]);
        exit(1);
    }

    int 		t_fd, t_size;
    struct 		stat _info;
    void		*t_addr;

    // open target file
    if ((t_fd = open(argv[1], O_APPEND | O_RDWR, 0)) < 0) {
        perror("[!] open:");
        exit(1);
    }

    // get target file disk size
    fstat (t_fd, &_info);
    t_size = _info.st_size;

    // map target file to memory
    if ((t_addr = mmap(0, t_size, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_SHARED, t_fd, 0)) == MAP_FAILED) {
        perror("[!] mmap:");
        exit(1);
    }

    Elf64_Ehdr	*t_ehdr;

    // get ELF header
    t_ehdr = (Elf64_Ehdr *) t_addr;


    /************************************
| (a) open and map payload file     |
************************************/

    int		p_fd, p_size;
    void		*p_addr;

    // open payload file
    if ((p_fd = open(argv[2], O_APPEND | O_RDWR, 0)) < 0) {
        perror("[!] open:");
        exit(1);
    }

    // get payload file disk size
    fstat (p_fd, &_info);
    p_size = _info.st_size;

    // map payload file to memory
    if ((p_addr = mmap(0, p_size, PROT_READ| PROT_WRITE| PROT_EXEC, MAP_SHARED, p_fd, 0)) == MAP_FAILED) {
        perror("[!] mmap:");
        exit(1);
    }


    int		i, sc_size = p_size;


    Elf64_Addr ep = t_ehdr->e_entry;
    int is_injected=1;
    char *t_ch = (char*)t_addr + ep, *p_ch = (char*)p_addr;
    for (int i=0; i<10;i++) {
        if (*t_ch!=*p_ch) {
            is_injected = 0;
            break;
        }
        p_ch++;
        t_ch++;
    }
    if (is_injected)
        printf("1");
    else
        printf("0");

    close(t_fd);
    close(p_fd);
    exit(0);
}
