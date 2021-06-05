#ifndef UNTITLED1_LN_H
#define UNTITLED1_LN_H

#include <string_view>


class LN
{
public:

    ~LN();

    LN(const LN &other);

    LN(long long number = 0);

    LN(const char *string);

    LN(std::string_view string);

    LN(LN &&other);

    LN(int array_size, int *digit, bool negate);

    LN operator_() const;

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

    bool negate;
    int array_size;
    int *digit;
};


#endif //UNTITLED1_LN_H
