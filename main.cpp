#include <iostream>
#include <stack>
#include <fstream>
#include "LN.h"
#include <set>
#include <vector>
#include <map>

using namespace std;

typedef LN (LN::*bin_method)(const LN &) const;

typedef LN (LN::*un_method)() const;

void close_all(ifstream &fin, FILE *fout)
{
    fin.close();
    fclose(fout);
}

int main()
{
    map<string, bin_method> bin_map = {{"+",  &LN::operator+},
                                       {"-",  &LN::operator-},
                                       {"*",  &LN::operator*},
                                       {"/",  &LN::operator/},
                                       {"%",  &LN::operator%},
                                       {"<",  &LN::operator<},
                                       {"<=", &LN::operator<=},
                                       {">",  &LN::operator>},
                                       {">=", &LN::operator>=},
                                       {"==", &LN::operator==},
                                       {"!=", &LN::operator!=}};
    map<string, un_method> un_map = {{"_", &LN::operator-},
                                     {"~", &LN::operator~}};
    ifstream fin;
    fin.open("input.txt");
    FILE *fout = fopen("output.txt", "w");
    if (!fin.is_open())
        printf("input file didn't open\n");
    if (fout == NULL)
        printf("output file didn't open\n");
    string str;
    stack<LN> stack;
    while (fin >> str)
    {
        if (bin_map.count(str))
        {
            if (stack.size() >= 2)
            {
                LN arg2 = stack.top();
                stack.pop();
                LN arg1 = stack.top();
                stack.pop();
                bin_method method = bin_map[str];
                stack.push((arg1.*method)(arg2));
                continue;
            }
        }
        else if (un_map.count(str))
        {
            if (stack.size() >= 1)
            {
                LN arg1 = stack.top();
                stack.pop();
                un_method method = un_map[str];
                stack.push((arg1.*method)());
                continue;
            }
        }
        else
        {
            try
            {
                stack.push(LN(str));
            }
            catch (const char *e)
            {
                while (!stack.empty())
                {
                    stack.pop();
                }
                close_all(fin, fout);
                printf("%s\n", e);
                return 2;
            }
            continue;
        }
        break;
    }
    while (!stack.empty())
    {
        stack.top().print(fout);
        stack.pop();
    }
    close_all(fin, fout);
}



