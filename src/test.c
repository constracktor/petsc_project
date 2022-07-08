#include <stdio.h>
typedef float real;

int main() {
    FILE    *training_input_file;
    FILE    *training_output_file;
    FILE    *test_input_file;
    FILE    *test_output_file;

    real   value;
    const int length_training = 100 * 1000;
    const int length_test = 5 * 1000;

    real   training_input[length_training];
    real   training_output[length_training];
    real   test_input[length_test];
    real   test_output[length_test];

    training_input_file = fopen("data/training/training_input.txt", "r");
    training_output_file = fopen("data/training/training_output.txt", "r");
    test_input_file = fopen("data/test/test_input_3.txt", "r");
    test_output_file = fopen("data/test/test_output_3.txt", "r");

    if(training_input_file == NULL || training_output_file == NULL || test_input_file == NULL || test_output_file == NULL)
    {
      printf("return 1\n");
      return 1;
    }
    if(real == float)
    {
      return 1;
    }
    // load training data
    for(int i = 0; i < length_training; i++)
    {
      fscanf(training_input_file,"%lf",&value);
      training_input[i] = value;
      fscanf(training_output_file,"%lf",&value);
      training_output[i] = value;
    }
    // load test data
    for(int i = 0; i < length_test; i++)
    {
      fscanf(test_input_file,"%lf",&value);
      test_input[i] = value;
      fscanf(test_output_file,"%lf",&value);
      test_output[i] = value;
    }

    fclose(training_input_file);
    fclose(training_output_file);
    fclose(test_input_file);
    fclose(test_output_file);

    for(int i = 0; i < length_training; i++)
    {
      printf("index %d: input = %lf output = %lf\n", i, training_input[i], training_output[i]);
    }
    for(int i = 0; i < length_test; i++)
    {
      printf("index %d: input = %lf output = %lf\n", i, test_input[i], test_output[i]);
    }

    printf("return 0\n");
    return 0;
}
