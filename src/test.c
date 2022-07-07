#include <stdio.h>

int main() {
    FILE    *training_input_file;
    FILE    *training_output_file;

    float   value;
    const int length_training = 100 * 1000;

    float   training_input[length_training];
    float   training_output[length_training];

    training_input_file = fopen("data/training/training_input.txt", "r");
    training_output_file = fopen("data/training/training_output.txt", "r");

    if(training_input_file == NULL || training_output_file == NULL)
    {
      return 1;
    }

    for(int i = 0; i < length_training; i++)
    {
      fscanf(training_input_file,"%f",&value);
      training_input[i] = value;
      fscanf(training_output_file,"%f",&value);
      training_output[i] = value;
    }

    fclose(training_input_file);
    fclose(training_output_file);

    for(int i = 0; i < length_training; i++)
    {
      printf("index %d: input = %f output = %f\n", i, training_input[i], training_output[i]);
    }


    return 0;
}
