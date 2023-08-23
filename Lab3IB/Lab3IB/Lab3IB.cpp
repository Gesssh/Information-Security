/*Алгоритм RSA. Параметры: p = 233, q = 167, e = 5*/

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <queue>
#include <windows.h>
using namespace std;
vector<char> alphabet;
void create_alphabet()
{
	ifstream f_in; f_in.open("alphabet.txt", ios::in | ios::binary);
	if (!f_in.is_open())
	{
		cout << "error: alphabet" << endl;
		return;
	}
	else
	{
		char symbol;
		while (f_in.get(symbol))
		{
			if (symbol == '\n' || symbol == '\r' || symbol == ' ' || symbol == '\t')
				continue;
			alphabet.push_back(symbol);
		}
		alphabet.push_back(' ');
		alphabet.push_back(',');
		alphabet.push_back('.');
		alphabet.push_back('-');
		alphabet.push_back('!');
		alphabet.push_back('?');
		alphabet.push_back(';');
		alphabet.push_back(':');
	}
}

int symbol_number(char symbol)
{
	for (int i = 0; i < alphabet.size(); i++)
		if (alphabet[i] == tolower(symbol))
			return i + 10;
	return -1;
}

int radix_f(int n)
{
	int res = 0;
	while (n > 0)
	{
		res++;
		n /= 10;
	}
	return res;
}

int mod_exp(int a, int x, int p)
{
	unsigned long long y = 1;
	unsigned long long s = a;
	int x_2;
	while (x >= 1)
	{
		//опеределение очередного разраяда двоичного представления степени
		x_2 = x % 2;
		x /= 2;
		//быстрое возведение в степень по модулю справа-налево
		if (x_2 == 1)
			y = y * s % p;
		s = s * s % p;
	}
	return y;
}
//возвращает d: e*d mod (p-1)(q-1) = 1
int Euclidean_alg(int e, int p, int q)
{
	int V[2] = { (p - 1) * (q - 1), 0 };
	int U[2] = { e, 1 };
	int T[2];
	int qq = 0;
	while (V[0] != 0)
	{
		qq = U[0] / V[0];
		T[0] = U[0] % V[0];
		T[1] = U[1] - qq * V[1];
		U[0] = V[0]; U[1] = V[1];
		V[0] = T[0]; V[1] = T[1];
	}
	return U[1];
}

void init(int user, int* key, string& in_path, string& out_path)
{
	int p = 233;
	int q = 167;
	int e = 5;
	// вычисляется n
	// произведение p * q называется модулем
	key[1] = p * q;
	if (user == 1)
	{
		key[0] = e;
		in_path = "encryption_in.txt";
		out_path = "encryption_out.txt";
	}
	else
	{
		key[0] = Euclidean_alg(e, p, q);
		in_path = "encryption_out.txt";
		out_path = "decryption_out.txt";
	}
}

void encryption(string in_path, string out_path, int* key)
{
	ifstream f_in; f_in.open(in_path, ios::in | ios::binary);
	ofstream f_out; f_out.open(out_path, ios::out | ios::binary);
	if (!f_in.is_open())
	{
		cout << "error: f_in" << endl;
		return;
	}
	else if (!f_out.is_open())
	{
		cout << "error: f_out" << endl;
		return;
	}
	else
	{
		int radix = radix_f(key[1]);
		char symbol;
		queue<int> q;
		unsigned long long iter, res;
		int number = 0; //
		//перевод символов открытого текста в номера символов
		while (f_in.get(symbol))
		{
			if (symbol == '\n' || symbol == '\r')
				continue;

			number = symbol_number(symbol);
			if (number == -1)
			{
				cout << "error: unknown symbol " << symbol;
				return;
			}

			q.push(number / 10);
			q.push(number % 10);
		}
		iter = q.front(); q.pop();

		while (!q.empty())
		{

			for (int i = 1; i < radix; i++)
			{
				if (q.empty())
					break;
				iter = iter * 10 + q.front();
				//если текущее число больше модуля - берем на 1 разряд меньше
				if (iter > key[1])
				{
					iter /= 10;
					break;
				}
				q.pop();
			}
			//если следующая цифра следующего числа для шифрования - 0,
			// то избавляемся от последнего разряда в текущем числе и считаем результат
			if (!q.empty() && q.front() == 0)
			{
				res = mod_exp(iter / 10, key[0], key[1]);
				iter %= 10;
			}
			//иначе считаем результат и считываем следующую цифру следующего числа
			else
			{
				res = mod_exp(iter, key[0], key[1]);
				if (!q.empty())
				{
					iter = q.front();
					q.pop();
				}
			}
			//запись очередного результата в выходной файл через разделитель
			f_out << res;
			f_out << '\n';
		}
	}
	f_in.close();
	f_out.close();
}

void decryption(string in_path, string out_path, int* key)
{
	ifstream f_in; f_in.open(in_path, ios::in | ios::binary);
	ofstream f_out; f_out.open(out_path, ios::out | ios::binary);
	if (!f_in.is_open())
	{
		cout << "error: f_in" << endl;
		return;
	}
	else if (!f_out.is_open())
	{
		cout << "error: f_out" << endl;
		return;
	}
	else
	{
		char symbol;
		string res = "";
		unsigned long long crypt = 0;
		int i;

		while (f_in.get(symbol))
		{

			if (symbol == '\n')
			{

				res += to_string(mod_exp(crypt, key[0], key[1]));

				for (i = 0; i < res.size(); i++)
				{
					//если в числе нечетное количество разрядов - не обрабатываем последнюю цифру
					if (i + 1 == res.size())
						break;
					//запись символа в выходной файл
					f_out << alphabet[(res[i] - '0') * 10 + (res[i + 1] - '0') - 10];
					i++;
				}
				//если обработаны не все цифры - запоминаем последнюю
				//иначе очищаем строку
				if (i + 1 == res.size())
					res = res[i];
				else
					res = "";
				crypt = 0;
				continue;
			}
			//если встретилась цифра - добавляем к числу
			crypt = crypt * 10 + (symbol - '0');
		}
	}
	f_in.close();
	f_out.close();
}

void RSA(int user)
{
	int key[2] = { 0, 0 };
	string in_path, out_path;
	init(user, key, in_path, out_path);
	if (user == 1)
		encryption(in_path, out_path, key);
	else
		decryption(in_path, out_path, key);
}

int main(int argc, char* argv[])
{
	system("chcp 1251");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);




	create_alphabet();
	int user;
	cout << endl << "enter method: \n 1 - to use open key(encrypt) \n 2 - to use close key(decrypt)\n"; cin >> user;
	if (user != 1 && user != 2)
	{
		cout << endl << "unknown method";
		return -1;
	}
	RSA(user);
	return 0;
}