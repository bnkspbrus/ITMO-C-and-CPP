#ifndef LN_H
#define LN_H

#include <string_view>

class LN
{
public:

    LN(const LN &other);

    LN(LN &&other);

    LN(long long number = 0);

    LN(const char *string);

    LN(std::string_view string);

    LN(int array_size, int *digit, bool negate);

    ~LN();

    friend LN operator ""_ln(const char *string);

    LN operator-() const;

    LN &operator=(const LN &other);

    LN operator+(const LN &other) const;

    LN operator-(const LN &other) const;

    LN operator*(const LN &other) const;

    LN operator/(const LN &other) const;

    LN operator%(const LN &other) const;

    LN operator~() const;

    LN operator<(const LN &other) const;

    LN operator<=(const LN &other) const;

    LN operator>(const LN &other) const;

    LN operator>=(const LN &other) const;

    LN operator==(const LN &other) const;

    LN operator!=(const LN &other) const;

    operator bool() const;

    operator long long() const;

    void print(FILE *fout) const;

private:
    int compare_abs(const LN &first, const LN &second) const;

    bool negate_;
    int array_size_;
    int *digit_;
};

#endif //LN_H
