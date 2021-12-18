#include <stdio.h>
#include <dirent.h>
#include <iostream>
#include<string>
#include <cstring>


int main(int argc, const char**argv) {
    struct dirent *entry = nullptr;
    DIR *dp = nullptr;

    std::string dir_name = "files_renames";
    dp = opendir(dir_name.c_str());
    char new_char = 'h';
    if (dp != nullptr) {
        int i = 1;
        while ((entry = readdir(dp))) {
            printf("%s\n", entry->d_name);

            std::string filename = entry->d_name;
            if (filename.length() < 4)
                continue;
            std::string oldname_str = "./" + dir_name + "/" + filename;

            std::string newname_str = "./" + dir_name + "/file_new0.txt";
            char newname[newname_str.length() + 1];
            for (int i = 0; i < newname_str.length(); i++)
                newname[i] = newname_str[i];
            newname[newname_str.length()] = '\0';

            newname[newname_str.length() - 5] = new_char;
            new_char++;

            if (rename(oldname_str.c_str(), newname) != 0)
                perror("Error renaming file");
            else
                std::cout << "File renamed successfully";
        }

    }

    closedir(dp);
    return 0;
}