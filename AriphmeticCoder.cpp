#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <map>
#include <queue>
using namespace std;

struct SymbolProb
{
    char letter;
    double Rprob;
    double Lprob;
};

struct SymbolFreq
{
    char letter;
    double frequency;
};

SymbolProb* defineRanges(unordered_map<char, double> prob)
{
    SymbolProb* result = new SymbolProb[256];
    double l = 0;
    for (auto pair : prob)
    {
        result[pair.first].letter = pair.first;
        result[pair.first].Lprob = l;
        result[pair.first].Rprob = l + pair.second;
        l = result[pair.first].Rprob;
    }

    return result;
}

void ArCoder(const char* input_text = "text.txt", const char* output_text = "cipher.txt")
{
    FILE* input = fopen(input_text, "rb");
    if (!input)
    {
        puts("ArCoder ERROR: No such file or directory\n");
        exit(1);
    }

    char key; double countSymbols;
    fseek(input, 0, SEEK_END);
    countSymbols = ftell(input);
    fseek(input, 0, SEEK_SET);

    map<char, double> probUO;
    while (!feof(input))
    {
        key = fgetc(input);
        if (!feof(input))
        {
            probUO[key] += 1 / countSymbols;
        }
    }

    priority_queue <double> pq;
    for (auto pair : probUO)
    {
        pq.push(pair.second);
    }

    unordered_map<char, double> probO;
    //map<char, double> ::iterator it;
    double tmp;
    while (!probUO.empty())
    {
        tmp = pq.top(); pq.pop();
        for (auto pair : probUO)
        {
            if (pair.second == tmp)
            {
                probO.emplace(pair.first, tmp);
                
                probUO.erase(probUO.find(pair.first));
                break;
            }
        }

    }

    for (auto pair : probO)
    {
        cout << pair.first << " " << pair.second << "\n";
    }

    fseek(input, 0, SEEK_SET);
    SymbolProb* Ranges = defineRanges(probO);
    double left = 0;
    double right = 1;
    for (int i = 0; !feof(input) && i < countSymbols - 1; i++)
    {
        char symb = fgetc(input);
        double newRight = left + (right - left) * Ranges[symb].Rprob;
        double newLeft = left + (right - left) * Ranges[symb].Lprob;
        left = newLeft;
        right = newRight;
    }
    double code = (left + right) / 2;
    cout << (left + right) / 2 << "\n";
    
    string s = "";
    for (int i = 0; i < countSymbols; i++)
    {
        for (auto pair : probO)
        {
            if (code >= Ranges[pair.first].Lprob && code < Ranges[pair.first].Rprob)
            {
                s += Ranges[pair.first].letter;
                code = (code - Ranges[pair.first].Lprob) / (Ranges[pair.first].Rprob - Ranges[pair.first].Lprob);
                break;
            }
        }
    }

    cout << s;
}

int main()
{
    ArCoder();
}