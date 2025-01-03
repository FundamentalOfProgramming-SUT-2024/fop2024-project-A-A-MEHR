#include <stdio.h>

int main() {
    FILE *file;
    char ch;
    file = fopen("example.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    while ((ch = fgetc(file)) != EOF) {
        if (!ch)
            putchar(' ');
        else {
            putchar(ch);
        }
    }

    //free_map();
    fclose(file);

    return 0;
}