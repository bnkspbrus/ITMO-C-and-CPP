#ifndef LN_H
#define LN_H

#include <string_view>
#include <cstdio>

class LN
{
public:

    LN(const LN &other);

    LN(LN &&other);

    LN(long long number = 0);

    LN(const char *string);

    LN(std::string_view string);

    ~LN();

    LN
    operator-() const;

    LN &
    operator=(const LN &other);

    LN &operator=(LN &&other);

    LN operator+(const LN &other) const;

    LN operator-(const LN &other) const;

    LN operator*(const LN &other) const;

    LN operator/(const LN &other) const;

    LN operator%(const LN &other) const;

    LN operator~() const;

    bool operator<(const LN &other) const;

    bool operator<=(const LN &other) const;

    bool operator>(const LN &other) const;

    bool operator>=(const LN &other) const;

    bool operator==(const LN &other) const;

    bool operator!=(const LN &other) const;

    operator bool() const;

    operator long long() const;

    void print(FILE *fout) const;

private:

    LN(int array_size, int *digit, bool negate);

    LN operator*(const int other) const;

    LN divide(const LN &other, LN &mod) const;

    int cmp_abs(const LN &second) const;

    bool is_zero() const;

    void shift_right();

    bool is_nan_ = false;
    bool negate_ = false;
    int array_size_ = 0;
    int *digit_ = NULL;
};

LN operator ""_ln(const char *string);

#endif //LN_H