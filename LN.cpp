#include <cstring>
#include "LN.h"
#include <cmath>
#include <cstdio>
#include <string>

#define BASE (int) 1e9

using namespace std;

LN::LN(const LN &other)
{
    array_size = other.array_size;
    negate = other.negate;
    digit = new int[array_size];
    copy(other.digit, other.digit + array_size, digit);
}

LN::LN(long long number)
{
    if (number < 0)
        negate = true;
    else
        negate = false;
    array_size = 1;
    if (number / BASE != 0) array_size = 2;
    if (number / (long long) 1e18 != 0) array_size = 3;
    digit = new int[array_size];
    for (int i = 0; i < array_size; i++)
    {
        digit[i] = abs(number % BASE);
        number /= BASE;
    }
}

LN::LN(const char *string)
{
//    printf("%s\n", string);
    while (*string == '0' && strlen(string) > 1)
    {
        string++;
    }
    if (string[0] == '-')
    {
        negate = true;
        string++;
    }
    else
        negate = false;
    int len = strlen(string);
    array_size = ceil(len / 9.0);
    digit = new int[array_size];
    char num[10];
    num[9] = '\0';
    for (int i = strlen(string), ind = 0; i > 0; i -= 9, ind++)
    {
        if (i < 9)
        {
            strncpy(num, string, i);
            num[i] = '\0';
        }
        else
            strncpy(num, string + i - 9, 9);
        sscanf(num, "%d", &digit[ind]);
//        printf("num = %s digit[%i] = %i ", num, ind, digit[ind]);
    }
//    printf("\n");
}

LN::LN(int array_size, int *digit, bool negate)
{
    this->array_size = array_size;
    this->digit = digit;
    this->negate = negate;
}


LN::LN(string_view str) : LN(str.data())
{
}

LN::~LN()
{
    if (digit != NULL)
        delete[] digit;
}

LN::LN(LN &&other) : digit(other.digit)
{
    array_size = other.array_size;
    negate = other.negate;
    other.digit = NULL;
}

LN &LN::operator=(const LN &other)
{
    if (this != &other)
    {
        delete[] digit;
        array_size = other.array_size;
        digit = new int[array_size];
        copy(other.digit, other.digit + array_size, digit);
        negate = other.negate;
    }
    return *this;
}

LN LN::operator_() const
{
    LN temp(*this);
    temp.negate = !temp.negate;
    return temp;
}

LN LN::operator+(const LN &other) const
{
    if (this->negate != other.negate)
        return *this - other.operator_();
    int carry = 0;
    int max_size = max(array_size, other.array_size);
    int *res = new int[max_size]();
    for (int i = 0; i < max_size; i++)
    {
        res[i] = i < array_size ? digit[i] : 0;
        res[i] += carry + (i < other.array_size ? other.digit[i] : 0);
        carry = res[i] >= BASE;
        if (carry) res[i] -= BASE;
    }
    if (carry)
    {
        int *temp = new int[max_size + 1];
        copy(res, res + max_size, temp);
        delete[] res;
        temp[max_size] = 1;
        max_size++;
        res = temp;
    }
    return LN(max_size, res, negate);
}

LN LN::operator-(const LN &other) const
{
    if (this->negate != other.negate)
        return *this + other.operator_();
    int carry = 0;
    int max_size = max(array_size, other.array_size);
    int min_size = min(array_size, other.array_size);
    int *res = new int[max_size]();
    int *sub;
    bool neg;
    if (compare_abs(*this, other) > 0)
    {
        neg = negate;
        copy(digit, digit + max_size, res);
        sub = other.digit;
    }
    else
    {
        neg = other.negate;
        copy(other.digit, other.digit + max_size, res);
        sub = digit;
    }
    for (int i = 0; i < min_size || carry; i++)
    {
        res[i] -= carry + (i < min_size ? sub[i] : 0);
        carry = res[i] < 0;
        if (carry) res[i] += BASE;
    }
    int size = max_size;
    while (size > 1 && !res[size - 1])
        size--;
    if (max_size != size)
    {
        int *term = new int[size];
        copy(res, res + size, term);
        delete[] res;
        max_size = size;
        res = term;
    }
    return LN(max_size, res, neg);
}


LN LN::operator*(const LN &other) const
{
    int *res = new int[array_size + other.array_size]();
    for (int i = 0; i < array_size; i++)
    {
        for (int j = 0, carry = 0; j < other.array_size || carry; j++)
        {
            long long cur = res[i + j] + (long long) digit[i] * (j < other.array_size ? other.digit[j] : 0) + carry;
            res[i + j] = cur % BASE;
            carry = cur / BASE;
        }
    }
    int size = array_size + other.array_size;
    while (size > 1 && !res[size - 1])
        size--;
    if (array_size + other.array_size != size)
    {
        int *temp = new int[size]();
        copy(res, res + size, temp);
        delete[] res;
        res = temp;
    }
    return LN(size, res, negate != other.negate);
}

LN LN::operator/(const LN &other) const
{
    int *res = new int[array_size];
    copy(digit, digit + array_size, res);
    int carry = 0;
    for (int i = array_size - 1; i >= 0; i--)
    {
        long long cur = res[i] + (long long) carry * BASE;
        res[i] = int(cur / other.digit[0]);
        carry = int(cur % other.digit[0]);
    }
    int size = array_size;
    while (size > 1 && !res[size - 1])
    {
        size--;
    }
    if (size != array_size)
    {
        int *temp = new int[size];
        copy(res, res + size, temp);
        delete[] res;
        res = temp;
    }
    return LN(size, res, negate != other.negate);
}

LN LN::operator%(const LN &other) const
{
}

int LN::compare_abs(const LN &first, const LN &second) const
{
    if (first.array_size != second.array_size)
    {
        if (first.array_size > second.array_size)
            return 1;
        else
            return -1;
    }
    for (int i = first.array_size - 1; i >= 0; i--)
    {
        if (first.digit[i] != second.digit[i])
        {
            if (first.digit[i] > second.digit[i])
                return 1;
            else
                return -1;
        }
    }
    return 0;
}

LN LN::operator<(const LN &other) const
{
    if (negate != other.negate)
    {
        if (negate)
            return true;
        else
            return false;
    }
    if (negate && other.negate)
        return compare_abs(*this, other) == 1;
    return compare_abs(*this, other) == -1;
}

LN LN::operator==(const LN &other) const
{
    return (negate == other.negate || array_size == 1 && digit[0] == 0) && compare_abs(*this, other) == 0;
}

LN LN::operator<=(const LN &other) const
{
    return *this < other || *this == other;
}

LN LN::operator>(const LN &other) const
{
    if (negate != other.negate)
    {
        if (negate)
            return false;
        else
            return true;
    }
    if (negate && other.negate)
        return compare_abs(*this, other) == -1;
    return compare_abs(*this, other) == 1;
}

LN LN::operator>=(const LN &other) const
{
    return *this > other || *this == other;
}

LN LN::operator!=(const LN &other) const
{
    return !(*this == other);
}

LN::operator bool() const
{
    return array_size != 1 || digit[0];
}

LN::operator long long() const
{

}

void LN::print(FILE *fout) const
{
    if (negate)
        fprintf(fout, "-");
    fprintf(fout, "%d", digit[array_size - 1]);
    for (int i = array_size - 2; i >= 0; i--)
    {
        fprintf(fout, "%09d", digit[i]);
    }
    fprintf(fout, "\n");
}

LN LN::operator~() const
{
}