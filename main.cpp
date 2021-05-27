#include <iostream>
#include <stack>
#include <fstream>
#include "LN.h"
#include <set>

using namespace std;

int main()
{
    ifstream fin;
    fin.open("input.txt");
    ofstream fout;
    fout.open("output.txt");
    try
    {
        if (!fin.is_open())
            throw "input file didn't open\n";
        if (!fout.is_open())
            throw "output file didn't open\n";
    }
    catch (const char *str)
    {
        cerr << str << endl;
    }
    string arr_bin[] = {"+", "-", "*", "/", "%", "<", "<=", ">", ">=", "==", "!="};
    string arr_un[] = {"_", "~"};
    set<string> bin_ops(arr_bin, arr_bin + 11);
    set<string> un_ops(arr_un, arr_un + 2);
    stack<string> stack;
    string str;
    while (fin >> str)
    {
        stack.push(str);
    }
    while (true)
    {
        if (!un_ops.count(stack.top()) && !bin_ops.count(stack.top()))
        {
            if (stack.size() == 1)
                break;
            string arg1 = stack.top();
            stack.pop();
            if (!un_ops.count(stack.top()) && !bin_ops.count(stack.top()))
            {
                string arg2 = stack.top();
                stack.pop();
                if (bin_ops.count(stack.top()))
                {
                    string fun = stack.top();
                    stack.pop();
                    LN a1(arg1);
                    LN a2(arg2);
                    if (fun == "+")
                        stack.push((a1 + a2).toString());
                    else if (fun == "-")
                        stack.push((a1 - a2).toString());
                    else if (fun == "*")
                        stack.push((a1 * a2).toString());
                    else if (fun == "/")
                        stack.push((a1 / a2).toString());
                    else if (fun == "%")
                        stack.push((a1 % a2).toString());
                    else if (fun == "<")
                        stack.push(a1 < a2 ? "1" : "0");
                    else if (fun == "<=")
                        stack.push(a1 <= a2 ? "1" : "0");
                    else if (fun == ">")
                        stack.push(a1 > a2 ? "1" : "0");
                    else if (fun == ">=")
                        stack.push(a1 >= a2 ? "1" : "0");
                    else if (fun == "==")
                        stack.push(a1 == a2 ? "1" : "0");
                    else
                        stack.push(a1 != a2 ? "1" : "0");
                    continue;
                }
            }
            else
            {
                if (un_ops.count(stack.top()))
                {
                    string fun = stack.top();
                    stack.pop();
                    LN a1(arg1);
                    if (fun == "_")
                        stack.push(a1.operator_().toString());
                    else
                        stack.push(a1.operator~().toString());
                    continue;
                }
            }
        }
        break;
    }
    while (!stack.empty())
    {
        fout << stack.top() << endl;
        stack.pop();
    }
    fin.close();
    fout.close();
}