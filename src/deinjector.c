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

    // open payload file
    if ((p_fd = open(argv[2], O_APPEND | O_RDWR, 0)) < 0) {
        perror("[!] open:");
        exit(1);
    }

    // get payload file disk size
    fstat (p_fd, &_info);
    p_size = _info.st_size;

    // map payload file to memory
    if (mmap(0, p_size, PROT_READ| PROT_WRITE| PROT_EXEC, MAP_SHARED, p_fd, 0) == MAP_FAILED) {
        perror("[!] mmap:");
        exit(1);
    }



    unsigned char bytes[4];
    int bytes_to_oep=0;
    Elf64_Addr end_of_payload = t_ehdr->e_entry + p_size + 2, real_ep;
    char *t_ch = (char*)t_addr + end_of_payload;
    for (int i=0; i<4;i++) {
        bytes[i] = *t_ch;
        t_ch++;
    }
    for (int j = 3; j >=0; j--) {
        bytes_to_oep += bytes[j] << (j*8);
    }
    real_ep = end_of_payload +  bytes_to_oep;

//    printf("%x\n", bytes_to_oep);
//    printf("%x\n", end_of_payload);
//    printf("%x\n", real_ep);

    t_ehdr->e_entry = (unsigned int) real_ep;

    close(t_fd);
    close(p_fd);
    exit(0);
}
