#ifndef LN_H
#define LN_H

#include <string_view>


class LN
{
public:
    bool negate;
    int array_size;
    int *digit;

    ~LN();

    LN(const LN &other);

    LN(long long number);

    LN(const char *string);

    LN(std::string_view string);

    LN(LN &&other);

    LN(int array_size, int *digit, bool negate);

    LN operator_() const;

    LN &operator=(const LN &other);

    LN operator+(const LN &other) const;

    LN operator-(const LN &other) const;

    LN &operator*(const LN &other) const;

    LN &operator/(const LN &other) const;

    LN &operator%(const LN &other) const;

    LN operator~() const;

    bool operator<(const LN &other) const;

    bool operator<=(const LN &other) const;

    bool operator>(const LN &other) const;

    bool operator>=(const LN &other) const;

    bool operator==(const LN &other) const;

    bool operator!=(const LN &other) const;

    std::string toString() const;
};


#endif //LN_H
