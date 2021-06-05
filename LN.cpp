#include <cstring>
#include "LN.h"
#include <cmath>
#include <cstdio>
#include <string>

#define BASE (int) 1e9

using namespace std;

void check_alloc(int *point);

LN::LN(const LN &other)
{
    array_size_ = other.array_size_;
    negate_ = other.negate_;
    digit_ = new(nothrow) int[array_size_];
    check_alloc(digit_);
    copy(other.digit_, other.digit_ + array_size_, digit_);
}

LN::LN(long long number)
{
    if (number < 0)
        negate_ = true;
    else
        negate_ = false;
    array_size_ = 1;
    if (number / BASE != 0) array_size_ = 2;
    if (number / (long long) 1e18 != 0) array_size_ = 3;
    digit_ = new(nothrow) int[array_size_];
    check_alloc(digit_);
    for (int i = 0; i < array_size_; i++)
    {
        digit_[i] = abs(number % BASE);
        number /= BASE;
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
        negate_ = true;
        string++;
    }
    else
        negate_ = false;
    int len = strlen(string);
    array_size_ = ceil(len / 9.0);
    digit_ = new(nothrow) int[array_size_];
    check_alloc(digit_);
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
        sscanf(num, "%d", &digit_[ind]);
    }
}

LN::LN(int array_size, int *digit, bool negate)
{
    this->array_size_ = array_size;
    this->digit_ = digit;
    this->negate_ = negate;
}


LN::LN(string_view str) : LN(str.data())
{
}

LN::~LN()
{
    if (digit_ != NULL)
        delete[] digit_;
}

LN::LN(LN &&other) : digit_(other.digit_)
{
    array_size_ = other.array_size_;
    negate_ = other.negate_;
    other.digit_ = NULL;
}

LN &LN::operator=(const LN &other)
{
    if (this != &other)
    {
        delete[] digit_;
        array_size_ = other.array_size_;
        digit_ = new(nothrow) int[array_size_];
        check_alloc(digit_);
        copy(other.digit_, other.digit_ + array_size_, digit_);
        negate_ = other.negate_;
    }
    return *this;
}

LN LN::operator-() const
{
    LN temp(*this);
    temp.negate_ = !temp.negate_;
    return temp;
}

LN LN::operator+(const LN &other) const
{
    if (this->negate_ != other.negate_)
        return *this - (-other);
    int carry = 0;
    int max_size = max(array_size_, other.array_size_);
    int *res = new(nothrow) int[max_size]();
    check_alloc(res);
    for (int i = 0; i < max_size; i++)
    {
        res[i] = i < array_size_ ? digit_[i] : 0;
        res[i] += carry + (i < other.array_size_ ? other.digit_[i] : 0);
        carry = res[i] >= BASE;
        if (carry) res[i] -= BASE;
    }
    if (carry)
    {
        int *temp = new(nothrow) int[max_size + 1];
        check_alloc(temp);
        copy(res, res + max_size, temp);
        delete[] res;
        temp[max_size] = 1;
        max_size++;
        res = temp;
    }
    return LN(max_size, res, negate_);
}

LN LN::operator-(const LN &other) const
{
    if (this->negate_ != other.negate_)
        return *this + (-other);
    int carry = 0;
    int max_size = max(array_size_, other.array_size_);
    int min_size = min(array_size_, other.array_size_);
    int *res = new(nothrow) int[max_size]();
    check_alloc(res);
    int *sub;
    bool neg;
    if (compare_abs(*this, other) > 0)
    {
        neg = negate_;
        copy(digit_, digit_ + max_size, res);
        sub = other.digit_;
    }
    else
    {
        neg = !negate_;
        copy(other.digit_, other.digit_ + max_size, res);
        sub = digit_;
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
        int *temp = new(nothrow) int[size];
        check_alloc(temp);
        copy(res, res + size, temp);
        delete[] res;
        max_size = size;
        res = temp;
    }
    return LN(max_size, res, neg);
}


LN LN::operator*(const LN &other) const
{
    int *res = new(nothrow) int[array_size_ + other.array_size_]();
    check_alloc(res);
    for (int i = 0; i < array_size_; i++)
    {
        for (int j = 0, carry = 0; j < other.array_size_ || carry; j++)
        {
            long long cur = res[i + j] + (long long) digit_[i] * (j < other.array_size_ ? other.digit_[j] : 0) + carry;
            res[i + j] = cur % BASE;
            carry = cur / BASE;
        }
    }
    int size = array_size_ + other.array_size_;
    while (size > 1 && !res[size - 1])
        size--;
    if (array_size_ + other.array_size_ != size)
    {
        int *temp = new(nothrow) int[size]();
        check_alloc(temp);
        copy(res, res + size, temp);
        delete[] res;
        res = temp;
    }
    return LN(size, res, negate_ != other.negate_);
}

LN LN::operator/(const LN &other) const
{
    int *res = new(nothrow) int[array_size_];
    check_alloc(res);
    copy(digit_, digit_ + array_size_, res);
    int carry = 0;
    for (int i = array_size_ - 1; i >= 0; i--)
    {
        long long cur = res[i] + (long long) carry * BASE;
        res[i] = int(cur / other.digit_[0]);
        carry = int(cur % other.digit_[0]);
    }
    int size = array_size_;
    while (size > 1 && !res[size - 1])
    {
        size--;
    }
    if (size != array_size_)
    {
        int *temp = new(nothrow) int[size];
        check_alloc(temp);
        copy(res, res + size, temp);
        delete[] res;
        res = temp;
    }
    return LN(size, res, negate_ != other.negate_);
}

LN LN::operator%(const LN &other) const
{
}

int LN::compare_abs(const LN &first, const LN &second) const
{
    if (first.array_size_ != second.array_size_)
    {
        if (first.array_size_ > second.array_size_)
            return 1;
        else
            return -1;
    }
    for (int i = first.array_size_ - 1; i >= 0; i--)
    {
        if (first.digit_[i] != second.digit_[i])
        {
            if (first.digit_[i] > second.digit_[i])
                return 1;
            else
                return -1;
        }
    }
    return 0;
}

LN LN::operator<(const LN &other) const
{
    if (negate_ != other.negate_)
    {
        if (negate_)
            return true;
        else
            return false;
    }
    if (negate_ && other.negate_)
        return compare_abs(*this, other) == 1;
    return compare_abs(*this, other) == -1;
}

LN LN::operator==(const LN &other) const
{
    return (negate_ == other.negate_ || array_size_ == 1 && digit_[0] == 0) && compare_abs(*this, other) == 0;
}

LN LN::operator<=(const LN &other) const
{
    return *this < other || *this == other;
}

LN LN::operator>(const LN &other) const
{
    if (negate_ != other.negate_)
    {
        if (negate_)
            return false;
        else
            return true;
    }
    if (negate_ && other.negate_)
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
    return array_size_ != 1 || digit_[0];
}

LN::operator long long() const
{
    long long res = 0;
    for (int i = array_size_ - 1; i >= 0; i--)
    {
        res *= BASE;
        res += negate_ ? -digit_[i] : digit_[i];
        if (res < 0 != negate_)
        {
            throw "number is large";
        }
    }
    return res;
}

void LN::print(FILE *fout) const
{
    if (negate_)
        fprintf(fout, "-");
    fprintf(fout, "%d", digit_[array_size_ - 1]);
    for (int i = array_size_ - 2; i >= 0; i--)
    {
        fprintf(fout, "%09d", digit_[i]);
    }
    fprintf(fout, "\n");
}

LN LN::operator~() const
{
}

void check_alloc(int *point)
{
    if (point == NULL)
    {
        throw "memory isn't allocated";
    }
}

LN operator ""_ln(const char *string)
{
    return LN(string);
}
