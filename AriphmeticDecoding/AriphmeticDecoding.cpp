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

void ArDecoder(const char* input_text = "encode.txt", const char* output_text = "decode.txt", const char* alphabet_text = "alphabet.txt")
{
    FILE* alphabet;
    fopen_s(&alphabet, alphabet_text, "rb");

    int size = fgetc(alphabet);
    int countSymbols = 0;

    LetterProb* probO = new LetterProb[size];

    for (int k = 0; k < size && !feof(alphabet); k++) {
        probO[k].letter = fgetc(alphabet);
        fread(&probO[k].prob, sizeof(double), 1, alphabet);
    }

    for (int k = 0; k < size; k++) {
        cout << probO[k].letter << " " << probO[k].prob << "\n";
    }

    FILE* input;
    fopen_s(&input, input_text, "rb");

    fseek(input, 0, SEEK_END);
    countSymbols = ftell(input);
    fseek(input, 0, SEEK_SET);

    SymbolProb* Ranges = defineRanges(probO, size);
    int countcode = 5;

    FILE* output = fopen(output_text, "wb +");

    char keys; int l = 0, iter = 0, jter = 0;
    long double code = 0;
    char weight;

    while (!feof(input))
    {
        jter = 0; code = 0;
        weight = fgetc(input);
        if (!feof(input))
        {
            iter = 0;
            while (iter < weight)
            {
                l = 0;
                keys = fgetc(input);
                while (l < 8 && l < weight)
                {
                    if (keys & (1 << (7 - l)))
                    {
                        code += 1 / pow(2, (jter + 1));
                    }
                    jter++;
                    l++;
                }
                iter += 8;
            }
            countSymbols -= countcode;
            for (int i = 0; i < countcode && i < (countSymbols + countcode); i++)
            {
                for (int k = 0; k < size; k++)
                {
                    if (code >= Ranges[probO[k].letter].Lprob && code < Ranges[probO[k].letter].Rprob)
                    {
                        fputc(Ranges[probO[k].letter].letter, output);
                        code = (code - Ranges[probO[k].letter].Lprob) / (Ranges[probO[k].letter].Rprob - Ranges[probO[k].letter].Lprob);
                        break;
                    }
                }
            }
        }
    }
}

int main() {
    ArDecoder();
}