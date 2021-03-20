#include <malloc.h>
#include <math.h>

int N;

void down_triangular_view(float (*matrix)[]);

float round_float(const float f);

void swap_row(float *a, float *b);

int main(const int argc, const char **argv)
{
    if (argc < 3)
    {
        printf("Expected tree arguments, actual: %i\n", argc);
        return 0;
    }
    FILE *input = fopen(argv[1], "r");
    fscanf(input, "%i", &N);
    float(*matrix)[N + 1] = malloc(N * (N + 1) * sizeof(float));
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N + 1; j++)
        {
            fscanf(input, "%f", &matrix[i][j]);
        }
    }
    fclose(input);
    down_triangular_view(matrix);
//    for (int i = 0; i < N; i++)
//    {
//        for (int j = 0; j < N + 1; j++)
//        {
//            printf("%g ", matrix[i][j]);
//        }
//        printf("\n");
//    }
    FILE *output = fopen(argv[2], "w");
    float *value = malloc(N * sizeof(float));
    for (int i = N - 1; i >= 0; i--)
    {
        float sum = round_float(matrix[i][N]);
        for (int j = i + 1; j < N; j++)
        {
            sum -= value[j] * round_float(matrix[i][j]);
        }
        if (round_float(matrix[i][i]) == 0)
        {
            if (sum == 0)
            {
                fprintf(output, "many solutions\n");
            }
            else
            {
                fprintf(output, "no solution\n");
            }
            fclose(output);
            return 0;
        }
        value[i] = sum / matrix[i][i];
    }
    for (int i = 0; i < N; i++)
    {
        fprintf(output, "%g\n", value[i]);
    }
    fclose(output);
    return 0;
}

void down_triangular_view(float (*matrix)[N + 1])
{
    int row = 0;
    for (int j = 0; j < N; j++)
    {
        if (round_float(matrix[row][j]) == 0)
        {
            int i = row + 1;
            while (i < N && round_float(matrix[i][j]) == 0)
            {
                i++;
            }
            if (i < N)
            {
                swap_row(matrix[row], matrix[i]);
            }
            else
            {
                continue;
            }
        }
        for (int i = row + 1; i < N; i++)
        {
            float factor = matrix[i][j] / matrix[row][j];
            for (int k = j; k < N + 1; k++)
            {
                matrix[i][k] -= matrix[row][k] * factor;
            }
        }
        row++;
    }
}

void swap_row(float *a, float *b)
{
    float temp;
    for (int i = 0; i < N + 1; i++)
    {
        temp = a[i];
        a[i] = b[i];
        b[i] = temp;
    }
}

float round_float(const float f)
{
    return roundf(f * pow(10, 6)) / pow(10, 6);
}


