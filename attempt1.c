#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void check_open_file(FILE *file, const char *name);

void check_count_args(const int argc);

typedef struct
{
    char *sur, *name, *pat;
    long long tel;
} Hum_data;

void free_fields(Hum_data data);

int partition(Hum_data *base, int left, int right, int (*compare)(Hum_data, Hum_data));

char *create_copy(const char *source);

void check_alloc_mem(const Hum_data *base);

Hum_data *ensure_capacity(Hum_data *base, int *size);

void init_data(Hum_data *data, const char *sur, const char *name, const char *pat, const long long tel);

int def_compare(const Hum_data data1, const Hum_data data2);

void q_sort(Hum_data *base, int left, int right, int (*compare)(Hum_data, Hum_data));

int main(const int argc, const char **argv)
{
    check_count_args(argc);
    FILE *input = fopen(argv[1], "r");
    check_open_file(input, argv[1]);
    char sur[81], name[81], pat[81];
    long long tel;
    int size = 10;
    Hum_data *base = malloc(size * sizeof(Hum_data));
    check_alloc_mem(base);
    int num = 0;
    while (fscanf(input, "%s %s %s %lli", sur, name, pat, &tel) == 4)
    {
        if (num == size)
        {
            base = ensure_capacity(base, &size);
        }
//        printf("%s %s %s %lli\n", sur, name, pat, tel);
        init_data(&base[num++], sur, name, pat, tel);
    }
    fclose(input);
    q_sort(base, 0, num - 1, def_compare);
    FILE *output = fopen(argv[2], "w");
    for (int i = 0; i < num; i++)
    {
        Hum_data data = base[i];
        fprintf(output, "%s %s %s %lli\n", data.sur, data.name, data.pat, data.tel);
        free_fields(base[i]);
    }
    free(base);
    fclose(output);
    return 0;
}

void free_fields(Hum_data data)
{
    free(data.sur);
    free(data.name);
    free(data.pat);
}

void init_data(Hum_data *data, const char *sur, const char *name, const char *pat, const long long tel)
{
    data->tel = tel;
    data->sur = create_copy(sur);
    data->name = create_copy(name);
    data->pat = create_copy(pat);
}

char *create_copy(const char *source)
{
//    printf("len = %lu\n", strlen(source));
    char *dist = malloc(strlen(source) + 1);
    strcpy(dist, source);
    return dist;
}

void q_sort(Hum_data *base, int left, int right, int (*compare)(Hum_data, Hum_data))
// left = 0 right = num - 1
{
    if (left < right)
    {
        int point = partition(base, left, right, compare);
        q_sort(base, left, point, compare);
        q_sort(base, point + 1, right, compare);
    }
}

void swap(Hum_data *data1, Hum_data *data2)
{
    Hum_data temp = *data1;
    *data1 = *data2;
    *data2 = temp;
}

int partition(Hum_data *base, int left, int right, int (*compare)(Hum_data, Hum_data))
{
    Hum_data mid = base[(left + right) / 2];
    int i = left, j = right;
    while (i <= j)
    {
        while (compare(base[i], mid) < 0)
        {
            i++;
        }
        while (compare(base[j], mid) > 0)
        {
            j--;
        }
        if (i >= j)
        {
            break;
        }
        swap(&base[i++], &base[j--]);
    }
    return j;
}

Hum_data *ensure_capacity(Hum_data *base, int *size)
{
    (*size) *= 2;
    base = realloc(base, *size * sizeof(Hum_data));
    check_alloc_mem(base);
    return base;
}

int def_compare(const Hum_data data1, const Hum_data data2)
{
//    printf("comp sur = %i\n", strcmp(data1.sur, data2.sur));
    if (strcmp(data1.sur, data2.sur))
    {
        return strcmp(data1.sur, data2.sur);
    }
//    printf("comp name = %i\n", strcmp(data1.name, data2.name));
    if (strcmp(data1.name, data2.name))
    {
        return strcmp(data1.name, data2.name);
    }
//    printf("comp pat = %i\n", strcmp(data1.pat, data2.pat));
    if (strcmp(data1.pat, data2.pat))
    {
        return strcmp(data1.pat, data2.pat);
    }
    return (data1.tel - data2.tel) % 2;
}

void check_alloc_mem(const Hum_data *base)
{
    if (base == NULL)
    {
        printf("failed to allocate memory for data\n");
        exit(2);
    }
}

void check_open_file(FILE *file, const char *name)
{
    if (file == NULL)
    {
        printf("Error opening file %s\n", name);
        exit(1);
    }
}

void check_count_args(const int argc)
{
    if (argc != 3)
    {
        printf("Wrong number of arguments\n");
        exit(1);
    }
}
