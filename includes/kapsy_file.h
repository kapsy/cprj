#include <stdio.h>
#include <stdlib.h>

// TODO: (Kapsy) A function that returns the file size so the implementer can malloc.

static char *
ReadEntireFile(char *Filename)
{
    char *FileMemory = 0;
    FILE *File = fopen(Filename, "r");

    if(File) {

        fseek(File, 0, SEEK_END);
        size_t Size = ftell(File);
        fseek(File, 0, SEEK_SET);

        FileMemory = (char *)malloc(Size);
        size_t Read = fread(FileMemory, sizeof(char), Size + 1, File);
        FileMemory[Size] = 0;
    }

    return(FileMemory);
}

