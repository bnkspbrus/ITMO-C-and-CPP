#include <malloc.h>
#include <math.h>

int N;

void down_triangular_view(float (*matrix)[]);

float round_float(const float f);

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
    //    for (int i = 0; i < N; i++) {
    //        for (int j = 0; j < N + 1; j++) {
    //            printf("%g ", matrix[i][j]);
    //        }
    //        printf("\N");
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
            } else
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
    int m = 0;
    for (int j = 0; j < N + 1; j++)
    {
        int i = m;
        while (i < N && round_float(matrix[i][j]) == 0 ||
               (j > 0 && round_float(matrix[i][j - 1]) != 0))
        {
            i++;
        }
        for (int k = i + 1; k < N; k++)
        {
            float factor = matrix[k][j] / matrix[i][j];
            for (int g = j; g < N + 1; g++)
            {
                matrix[k][g] -= matrix[i][g] * factor;
            }
        }
        if (i < N)
        {
            m++;
        }
    }
}

float round_float(const float f)
{
    return roundf(f * pow(10, 6)) / pow(10, 6);
}
