#include <cstring>
#include "LN.h"
#include <cmath>
#include <cstdio>
#include <string>
#include <iostream>

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
    if (number / 1e9 != 0) array_size = 2;
    if (number / 1e18 != 0) array_size = 3;
    digit = new int[array_size];
    for (int i = 0; i < array_size; i++)
    {
        digit[i] = abs(number % (int) 1e9);
        number /= (int) 1e9;
    }
}

LN::LN(const char *string)
{
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
    int last = len % 9 == 0 ? 9 : len % 9;
    strncpy(num, string, last);
    string += last;
    sscanf(num, "%i", &digit[array_size - 1]);
    for (int i = 1; i < array_size; i++)
    {
        strncpy(num, string, 9);
        string += 9;
        sscanf(num, "%i", &digit[array_size - 1 - i]);
    }
}

LN::LN(int array_size, int *digit, bool negate)
{
    this->array_size = array_size;
    this->digit = digit;
    this->negate = negate;
}


LN::LN(string_view str)
{
    int length = str.length();
    int start = 0;
    while (length - start > 1 && str[0] == '0')
        start++;
    if (str[start] == '-')
    {
        negate = true;
        start++;
    }
    else
        negate = false;
    int len = length - start;
    array_size = ceil(len / 9.0);
    digit = new int[array_size];
    for (int i = length, ind = 0; i > start; i -= 9, ind++)
    {
        if (i - start < 9)
        {
            for (int j = start; j < i; j++)
            {
                digit[ind] *= 10;
                digit[ind] += str[j] - '0';
                cout << "digit[" << ind << "] " << digit[ind] << endl;
            }
        }
        else
        {
            for (int j = i - 9; j < i; j++)
            {
                digit[ind] *= 10;
                digit[ind] += str[j] - '0';
            }
        }
    }
}

LN::~LN()
{
    if (digit != NULL)
        delete[] digit;
}

LN::LN(LN &&other)
{

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
    {
        return *this - other.operator_();
    }
    int carry = 0;
    int max_size = max(array_size, other.array_size);
    int min_size = min(array_size, other.array_size);
    int *max = array_size > other.array_size ? digit : other.digit;
    int *min = digit != max ? digit : other.digit;
    int *res = new int[max_size]();
    for (int i = 0; i < max_size; i++)
    {
        res[i] += carry + max[i] + (i < min_size ? min[i] : 0);
        carry = res[i] >= 1e9;
        if (carry) res[i] -= 1e9;
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
    return LN(max_size, res, other.negate);
}

int compare_abs(const LN &first, const LN &second);

LN LN::operator-(const LN &other) const
{
    if (this->negate != other.negate)
        return *this + other.operator_();
    int carry = 0;
    int max_size = max(array_size, other.array_size);
    int min_size = min(array_size, other.array_size);
    int *max = other.digit;
    int *min = digit;
    int *res = new int[max_size]();
    if (compare_abs(*this, other) > 0)
    {
        max = digit;
        min = other.digit;
    }
    for (int i = 0; i < max_size || carry; i++)
    {
        res[i] = max[i] - carry - (i < min_size ? min[i] : 0);
        carry = res[i] < 0;
        if (carry) res[i] += 1e9;
    }
    int i = max_size;
    while (i > 1 && res[i - 1] == 0)
    {
        i--;
    }
    if (max_size != i)
    {
        int *term = new int[i];
        copy(res, res + i, term);
        delete[] res;
        res = term;
        max_size = i;
    }

    return LN(max_size, res, compare_abs(*this, other) > 0 ? negate : !negate);
}


LN &LN::operator*(const LN &other) const
{

}

LN &LN::operator/(const LN &other) const
{

}

LN &LN::operator%(const LN &other) const
{
}

int compare_abs(const LN &first, const LN &second)
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

bool LN::operator<(const LN &other) const
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

bool LN::operator==(const LN &other) const
{
    if (array_size == 1 && other.array_size == 1 && digit[0] == 0 && other.digit[0] == 0)
        return true;
    return negate == other.negate && compare_abs(*this, other) == 0;
}

bool LN::operator<=(const LN &other) const
{
    return *this < other || *this == other;
}

bool LN::operator>(const LN &other) const
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

bool LN::operator>=(const LN &other) const
{
    return *this > other || *this == other;
}

bool LN::operator!=(const LN &other) const
{
    return !(*this == other);
}

string LN::toString() const
{
    string ans = to_string(digit[array_size - 1]);
    for (int i = array_size - 2; i >= 0; i--)
    {
        string add = to_string(digit[i]);
        while (add.length() < 9)
        {
            add = "0" + add;
        }
        ans += add;
    }
    return ans;
}

LN LN::operator~() const
{
}
