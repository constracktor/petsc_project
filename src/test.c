#include <stdio.h>

#define MAX_LINE_LENGTH 1000

int main() {
    FILE    *textfile;
    char    line[MAX_LINE_LENGTH];
    double value;

    textfile = fopen("data/training/training_input.txt", "r");
    if(textfile == NULL)
        return 1;

    while(fgets(&line, MAX_LINE_LENGTH, textfile)){
        printf(line);
        fscanf(testfile,"0.7f",&value);
        printf("%0.7f \n", value);
    }

    fclose(textfile);
    return 0;
}
