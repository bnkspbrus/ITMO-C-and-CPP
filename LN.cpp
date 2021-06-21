#include <cstring>
#include "LN.h"
#include <cmath>
#include <cstdio>
#include <algorithm>

#define BASE (int) 1e9

using namespace std;

void check_alloc(int *point, int *old_point = NULL);

LN::LN(const LN &other)
{
    if (other.is_nan_)
    {
        is_nan_ = true;
    }
    else
    {
        array_size_ = other.array_size_;
        negate_ = other.negate_;
        digit_ = (int *) malloc(sizeof(int) * array_size_);
        check_alloc(digit_);
        copy(other.digit_, other.digit_ + array_size_, digit_);
    }
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
    digit_ = (int *) malloc(sizeof(int) * array_size_);
    check_alloc(digit_);
    for (int i = 0; i < array_size_; i++)
    {
        digit_[i] = abs(number % BASE);
        number /= BASE;
    }
}

LN::LN(const char *string)
{
    if (strcmp("NaN", string) == 0)
    {
        is_nan_ = true;
        return;
    }
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
    digit_ = (int *) malloc(sizeof(int) * array_size_);
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
    free(digit_);
}

LN::LN(LN &&other)
{
    if (other.is_nan_)
    {
        is_nan_ = true;
    }
    else
    {
        array_size_ = other.array_size_;
        digit_ = other.digit_;
        negate_ = other.negate_;
        other.digit_ = NULL;
    }
}

LN &LN::operator=(const LN &other)
{
    if (this != &other)
    {
        if (other.is_nan_)
        {
            is_nan_ = true;
        }
        else
        {
            free(digit_);
            array_size_ = other.array_size_;
            digit_ = (int *) malloc(sizeof(int) * array_size_);
            check_alloc(digit_);
            copy(other.digit_, other.digit_ + array_size_, digit_);
            negate_ = other.negate_;
        }
    }
    return *this;
}

LN LN::operator-() const
{
    if (is_nan_)
    {
        return LN("NaN");
    }
    LN temp(*this);
    temp.negate_ = !temp.negate_;
    return temp;
}

LN LN::operator+(const LN &other) const
{
    if (is_nan_ || other.is_nan_)
    {
        return LN("NaN");
    }
    if (this->negate_ != other.negate_)
        return *this - (-other);
    int carry = 0;
    int max_size = max(array_size_, other.array_size_);
    int *res = (int *) malloc(sizeof(int) * max_size);
    check_alloc(res);
    fill(res, res + max_size, 0);
    for (int i = 0; i < max_size; i++)
    {
        res[i] = i < array_size_ ? digit_[i] : 0;
        res[i] += carry + (i < other.array_size_ ? other.digit_[i] : 0);
        carry = res[i] >= BASE;
        if (carry) res[i] -= BASE;
    }
    if (carry)
    {
        int *old_temp = res;
        res = (int *) realloc(res, sizeof(int) * (max_size + 1));
        check_alloc(res, old_temp);
        res[max_size] = 1;
        max_size++;
    }
    return LN(max_size, res, negate_);
}

LN LN::operator-(const LN &other) const
{
    if (is_nan_ || other.is_nan_)
    {
        return LN("NaN");
    }
    if (this->negate_ != other.negate_)
        return *this + (-other);
    int carry = 0;
    int max_size = max(array_size_, other.array_size_);
    int min_size = min(array_size_, other.array_size_);
    int *res = (int *) malloc(sizeof(int) * array_size_);
    check_alloc(res);
    int *sub;
    bool neg;
    if (cmp_abs(other) > 0)
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
        int *old_res = res;
        res = (int *) realloc(res, sizeof(int) * size);
        check_alloc(res, old_res);
        max_size = size;
    }
    return LN(max_size, res, neg);
}


LN LN::operator*(const LN &other) const
{
    if (is_nan_ || other.is_nan_)
    {
        return LN("NaN");
    }
    int *res = (int *) malloc(sizeof(int) * (array_size_ + other.array_size_));
    check_alloc(res);
    fill(res, res + array_size_ + other.array_size_, 0);
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
        int *old_res = res;
        res = (int *) realloc(res, sizeof(int) * size);
        check_alloc(res, old_res);
    }
    return LN(size, res, negate_ != other.negate_);
}

LN LN::operator/(const LN &other) const
{
    if (is_nan_ || other.is_nan_ || other.is_zero())
    {
        return LN("NaN");
    }
    LN mod;
    return divide(other, mod);
}

LN LN::operator%(const LN &other) const
{
    if (is_nan_ || other.is_nan_ || other.is_zero())
    {
        return LN("NaN");
    }
    LN mod;
    divide(other, mod);
    return mod;
}

int LN::cmp_abs(const LN &second) const
{
    if (array_size_ != second.array_size_)
    {
        if (array_size_ > second.array_size_)
            return 1;
        else
            return -1;
    }
    for (int i = array_size_ - 1; i >= 0; i--)
    {
        if (digit_[i] != second.digit_[i])
        {
            if (digit_[i] > second.digit_[i])
                return 1;
            else
                return -1;
        }
    }
    return 0;
}

LN LN::operator<(const LN &other) const
{
    if (is_nan_ || other.is_nan_)
    {
        return false;
    }
    if (negate_ != other.negate_)
    {
        if (negate_)
            return true;
        else
            return false;
    }
    if (negate_ && other.negate_)
        return cmp_abs(other) == 1;
    return cmp_abs(other) == -1;
}

LN LN::operator==(const LN &other) const
{
    if (is_nan_ || other.is_nan_)
    {
        return false;
    }
    return is_zero() && other.is_zero() || negate_ == other.negate_ && cmp_abs(other) == 0;
}

LN LN::operator<=(const LN &other) const
{
    if (is_nan_ || other.is_nan_)
    {
        return false;
    }
    return *this < other || *this == other;
}

LN LN::operator>(const LN &other) const
{
    if (is_nan_ || other.is_nan_)
    {
        return false;
    }
    if (negate_ != other.negate_)
    {
        if (negate_)
            return false;
        else
            return true;
    }
    if (negate_ && other.negate_)
        return cmp_abs(other) == -1;
    return cmp_abs(other) == 1;
}

LN LN::operator>=(const LN &other) const
{
    if (is_nan_ || other.is_nan_)
    {
        return false;
    }
    return *this > other || *this == other;
}

LN LN::operator!=(const LN &other) const
{
    if (is_nan_ || other.is_nan_)
    {
        return true;
    }
    return !(*this == other);
}

LN::operator bool() const
{
    return !is_zero();
}

LN::operator long long() const
{
    if (is_zero())
    {
        return 0;
    }
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
    if (is_nan_)
    {
        fprintf(fout, "NaN\n");
        return;
    }
    if (is_zero())
    {
        fprintf(fout, "0\n");
        return;
    }
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
    if (is_nan_ || negate_)
    {
        return LN("NaN");
    }
    int count_pair = (array_size_ >> 1) + (1 & array_size_);
    int *res = (int *) malloc(sizeof(int) * count_pair);
    LN current, subtract;
    for (int i = array_size_ - 1, j = count_pair - 1; i >= 0; j--)
    {
        current.shift_right();
        current.digit_[0] = digit_[i--];
        if (!(1 & i))
        {
            current.shift_right();
            current.digit_[0] = digit_[i--];
        }
        subtract.shift_right();
        int left = 0, right = BASE;
        while (left < right - 1)
        {
            int mid = (left + right) >> 1;
            subtract.digit_[0] = mid;
            if ((subtract * mid).cmp_abs(current) <= 0)
            {
                left = mid;
            }
            else
            {
                right = mid;
            }
        }
        res[j] = left;
        subtract.digit_[0] = left;
        current = current - (subtract * left);
        subtract = subtract + LN(left);
    }
    return LN(count_pair, res, false);
}

void check_alloc(int *point, int *old_point)
{
    if (point == NULL)
    {
        free(old_point);
        throw "memory isn't allocated";
    }
}

LN operator ""_ln(const char *string)
{
    return LN(string);
}

bool LN::is_zero() const
{
    return array_size_ == 1 && digit_[0] == 0;
}

void LN::shift_right()
{
    if (is_zero())
    {
        return;
    }
    array_size_++;
    int *temp = (int *) malloc(sizeof(int) * array_size_);
    check_alloc(temp, digit_);
    copy(digit_, digit_ + array_size_ - 1, temp + 1);
    free(digit_);
    digit_ = temp;
}

LN LN::operator*(const int other) const
{
    if (other == 0)
    {
        return LN();
    }
    int carry = 0;
    int *res = (int *) malloc(sizeof(int) * array_size_);
    int size = array_size_;
    for (int i = 0; i < array_size_; i++)
    {
        long long cur = carry + (long long) digit_[i] * other;
        res[i] = cur % BASE;
        carry = cur / BASE;
    }
    if (carry)
    {
        int *old_res = res;
        res = (int *) realloc(res, sizeof(int) * ++size);
        check_alloc(res, old_res);
        res[size - 1] = carry;
    }
    return LN(size, res, negate_ != (other < 0));
}

LN LN::divide(const LN &other, LN &mod) const
{
    if (is_zero() || cmp_abs(other) < 0)
    {
        return LN();
    }
    int *res = (int *) malloc(sizeof(int) * array_size_);
    for (int i = array_size_ - 1; i >= 0; i--)
    {
        mod.shift_right();
        mod.digit_[0] = digit_[i];
        int left = 0, right = BASE;
        while (left < right - 1)
        {
            int mid = (left + right) >> 1;
            if ((other * mid).cmp_abs(mod) <= 0)
            {
                left = mid;
            }
            else
            {
                right = mid;
            }
        }
        res[i] = left;
        mod = mod - (other.negate_ ? -other : other) * left;
    }
    int size = array_size_;
    while (size > 1 && !res[size - 1])
    {
        size--;
    }
    if (size != array_size_)
    {
        int *old_res = res;
        res = (int *) realloc(res, sizeof(int) * size);
        check_alloc(res, old_res);
    }
    mod.negate_ = negate_;
    return LN(size, res, negate_ != other.negate_);
}

LN &LN::operator=(LN &&other)
{
    if (this != &other)
    {
        if (other.is_nan_)
        {
            is_nan_ = true;
        }
        else
        {
            free(digit_);
            digit_ = other.digit_;
            other.digit_ = NULL;
            array_size_ = other.array_size_;
            negate_ = other.negate_;
        }
    }
    return *this;
}
