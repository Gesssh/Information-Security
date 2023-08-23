// напишите программу, позволяющую защифровать и расшифровать
//сообщения с использованием таблицы тритемия. входные и выходные данные записать в текст док

#include <fstream>
#include <string>
#include <iostream>
#include <ostream>
#include <math.h>
#include <algorithm>
#include <cctype>

using namespace std;


int main()
{
    setlocale(LC_ALL, "Russian");
    char alf[36]{ 'а','б','в','г','д','е','ё','ж','з','и','й','к','л','м','н','о','п','р','с','т','у','ф','х','ц','ч','ш','щ','ъ','ы','ь','э','ю','я',' ',',','.' };
    int z;
    cout << "Выбирите действие:\n1) Защифровать сообщение.\n2) Расшифровать сообщение.\n";
    cin >> z;
    if (z > 2 || z < 1)
    {
        cout << "Ошибка ввода данных..";
    }
    else
    {
        if (z == 1)
        {
            int count = 0, m, L, t = 0;
            char message[100];
            ifstream in("F_in.txt");
            ofstream out("F_out.txt");
            while (!in.eof()) { in.get(message[t]); t++; }
            
            while (count < t - 1)
            {
                for (int i = 0; i < t - 1; i++)
                {
                    if (message[count] == alf[i])
                    {
                        m = i;
                        break;
                    }
                }
                int k = 2 * count * count + 5 * count + 3;
                L = (m + k) % 36; count++;
                out << alf[L];
            }
            in.close();
            out.close();
        }
        else
        {
            int count = 0, m, L, t = 0;
            char message[100];
            ifstream in("F_out.txt");
            ofstream out("F_in.txt");
            while (!in.eof()) { in.get(message[t]); t++; }

            while (count < t - 1)
            {
                for (int i = 0; i < t - 1; i++)
                {
                    if (message[count] == alf[i])
                    {
                        m = i;
                        break;
                    }
                }
                int k = 2 * count * count + 5 * count + 3;
                int rez = m - k;
                if (rez < 0)
                {
                    while (rez < 0)
                    {
                        rez = rez + 36;
                    }
                }
                L = rez % 36; count++;
                out << alf[L];
            }
            in.close();
            out.close();
        }
            
    }
}
    


