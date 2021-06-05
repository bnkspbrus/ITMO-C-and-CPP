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
    map<string, un_method> un_map = {{"-", &LN::operator_},
                                     {"~", &LN::operator~}};
    ifstream fin;
    fin.open("input.txt");
    FILE *fout = fopen("output.txt", "w");
    try
    {
        if (!fin.is_open())
            throw "input file didn't open\n";
        if (fout == NULL)
            throw "output file didn't open\n";
    }
    catch (const char *str)
    {
        cerr << str << endl;
    }
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
            stack.push(LN(str));
            continue;
        }
        break;
    }
    while (!stack.empty())
    {
        stack.top().print(fout);
        stack.pop();
    }
    fin.close();
    fclose(fout);
}



