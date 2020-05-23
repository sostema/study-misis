#include <iostream>
#include <string>
#include <cmath>

// Лабораторная работа №1
// Соседка Артём Валерьевич БПМ-18-2
// Системы счисления. Операции в системах счисления.

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
  int X = 0, Y = 0;
  string strx, stry;
  cin >> X >> strx >> Y;
  cout << endl;

  int value = 0, value1 = 0, value2 = 0;

  while( !strx.empty() )
  {
    value = (X * value) + digitToValue(strx[0]);
    strx.erase(0, 1);
  }

  value1 = value;

  while(value)
  {
    stry = valueToDigit(value % Y) + stry;
    value /= Y;
  }

  cout << stry << endl;

  strx.clear();
  stry.clear();

  cin >> X >> strx >> Y;
  cout << endl;

  value = 0;
  while( !strx.empty() )
  {
    value = (X * value) + digitToValue(strx[0]);
    strx.erase(0, 1);
  }
  value2 = value;

  while(value)
  {
    stry = valueToDigit(value % Y) + stry;
    value /= Y;
  }
  cout << stry << endl;
  cin >> Y;

  strx.clear();
  stry.clear();

  value = abs(value1-value2);
  while(value)
  {
    stry = valueToDigit(value % Y) + stry;
    value /= Y;
  }
  cout << stry << endl;

  return 0;
}
