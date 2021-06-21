#include <iostream>
#include <stack>
#include <fstream>
#include "LN.h"
#include <map>

using namespace std;

typedef LN (LN::*bin_method)(const LN &) const;

typedef LN (LN::*un_method)() const;

typedef bool (LN::*comp_method)(const LN &) const;

void close_all(ifstream &fin, FILE *fout)
{
    fin.close();
    fclose(fout);
}

void push_attempt(stack<LN> &stack, LN arg, ifstream &fin, FILE *fout);

int main(const int argc, const char **argv)
{
    map<string, comp_method> comp_map = {{"==", &LN::operator==},
                                         {"<",  &LN::operator<},
                                         {"<=", &LN::operator<=},
                                         {">",  &LN::operator>},
                                         {">=", &LN::operator>=},
                                         {"!=", &LN::operator!=}};
    map<string, bin_method> bin_map = {{"+", &LN::operator+},
                                       {"-", &LN::operator-},
                                       {"*", &LN::operator*},
                                       {"/", &LN::operator/},
                                       {"%", &LN::operator%}};
    map<string, un_method> un_map = {{"_", &LN::operator-},
                                     {"~", &LN::operator~}};
    if (argc != 3)
    {
        printf("invalid number of arguments\n");
        return 1;
    }
    ifstream fin;
    fin.open(argv[1]);
    FILE *fout = fopen(argv[2], "w");
    if (!fin.is_open())
    {
        printf("input file didn't open\n");
        return 1;
    }
    if (fout == NULL)
    {
        printf("output file didn't open\n");
        return 1;
    }
    string str;
    stack<LN> stack;
    while (fin >> str)
    {
        if (comp_map.count(str) || bin_map.count(str))
        {
            if (stack.size() >= 2)
            {
                LN arg2 = stack.top();
                stack.pop();
                LN arg1 = stack.top();
                stack.pop();
                if (comp_map.count(str))
                {
                    comp_method method = comp_map[str];
                    push_attempt(stack, LN((arg1.*method)(arg2)), fin, fout);
                }
                else
                {
                    bin_method method = bin_map[str];
                    push_attempt(stack, (arg1.*method)(arg2), fin, fout);
                }
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
                push_attempt(stack, (arg1.*method)(), fin, fout);
                continue;
            }
        }
        else
        {
            push_attempt(stack, LN(str), fin, fout);
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

void push_attempt(stack<LN> &stack, LN arg, ifstream &fin, FILE *fout)
{
    try
    {
        stack.push(arg);
    }
    catch (const char *e)
    {
        while (!stack.empty())
        {
            stack.pop();
        }
        close_all(fin, fout);
        printf("%s\n", e);
        exit(2);
    }
}