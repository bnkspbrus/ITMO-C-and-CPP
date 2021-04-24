#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

unsigned decode_utf8(FILE *input, int *bom, int *start, int *size, int *read_eof);

void add_bom_utf16(int be, FILE *output);

void add_bom_utf8(FILE *output);

void add_bom_utf32(int be, FILE *output);

unsigned decode_utf32(FILE *input, int be, int *read_eof);

unsigned decode_utf16(FILE *input, int *bom, int *start, int size, int be, int *read_eof);

void encode_utf32(FILE *output, unsigned num, int be);

void encode_utf8(FILE *output, unsigned num);

void check_open_file(FILE *file, const char *name);

void encode_utf16(FILE *output, unsigned num, int be /* little endian */);

void file_copy(const int size, const int *bom, FILE *input, FILE *output, const int start);

int input_file_encoding(const int size, const int *bom);

int main(const int argc, const char **argv)
{
    int c;
    FILE *input = fopen(argv[1], "rb");
    check_open_file(input, argv[1]);
    int bom[4];
    int size = 0;
    while (size < 4 && (c = fgetc(input)) != EOF)
    {
        bom[size++] = c;
    }
//    printf("size = %d\n", size);
    int this = input_file_encoding(size, bom);
    int that;
    if (sscanf(argv[3], "%i", &that) == 0 || that < 0 || that > 5)
    {
        printf("Output file number is incorrect\n");
        return 1;
    }
//    printf("this = %i that = %i\n", this, that);
    FILE *output = fopen(argv[2], "wb");
    check_open_file(output, argv[2]);
    if (this == that || this == 0 && that == 1 || this == 1 && that == 0)
    {
        if (this == 1 && that == 0)
        {
            file_copy(size, bom, input, output, 3);
        }
        else
        {
            if (this == 0 && that == 1)
            { // add bom
                add_bom_utf8(output);
            }
            file_copy(size, bom, input, output, 0);
        }
    }
    else if ((this == 0 || this == 1) && (that == 2 || that == 3))
    {
        add_bom_utf16(that - 2, output);
        int start = 0;
        if (this == 1)
        {
            start = 3;
        }
        size -= start;
        while (1) // true
        {
            int read_eof = 0;
            unsigned num = decode_utf8(input, bom, &start, &size, &read_eof);
            if (read_eof)
            {
                break;
            }
            encode_utf16(output, num, that - 2);
        }
    }
    else if ((this == 2 || this == 3) && (that == 0 || that == 1))
    {
        if (that == 1)
        {
            add_bom_utf8(output);
        }
        int start = 2;
        while (1)
        {
            int read_eof = 0;
            unsigned num = decode_utf16(input, bom, &start, size, this - 2, &read_eof);
            if (read_eof)
            {
                break;
            }
            encode_utf8(output, num);
        }
    }
    else if ((this == 0 || this == 1) && (that == 4 || that == 5))
    {
        add_bom_utf32(that - 4, output);
        int start = 0;
        if (this == 1)
        {
            start = 3;
        }
        size -= start;
        while (1)
        {
            int read_eof = 0;
            unsigned num = decode_utf8(input, bom, &start, &size, &read_eof);
            if (read_eof)
            {
                break;
            }
            encode_utf32(output, num, that - 4);
        }
    }
    else if ((this == 4 || this == 5) && (that == 0 || that == 1))
    {
        if (that == 1)
        {
            add_bom_utf8(output);
        }
        while (1)
        {
            int read_eof = 0;
            unsigned num = decode_utf32(input, this - 4, &read_eof);
            if (read_eof)
            {
                break;
            }
            encode_utf8(output, num);
        }
    }
    else if ((this == 2 || this == 3) && (that == 4 || that == 5))
    {
        add_bom_utf32(that - 4, output);
        int start = 2;
        while (1)
        {
            int read_eof = 0;
            unsigned num = decode_utf16(input, bom, &start, size, this - 2, &read_eof);
            if (read_eof)
            {
                break;
            }
            encode_utf32(output, num, that - 4);
        }
    }
    else if ((this == 4 || this == 5) && (that == 2 || that == 3))
    {
        add_bom_utf16(that - 2, output);
        while (1)
        {
            int read_eof = 0;
            unsigned num = decode_utf32(input, this - 4, &read_eof);
            if (read_eof)
            {
                break;
            }
            encode_utf16(output, num, that - 2);
        }
    }
    else if (this == 2 && that == 3 || this == 3 && that == 2)
    {
        add_bom_utf16(that - 2, output);
        int start = 2;
        while (1)
        {
            int read_eof = 0;
            unsigned num = decode_utf16(input, bom, &start, size, this - 2, &read_eof);
            if (read_eof)
            {
                break;
            }
            encode_utf16(output, num, that - 2);
        }
    }
    else if (this == 4 && that == 5 || this == 5 && that == 4)
    {
        add_bom_utf32(that - 4, output);
        while (1)
        {
            int read_eof = 0;
            unsigned num = decode_utf32(input, this - 4, &read_eof);
            if (read_eof)
            {
                break;
            }
            encode_utf32(output, num, that - 4);
        }
    }
    fclose(input);
    fclose(output);
    return 0;
}

void add_bom_utf16(int be, FILE *output)
{
    if (be)
    {
        fputc(0xFE, output);
        fputc(0xFF, output);
    }
    else
    {
        fputc(0xFF, output);
        fputc(0xFE, output);
    }
}

void add_double_zero(FILE *output)
{
    fputc(0x00, output);
    fputc(0x00, output);
}

void add_bom_utf32(int be, FILE *output)
{
    if (be)
    {
        add_double_zero(output);
        add_bom_utf16(1, output);
    }
    else
    {
        add_bom_utf16(0, output);
        add_double_zero(output);
    }
}


void add_bom_utf8(FILE *output)
{
    fputc(0xEF, output);
    fputc(0xBB, output);
    fputc(0xBF, output);
}

void encode_utf32(FILE *output, unsigned num, int be)
{
    for (int i = 0; i < 4; i++)
    {
        unsigned out;
        if (be)
        {
            out = (num >> ((3 - i) * 8)) & ((1 << 8) - 1);
        }
        else
        {
            out = (num >> (i * 8)) & ((1 << 8) - 1);
        }
        fputc(out, output);
    }
}

unsigned decode_utf32(FILE *input, int be, int *read_eof) // decoding
{
    unsigned res = 0;
    for (int i = 0; i < 4; i++)
    {
        int c;
        if ((c = fgetc(input)) == EOF)
        {
            *read_eof = 1;
            return EOF;
        }
        if (be)
        {
            res = (res << 8) + c;
        }
        else
        {
            res += c << (i * 8);
        }
    }
    return res;
}

unsigned decode_utf16(FILE *input, int *bom, int *start, int size, int be, int *read_eof)
{
    int part1, part2;
    if (*start < size)
    {
        part1 = bom[*start];
        part2 = bom[*start + 1];
        *start = *start + 2;
    }
    else
    {
        part1 = fgetc(input); // little endian
        part2 = fgetc(input);
    }
    if (part1 == EOF || part2 == EOF)
    {
        *read_eof = 1;
        return EOF;
    }
    if (be)
    {
        int tmp = part1;
        part1 = part2;
        part2 = tmp;
    }
    part1 += (part2 << 8);
    if (0xD800 <= part1 && part1 <= 0xDBFF)
    {
        if (be)
        {
            part2 = ((bom[2] = fgetc(input)) << 8) + (bom[3] = fgetc(input));
        }
        else
        {
            part2 = (bom[2] = fgetc(input)) + ((bom[3] = fgetc(input)) << 8);
        }
        if (0xDC00 <= part2 && part2 <= 0xDFFF)
        {
            unsigned res = 0x10000;
            res += (part1 & 0x03FF) << 10;
            res += (part2 & 0x03FF);
            return res;
        }
        else
        {
            *start = 2;
        }
    }
    return part1;
}

void encode_utf16(FILE *output, unsigned num, int be)
{
    if (num <= 0x10000)
    {
        if (be)
        {
            fputc((num >> 8) & ((1 << 8) - 1), output);
            fputc(num & ((1 << 8) - 1), output);
        }
        else
        {
            fputc(num & ((1 << 8) - 1), output);
            fputc((num >> 8) & ((1 << 8) - 1), output);
        }
        return;
    }
    num -= 0x10000;
    unsigned part1 = 0xD800 | (num >> 10);
    unsigned part2 = 0xDC00 | (num & 0x3FF);
    encode_utf16(output, part1, be);
    encode_utf16(output, part2, be);
}

unsigned decode_utf8(FILE *input, int *bom, int *start, int *size, int *read_eof)
{
    int c;
    if (*size) // --> have bytes
    {
        c = bom[*start];
        *start = (*start + 1) % 4;
        *size = *size - 1;
    }
    else
    {
        c = fgetc(input);
    }
    if (c == EOF)
    {
        *read_eof = 1;
        return EOF;
    }
    if (!(c & (1 << 7)))
    {
        return c;
    }
    int num_bytes = 0;
    while (num_bytes < 4 && (c & (1 << (6 - num_bytes))))
    {
        num_bytes++;
    }
    if (num_bytes == 0 || num_bytes == 4)
    {
        return 0xDC80 + (c & ((1 << 7) - 1));
    }
    for (int i = 0; i < num_bytes; i++)
    {
        int g;
        if (i >= *size)
        {
            bom[(*start + i) % 4] = fgetc(input);
            *size = *size + 1;
            if (bom[(*start + i) % 4] == EOF)
            {
                return 0xDC80 + (c & ((1 << 7) - 1));
            }
        }
        g = bom[(*start + i) % 4];
        if ((bom[(*start + i) % 4] & 0xC0) != 0x80) // "10" in begin
        {
            return 0xDC80 + (c & ((1 << 7) - 1));
        }
    }
    unsigned res = 0;
    for (int i = 0; i < num_bytes; i++)
    {
        res = (res << 6) + (bom[*start] & ((1 << 6) - 1));
        *start = (*start + 1) % 4;
        *size = *size - 1;
    }
    res += (c & ((1 << (6 - num_bytes)) - 1)) << (num_bytes * 6);
    return res;
}

void encode_utf8(FILE *output, unsigned num)
{
    if (0xDC80 <= num && num <= 0xDCFF)
    {
        num += (1 << 7) - 0xDC80;
        fputc(num, output);
        return;
    }
    if (num <= 0x0000007F) // 1 byte
    {
        fputc(num, output);
        return;
    }
    int i;
    unsigned part1;
    if (num <= 0x000007FF) // 2 byte
    {
        // 3 -- 11 in bin
        part1 = (num >> 6) & ((1 << 5) - 1);
        part1 += 3 << 6;
        fputc(part1, output);
        i = 0;
    }
    else if (num <= 0x0000FFFF) // 3 byte
    {
        // 7 -- 111 in bin
        part1 = (num >> 12) & ((1 << 4) - 1);
        part1 += 7 << 5;
        fputc(part1, output);
        i = 1;
    }
    else // 4 num_bytes
    {
        // 15 -- 1111 in bin
        part1 = (num >> 18) & ((1 << 3) - 1);
        part1 += 15 << 4;
        fputc(part1, output);
        i = 2;
    }
    for (; i >= 0; i--)
    {
        unsigned out = ((num >> (6 * i)) & ((1 << 6) - 1)) + (1 << 7);
        fputc(out, output);
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

void file_copy(const int size, const int *bom, FILE *input, FILE *output, const int start)
{
    for (int i = start; i < size; i++)
    {
        fputc(bom[i], output);
    }
    int c;
    while ((c = fgetc(input)) != EOF)
    {
        fputc(c, output);
    }
}

int input_file_encoding(const int size, const int *bom)
{
    switch (size)
    {
    case 4:
        if (bom[0] == 0x00 && bom[1] == 0x00 && bom[2] == 0xFE && bom[3] == 0xFF)
        {
            return 5;
        }
        if (bom[0] == 0xFF && bom[1] == 0xFE && bom[2] == 0x00 && bom[3] == 0x00)
        {
            return 4;
        }
    case 3:
        if (bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF)
        {
            return 1;
        }
    case 2:
        if (bom[0] == 0xFE && bom[1] == 0xFF)
        {
            return 3;
        }
        if (bom[0] == 0xFF && bom[1] == 0xFE)
        {
            return 2;
        }
    default:
        return 0;
    }
}

