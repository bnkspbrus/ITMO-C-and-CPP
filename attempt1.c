#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

unsigned dec_utf8(FILE *input, unsigned *bom, int *start, int size, int *bool);

void add_bom_utf16(int that, FILE *output);

void add_bom_utf8(int that, FILE *output);

void add_bom_utf32(int that, FILE *output);

unsigned dec_utf32(FILE *input, int be, int *bool);

unsigned dec_utf16(FILE *input, unsigned *bom, int *start, int size, int be, int *bool);

void code_utf32(FILE *output, unsigned num, int be);

void code_utf8(FILE *output, unsigned num);

void check_file(FILE *file, const char *name);

void code_utf16(FILE *output, unsigned num, int be /* little endian */);

void file_copy(/*const int size_bom, */const int size, const unsigned *bom, FILE *input, FILE *output, const int start);

int input_file_encoding(const int size, const unsigned *buf);

int main(const int argc, const char **argv)
{
    unsigned c;
    FILE *input = fopen(argv[1], "rb");
    check_file(input, argv[1]);
    unsigned bom[4];
    int size = 0;
    while (size < 4 && (c = fgetc(input)) != EOF)
    {
        bom[size++] = c;
    }
//    printf("size = %d\n", size);
    int this = input_file_encoding(size, bom);
    int that;
    sscanf(argv[3], "%i", &that);
//    printf("this = %i that = %i\n", this, that);
    ////////
    FILE *output = fopen(argv[2], "wb");
    check_file(output, argv[2]);
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
                fputc(0xEF, output);
                fputc(0xBB, output);
                fputc(0xBF, output);
            }
            file_copy(size, bom, input, output, 0);
        }
    }
        ////////
    else if ((this == 0 || this == 1) && (that == 2 || that == 3))
    {
        add_bom_utf16(that, output);
        int start;
        if (this == 0)
        {
            start = 0;
        }
        else
        {
            start = 3;
        }
//      int *s = &start;
        while (1) // true
        {
            int bool = 0;
            unsigned num = dec_utf8(input, bom, &start, size, &bool);
            if (bool)
            {
                break;
            }
            code_utf16(output, num, that - 2);
        }
    }
    else if ((this == 2 || this == 3) && (that == 0 || that == 1))
    {
        add_bom_utf8(that, output);
        int start = 2;
        while (1)
        {
            int bool = 0;
            unsigned num = dec_utf16(input, bom, &start, size, this - 2, &bool);
            if (bool)
            {
                break;
            }
            code_utf8(output, num);
        }
    }
    else if ((this == 0 || this == 1) && (that == 4 || that == 5))
    {
        add_bom_utf32(that, output);
        int start = 0;
        if (this == 1)
        {
            start = 3;
        }
        while (1)
        {
            int bool = 0;
            unsigned num = dec_utf8(input, bom, &start, size, &bool);
            if (bool)
            {
                break;
            }
            code_utf32(output, num, that - 4);
        }
    }
    else if ((this == 4 || this == 5) && (that == 0 || that == 1))
    {
        add_bom_utf8(that, output);
        while (1)
        {
            int bool = 0;
            unsigned num = dec_utf32(input, this - 4, &bool);
            if (bool)
            {
                break;
            }
            code_utf8(output, num);
        }
    }
    else if ((this == 2 || this == 3) && (that == 4 || that == 5))
    {
        add_bom_utf32(that, output);
        int start = 2;
        while (1)
        {
            int bool = 0;
            unsigned num = dec_utf16(input, bom, &start, size, this - 2, &bool);
            if (bool)
            {
                break;
            }
            code_utf32(output, num, that - 4);
        }
    }
    else if ((this == 4 || this == 5) && (that == 2 || that == 3))
    {
        add_bom_utf16(that, output);
        while (1)
        {
            int bool = 0;
            unsigned num = dec_utf32(input, this - 4, &bool);
            if (bool)
            {
                break;
            }
            code_utf16(output, num, that - 2);
        }
    }
    else if (this == 2 && that == 3 || this == 3 && that == 2)
    {
        add_bom_utf16(that, output);
        int start = 2;
        while (1)
        {
            int bool = 0;
            unsigned num = dec_utf16(input, bom, &start, size, this - 2, &bool);
            if (bool)
            {
                break;
            }
            code_utf16(output, num, that - 2);
        }
    }
    else if (this == 4 && that == 5 || this == 5 && that == 4)
    {
        add_bom_utf32(that, output);
        while (1)
        {
            int bool = 0;
            unsigned num = dec_utf32(input, this - 4, &bool);
            if (bool)
            {
                break;
            }
            code_utf32(output, num, that - 4);
        }
    }
    fclose(input);
    fclose(output);
    return 0;
}

void add_bom_utf16(int that, FILE *output)
{
    if (that == 2)
    {
        fputc(0xFF, output);
        fputc(0xFE, output);
    }
    else
    {
        fputc(0xFE, output);
        fputc(0xFF, output);
    }
}

void add_bom_utf32(int that, FILE *output)
{
    if (that == 5)
    {
        fputc(0x00, output);
        fputc(0x00, output);
        fputc(0xFE, output);
        fputc(0xFF, output);
    }
    else
    {
        fputc(0xFF, output);
        fputc(0xFE, output);
        fputc(0x00, output);
        fputc(0x00, output);
    }
}

void add_bom_utf8(int that, FILE *output)
{
    if (that == 1)
    {
        fputc(0xEF, output);
        fputc(0xBB, output);
        fputc(0xBF, output);
    }
}

void code_utf32(FILE *output, unsigned num, int be)
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

unsigned dec_utf32(FILE *input, int be, int *bool) // decoding
{
    unsigned res = 0;
    for (int i = 0; i < 4; i++)
    {
        unsigned add = fgetc(input);
        if (add == EOF)
        {
            *bool = 1;
            return EOF;
        }
        if (be)
        {
            res = (res << 8) + add;
        }
        else
        {
            res += add << (i * 8);
        }
    }
    return res;
}

unsigned dec_utf16(FILE *input, unsigned *bom, int *start, int size, int be, int *bool)
{
    unsigned part1;
    unsigned part2;
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
        *bool = 1;
        return EOF;
    }
    if (be)
    {
        unsigned tmp = part1;
        part1 = part2;
        part2 = tmp;
    }
    if (0xD800 <= part1 && part1 <= 0xDBFF && 0xDC00 <= part2 && part2 <= 0xDFFF)
    {
        unsigned res = 0x10000;
        res += (part1 & 0x03FF) << 10;
        res += part2 & 0x03FF;
        return res;
    }
    return part1 + (part2 << 8);
}

void code_utf16(FILE *output, unsigned num, int be /* big endian */)  ///
{
    if (num <= 0x7FF || num >= 0xE000 && num <= 0xFFFD)
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
    if (be)
    {
        fputc(part2, output);
        fputc(part1, output);
    }
    else
    {
        fputc(part1, output);
        fputc(part2, output);
    }
}

unsigned dec_utf8(FILE *input, unsigned *bom, int *start, int size, int *bool)
{
    int c;
    if (*start < size)
    {
        c = bom[*start];
        *start = *start + 1;
    }
    else
    {
        c = fgetc(input);
    }
    if (c == EOF)
    {
        *bool = 1;
        return EOF;
    }
    if (!(c & (1 << 7)))
    {
        return c;
    }
    int res = 0;
    int i;
    for (i = 6; c & (1 << i); i--)
    {
        int g;
        if (*start < size)
        {
            g = bom[*start];
            *start = *start + 1;
        }
        else
        {
            g = fgetc(input);
        }
        res = (res << 6) + (g & ((1 << 6) - 1));
    }
    res += (c & ((1 << i) - 1)) << ((6 - i) * 6);
    return res;
}

void code_utf8(FILE *output, unsigned num)
{
    if (num <= 0x0000007F) // 1 byte
    {
        fputc(num, output);
        return;
    }
    if (num <= 0x000007FF) // 2 byte
    {
        // 6 -- 110 in bin
        unsigned part1 = (num >> 6) & ((1 << 5) - 1);
        unsigned part2 = num & ((1 << 6) - 1);
        part1 += 6 << 5;
        fputc(part1, output);
        part2 += 2 << 6; // 2 << 6   2 -- 10 in bin
        fputc(part2, output);
        return;
    }
    if (num <= 0x0000FFFF) // 3 byte
    {
        int part1 = (num >> 12) & ((1 << 4) - 1);
        part1 += 14 << 4;
        fputc(part1, output);
        for (int i = 1; i >= 0; i--)
        {
            int out = (num >> (6 * i)) & ((1 << 6) - 1);
            fputc(out, output);
        }
        return;
    }
    // 4 bytes
    int part1 = (num >> 18) & ((1 << 3) - 1);
    part1 += 30 << 3;
    fputc(part1, output);
    for (int i = 2; i >= 0; i--)
    {
        int out = (num >> (6 * i)) & ((1 << 6) - 1);
        fputc(out, output);
    }
    return;
}

void check_file(FILE *file, const char *name)
{
    if (file == NULL)
    {
        printf("Error opening file %s\n", name);
        exit(1);
    }
}

void
file_copy(/*const int size_bom, */const int size, const unsigned *bom, FILE *input, FILE *output, const int start)
{
    for (int i = start; i < size; i++)
    {
        fputc(bom[i], output);
    }
    unsigned c;
    while ((c = fgetc(input)) != EOF)
    {
        fputc(c, output);
    }
}

int /* char **/ input_file_encoding(const int size, const unsigned *buf)
{
    switch (size)
    {
        case 4:
            if (buf[0] == 0x00 && buf[1] == 0x00 && buf[2] == 0xFE && buf[3] == 0xFF)
            {
//                printf("utf-32be\n");
                return 5;//"5";
            }
            if (buf[0] == 0xFF && buf[1] == 0xFE && buf[2] == 0x00 && buf[3] == 0x00)
            {
//                printf("utf-32le\n");
                return 4;//"4";
            }
        case 3:
            if (buf[0] == 0xEF && buf[1] == 0xBB && buf[2] == 0xBF)
            {
//                printf("utf-8b\n");
                return 1;//"1";
            }
        case 2:
            if (buf[0] == 0xFE && buf[1] == 0xFF)
            {
//                printf("utf-16be\n");
                return 3;//"3";
            }
            if (buf[0] == 0xFF && buf[1] == 0xFE)
            {
//                printf("utf-16le\n");
                return 2;//"2";
            }
        default:
//            printf("utf-8\n");
            return 0;//"0";
    }
}

