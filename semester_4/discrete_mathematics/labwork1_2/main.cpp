#include <iostream>
#include <string>
#include <vector>
#include <cmath>

// Лабораторная работа №1_2
// Соседка Артём Валерьевич БПМ-18-2
// Переменные системы счисления.

using namespace std;

inline int digitToValue(char digit)
{
  return isalpha(digit) ? (digit - 'A' + 10) : (digit - '0');
}

inline char valueToDigit(int value)
{
  return static_cast<char>( (value < 10) ? (value + '0') : (value - 10 + 'A') );
}

int main()
{
  int n = 0;
  int buff = 0;
  vector<int> x;
  vector<int> y;
  string strx, stry;

  cin >> n;
  x.resize(n);
  for(int i = 0; i < n; ++i){
    cin >> buff;
    x[i] = buff;
  }

  cin >> n;
  y.resize(n);
  for(int i = 0; i < n; ++i){
    cin >> buff;
    y[i] = buff;
  }

  cin >> strx;

  int value = 0;
  ptrdiff_t counter = x.size()-1;
  while( !strx.empty() )
  {
    value = (x[counter] * value) + digitToValue(strx[0]);
    strx.erase(0, 1);
    counter--;
  }

  counter = y.size()-1;
  while(value)
  {
    stry = valueToDigit(value % y[counter]) + stry;
    counter--;
    if (counter < 0) {
      break;
    }
    value /= y[counter];
  }

  cout << stry << endl;

  return 0;
}
