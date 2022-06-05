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
    long double Rprob;
    long double Lprob;
};

struct LetterProb
{
    char letter;
    long double prob;
};

SymbolProb* defineRanges(LetterProb* prob, int count)
{
    SymbolProb* result = new SymbolProb[256];
    double l = 0;
    for (int i = 0; i < count; i++)
    {
        result[prob[i].letter].letter = prob[i].letter;
        result[prob[i].letter].Lprob = l;
        result[prob[i].letter].Rprob = l + prob[i].prob;
        l = result[prob[i].letter].Rprob;
    }

    return result;
}

void ArCoder(const char* input_text = "text.txt", const char* output_text = "encode.txt", const char* alphabet_text = "alphabet.txt")
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

    LetterProb* probO = new LetterProb[probUO.size()];
    double tmp;
    int i = 0;
    int size = probUO.size();
    while (!probUO.empty())
    {
        tmp = pq.top(); pq.pop();
        for (auto pair : probUO)
        {
            if (pair.second == tmp)
            {
                probO[i].letter = pair.first;
                probO[i].prob = pair.second;
                probUO.erase(probUO.find(pair.first));
                i++;
                break;
            }
        }
    }

    for (int k = 0; k < size; k++) {
        cout << probO[k].letter << " " << probO[k].prob << "\n";
    }

    FILE* alphabet;
    fopen_s(&alphabet, alphabet_text, "wb");

    fputc(size, alphabet);
    for (int k = 0; k < size; k++) {
        fputc(probO[k].letter, alphabet);
        fwrite(&probO[k].prob, sizeof(double), 1, alphabet);
    }

    fclose(alphabet);

    fseek(input, 0, SEEK_SET);
    SymbolProb* Ranges = defineRanges(probO, size);
    int countcode = 5;
    FILE* output = fopen(output_text, "wb +");
    if (!output) {
        puts("ArCoder ERROR: No such file or directory\n");
        exit(1);
    }
    while (!feof(input))
    {
        long double left = 0;
        long double right = 1;
        for (int i = 0; !feof(input) && i < countcode; i++)
        {
            char symb = fgetc(input);
            if (!feof(input))
            {
                long double newRight = left + (right - left) * Ranges[symb].Rprob;
                long double newLeft = left + (right - left) * Ranges[symb].Lprob;
                left = newLeft;
                right = newRight;
            }
        }

        bool flag = false; char symbol1 = 0; string str = "";
        long double tmp = 0; int bits = 0, i = 1;
        while (!flag)
        {
            if (tmp + (1 / pow(2, i)) < left)
            {
                tmp += 1 / pow(2, i);
                symbol1 = symbol1 + (1 << (7 - bits));
                bits++;
            }
            else {
                if (tmp + (1 / pow(2, i)) >= right)
                {
                    bits++;
                }
                else
                {
                    symbol1 = symbol1 + (1 << (7 - bits));
                    str += symbol1;
                    fputc(i, output);
                    for (int y = 0; str[y] != '\0'; y++)
                    {
                        fputc(str[y], output);
                    }
                    bits = 0;
                    flag = true;
                }
            }
            if (bits == 8)
            {
                str += symbol1;
                symbol1 = 0;
                bits = 0;
            }
            i++;
        }
        
    }

    fclose(output);  
}

void CompressValue(const char* input_text = "text.txt", const char* output_text = "encode.txt") {
    long long file = 0;
    long long compressed_file = 0;
    struct stat f;

    if (!stat(input_text, &f))
        file = f.st_size;
    else perror("stat");

    if (!stat(output_text, &f))
        compressed_file = f.st_size;
    else perror("stat");

    cout << "\nCompress value is: " << (compressed_file + 0.0) / file << "\n";
}

int main()
{
    ArCoder();
    CompressValue();
}