#include <iostream>
#include <cstring>
#include <cmath>
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
    if (!fin.is_open())
    {

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
        if (!un_ops.count(stack.top()) && !bin_ops.count(stack.top()))
        {
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
                }
                else
                {
                    //throw
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
                }
                else
                {
                    stack.push(arg1);
                }
            }
        }
    }
    while (!stack.empty())
    {
        fout << stack.top() << endl;
        stack.pop();
    }
    fin.close();
    fout.close();
}