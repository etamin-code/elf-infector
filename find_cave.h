//
// Created by koleksandr on 11.01.22.
//

#ifndef ELF_INFECTOR_FIND_CAVE_H
#define ELF_INFECTOR_FIND_CAVE_H

typedef struct {
    long file_size;
    int size;
    int start;
    int end;
    int bytes_written;
} code_cave_t;

int find_code_cave(char *bin_path, code_cave_t *cc);

#endif //ELF_INFECTOR_FIND_CAVE_H
