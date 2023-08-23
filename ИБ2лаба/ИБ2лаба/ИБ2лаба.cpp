/*Два друга хотят обмениваться зашифрованными сообщениями, но у них нет подходящей программы.
Напишите программу позволяющую шифровать и расшифровывать сообщения 
с использованием алгоритма DESX (Des extended) с использованием трѐх ключей,
т.е. k1, k2, k3 не зависимые. Входные и выходные данные запишите в файл типа .txt.*/


#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <cmath>
#include "data.cpp";

void Write(std::string text, std::string expression)
{
    std::ofstream FileOut(text, std::ios::app);
    FileOut.seekp(0);
    if (FileOut.is_open())
    {
        FileOut << expression << std::endl;
    }
    FileOut.close();
}

void delOut(std::string text)
{
    std::fstream Filedel(text, std::ios::out);

    if (Filedel.is_open())
    {
        Filedel << "";
    }
    Filedel.close();
}

std::string XORg(std::string block1, std::string block2)
{
    if (block1.length() == block2.length())
    {
        for (int i = 0; i < block1.length(); i++)
        {
            if (block1[i] == block2[i])
                block1[i] = '0';
            else
                block1[i] = '1';
        }
    }
    return block1;
}

std::string Permutation(std::string block64b, const int* mat, size_t n)
{
    std::string result_block;
    for (int i = 0; i < n; i++)
        result_block += (block64b[mat[i] - 1]);
    return result_block;
}

std::string to_binary_char(unsigned int n, unsigned int limit)
{
    std::string buffer;
    do
    {
        buffer += char('0' + n % 2);
        n = n / 2;
    } while (n > 0);

    while (buffer.length() < limit)
        buffer.append("0");
    return std::string(buffer.crbegin(), buffer.crend());
}

void to_binary_string(std::string text, std::string& strbinary)
{
    std::string b;
    for (int i = 0; i < text.length(); i++)
    {
        std::cout << int(text[i]) << ' ' << (int(text[i]) & 0x00ff) << std::endl;
        b = to_binary_char(int(text[i]) & 0x00ff, 8);
        strbinary.append(b);
    }
    std::cout << std::endl;
    while (strbinary.length() % 64 != 0)
        strbinary.append("0");
}

char to_char_binary(std::string strbinary)
{
    int conv_char = 0;
    for (int i = 0; i < strbinary.length(); i++)
    {
        int _ = strbinary[i] - '0';
        conv_char += _ * std::pow(2, strbinary.length() - (i + 1));
    }
    std::cout << conv_char << ' ' << strbinary << char((conv_char - 0x0100)) << std::endl;
    return char((conv_char - 0x0100));
}

std::string to_string_binary(std::string strbinary)
{
    std::string text;
    for (int i = 0; i < strbinary.length(); i += 8)
    {
        char M_i = to_char_binary(strbinary.substr(i, 8));
        if (M_i)
            text += M_i;
        else
            return text;
    }
}

std::string* Read_Text(std::string text, std::string& M_bytes)
{
    std::string line;
    std::ifstream Filein(text);
    Filein.seekg(0);
    std::string* K_bytes = new std::string[3];

    int i = 0;
    if (Filein.is_open())
    {

        while (getline(Filein, line))
        {

            if (i == 0)
                M_bytes = line;
            else
                K_bytes[i - 1] = line;
            i++;
        }
        Filein.close();
    }
    else
    {
        std::cout << "File error" << std::endl;
    }
    return K_bytes;
}

std::string* Key_generator(std::string K)
{
    K = Permutation(K, data::PC1, 56);
    std::string C = K.substr(0, 28);
    std::string D = K.substr(28, 28);
    const int PK[16] = { 1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2 ,2 ,1 };
    std::string* result_Keys = new std::string[16];
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < PK[i]; j++)
        {
            char _ = C[0];
            C = C.substr(1, C.length() - 1);
            C += _;
            _ = D[0];
            D = D.substr(1, D.length() - 1);
            D += _;

        }
        result_Keys[i] = Permutation(C + D, data::PC2, 48);
    }
    return result_Keys;
}

std::string S_Per(std::string block6b, const int* S)
{
    int _ = S[(((int)block6b[0] - 48) * 2 + ((int)block6b[5] - 48)) * 16 + (((int)block6b[1] - 48) * 8 + ((int)block6b[2] - 48) * 4 + ((int)block6b[3] - 48) * 2 + ((int)block6b[4] - 48))];
    return to_binary_char(_, 4);
}

std::string F(std::string block, std::string K)
{
    block = Permutation(block, data::E, 48);
    block = XORg(block, K);
    for (int i = 0; i < 8; i++)
    {
        block.replace(4 * i, 4, S_Per(block.substr(6 * i, 6), data::S[i]));
    }
    return Permutation(block.substr(0, 32), data::P, 32);

}

std::string DesXE(std::string M, std::string* K)
{
    M = XORg(M, K[1]);
    M = Permutation(M, data::IP, 64);
    std::string* K1 = Key_generator(K[0]);
    std::string LR[2];
    LR[0] = M.substr(0, 32);
    LR[1] = M.substr(32, 32);

    for (int i = 0; i < 15; i++)
    {
        std::string _ = LR[1];
        LR[1] = XORg(LR[0], F(LR[1], K1[i]));
        LR[0] = _;
    }
    LR[0] = XORg(LR[0], F(LR[1], K1[15]));
    M = Permutation(LR[0] + LR[1], data::invIP, 64);
    M = XORg(M, K[2]);
    return M;
}

std::string DesXD(std::string M, std::string* K)
{

    M = XORg(M, K[2]);
    M = Permutation(M, data::IP, 64);
    std::string* K1 = Key_generator(K[0]);
    std::string LR[2];
    LR[0] = M.substr(0, 32);
    LR[1] = M.substr(32, 32);

    for (int i = 15; i > 0; i--)
    {
        std::string _ = LR[1];
        LR[1] = XORg(LR[0], F(LR[1], K1[i]));
        LR[0] = _;
    }
    LR[0] = XORg(LR[0], F(LR[1], K1[0]));
    M = Permutation(LR[0] + LR[1], data::invIP, 64);
    M = XORg(M, K[1]);
    return M;
}

void E(std::string& text, std::string* K, std::string file)
{
    for (int i = 0; i < text.length(); i += 64)
        text.replace(i, 64, DesXE(text.substr(i, 64), K));
    Write(file, text);
    Write(file, K[0]);
    Write(file, K[1]);
    Write(file, K[2]);
}

void D(std::string fileRead, std::string fileWrite)
{
    std::string M_byte;
    std::string* K = Read_Text(fileRead, M_byte);
    delOut(fileRead);
    for (int i = 0; i < M_byte.length(); i += 64)
        M_byte.replace(i, 64, DesXD(M_byte.substr(i, 64), K));
    std::cout << M_byte.length() << std::endl;
    Write(fileWrite, to_string_binary(M_byte));
}

int main()
{
    std::setlocale(LC_ALL, "rus");

    delOut("Output.txt");
    std::string M;
    std::string* K = Read_Text("input.txt", M);
    std::string M_bit;
    std::string* Key_bit = new std::string[3];
    to_binary_string(M, M_bit);
    for (int i = 0; i < 3; i++)
        to_binary_string(K[i], Key_bit[i]);
    E(M_bit, Key_bit, "Encryption.txt");
    //D("Encryption.txt", "output.txt");
    return 0;
}
