#include <stdio.h>

int main() {
    FILE    *training_input_file;
    FILE    *training_output_file;
    FILE    *test_input_file;
    FILE    *test_output_file;

    float   value;
    const int length_training = 100 * 1000;
    const int length_test = 5 * 1000;

    float   training_input[length_training];
    float   training_output[length_training];
    float   test_input[length_test];
    float   test_output[length_test];

    training_input_file = fopen("data/training/training_input.txt", "r");
    training_output_file = fopen("data/training/training_output.txt", "r");
    test_input_file = fopen("data/test/test_input_3.txt", "r");
    test_output_file = fopen("data/test/test_output_3.txt", "r");

    if(training_input_file == NULL || training_output_file == NULL || test_input_file == NULL || test_output_file)
    {
      printf("return 1");
      return 1;
    }
    // load training data
    for(int i = 0; i < length_training; i++)
    {
      fscanf(training_input_file,"%f",&value);
      training_input[i] = value;
      fscanf(training_output_file,"%f",&value);
      training_output[i] = value;
    }
    // load test data
    for(int i = 0; i < length_test; i++)
    {
      fscanf(test_input_file,"%f",&value);
      test_input[i] = value;
      fscanf(test_output_file,"%f",&value);
      test_output[i] = value;
    }

    fclose(training_input_file);
    fclose(training_output_file);
    fclose(test_input_file);
    fclose(test_output_file);

    for(int i = 0; i < length_training; i++)
    {
      printf("index %d: input = %f output = %f\n", i, training_input[i], training_output[i]);
    }
    for(int i = 0; i < length_test; i++)
    {
      printf("index %d: input = %f output = %f\n", i, test_input[i], test_output[i]);
    }

    printf("return 0");
    return 0;
}
