#include <stdio.h>
#define MAX_LINE_LENGTH 1000

int main() {
    FILE    *textfile;
    char    line[MAX_LINE_LENGTH];
    float value;
    int length_training = 1000;

    textfile = fopen("data/training/training_input.txt", "r");
    if(textfile == NULL)
        return 1;

    for(int i = 0, i < length_training, i++)
    {
      fscanf(textfile,"%f",&value);
      printf("index %d: %f \n", i, value);
    }

    fclose(textfile);
    return 0;
}
