#include <malloc.h>

int print_message(const char *file, const float *matrix, const int n);

int gauss(float *matrix, const int n);

int print_solution(const char *file, const float *matrix, const int n);

void swap_row(float *matrix, const int n, const int row, const int i);

//void print_matrix(const float *matrix, const int n);

int main(const int argc, const char **argv)
{
    FILE *input = fopen(argv[1], "r");
    if (input == NULL)
    {
        printf("input file name is incorrect\n");
        return 1;
    }
    int n;
    fscanf(input, "%i", &n);
    float *matrix = malloc(n * (n + 1) * sizeof(float));
    if (matrix == NULL)
    {
        printf("failed to allocate memory for matrix\n");
        return 2;
    }
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n + 1; j++)
        {
            fscanf(input, "%f", &matrix[i * (n + 1) + j]);
        }
    }
    fclose(input);
    int res;
    if (gauss(matrix, n))
    {
        res = print_solution(argv[2], matrix, n);
    }
    else
    {
        res = print_message(argv[2], matrix, n);
    }
//    printf("end\n");
//    print_matrix(matrix, n);
    free(matrix);
    return res;
}

int gauss(float *matrix, const int n)
{
    int col = 0, row = 0;
    for (; col < n; col++)
    {
        if (matrix[row * (n + 1) + col] == 0)
        {
            int i = row + 1;
            while (i < n && matrix[i * (n + 1) + col] == 0)
            {
                i++;
            }
            if (i == n)
            {
                continue;
            }
            swap_row(matrix, n, row, i);
        }
        for (int i = 0; i < n; i++)
        {
            if (i != row)
            {
                const float factor_up = matrix[i * (n + 1) + col];
                for (int j = 0; j < n + 1; j++)
                {
                    matrix[i * (n + 1) + j] *= matrix[row * (n + 1) + col];
                    matrix[i * (n + 1) + j] -= factor_up * matrix[row * (n + 1) + j];
                }
            }
        }
        row++;
    }
//    if (col != row)
//    {
//        return 0;
//    }
//    return 1;
    return col != row ? 0 : 1;
}

void swap_row(float *matrix, const int n, const int row, const int i)
{
    for (int k = 0; k < n + 1; k++)
    {
        float temp = matrix[row * (n + 1) + k];
        matrix[row * (n + 1) + k] = matrix[i * (n + 1) + k];
        matrix[i * (n + 1) + k] = temp;
    }
}

int print_solution(const char *file, const float *matrix, const int n)
{
    FILE *output = fopen(file, "w");
    if (output == NULL)
    {
        printf("output file name is incorrect\n");
        return 1;
    }
    for (int i = 0; i < n; i++)
    {
        fprintf(output, "%g\n", matrix[i * (n + 1) + n] / matrix[i * (n + 1) + i]);
    }
    fclose(output);
    return 0;
}

int print_message(const char *file, const float *matrix, const int n)
{
    FILE *output = fopen(file, "w");
    if (output == NULL)
    {
        printf("output file name is incorrect\n");
        return 1;
    }
    for (int i = n - 1; i >= 0; i--)
    {
        if (matrix[i * (n + 1) + n] != 0)
        {
            int bool = 1;
            for (int j = 0; j < n; j++)
            {
                if (matrix[i * (n + 1) + j] != 0)
                {
                    bool = 0;
                    break;
                }
            }
            if (bool)
            {
                fprintf(output, "no solution\n");
                fclose(output);
                return 0;
            }
        }
    }
    fprintf(output, "many solutions\n");
    fclose(output);
    return 0;
}

//void print_matrix(const float *matrix, const int n)
//{
//    for (int i = 0; i < n; i++)
//    {
//        for (int j = 0; j < n + 1; j++)
//        {
//            printf("%f ", matrix[i * (n + 1) + j]);
//        }
//        printf("\n");
//    }
//}




