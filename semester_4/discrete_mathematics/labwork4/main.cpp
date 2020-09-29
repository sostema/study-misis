#include <cstdio>
#include <iostream>

// Лабораторная работа №4
// Соседка Артём Валерьевич БПМ-18-2
// Порождение разбиений и композиций

void partitions_with_break(int number) {
  int j = 0;
  int sum = 0;
  int first_elem = 0;
  int min_elem = 0;


  /*Массив целых.*/
  int a[100]= {};
  for (j=0; j !=number; j++) a[j]=1;
  a[j]=0;


  int n = 0; /*Число объектов (первоначально) в массиве А*/
  for (; a[n] != 0; n++);
  /*Число объектов (в текущий момент) в массиве А*/
  int x = 0;

  while (true)
  {
    /*Обнулим переменную.*/
    min_elem = 0;

    /*Печать и выход.*/
    for (j=0; a[j] != 0; j++) printf("%d ", a[j]);
    printf("\n");
    if (a[0] == n) break;

    /*Элемент в нулевом индексе нашего динамического
    массива на текущий момент.*/
    first_elem = a[0];

    number = 0;

    for (x=0; a[x] != 0; x++);
    while (number != x -1)
    {

      /*Найдем элемент меньше первого.*/
      if (a[number] < first_elem)
      {
        first_elem = a[number];
        min_elem = number;
      }

      number++;
    }

    /*Перенос элемента  "1".*/
    a[min_elem]+= 1;
    a[x - 1]-= 1;
    /*Обрежем А*/
    a[min_elem+1] = 0;


    /*Добавим в массив единицы заново.*/
    for (j = min_elem+1; j != n; j++) a[j] = 1;
    a[j-1] = 0;

    /*Найдем сумму  элементов и обрежем массив А.*/
    sum = 0;
    for (j = 0; a[j] != 0; j++)
    {
      sum = sum + a[j];
      if (sum == n) a[j+1] = 0;
    }

  }
}

void partitions_without_break(int number) {
  int j = 0;
  int sum = 0;
  int first_elem = 0;
  int min_elem = 0;
  int a[100]= {}; 		/*Массив*/
  int n = 0;     		/*Число всех объектов в массиве а*/
  int x = 0;			/*Число объектов на опред. итерации.*/


  /*Массив целых.*/
  for (j=0; j !=number; j++) a[j]=1;
  a[j]=0;


  /*Число объектов (первоначально) в массиве А*/
  n=j;
  /*Число объектов (в текущий момент) в массиве А*/

  do
  {
    /*Обнулим переменную. */
    min_elem = 0;

    /*Печать и выход.*/
    for (j=0; a[j] != 0; j++) printf("%d ", a[j]);
    printf("\n");


    /*Элемент в нулевом индексе нашего динамического
    массива на текущий момент.*/
    first_elem = a[0];

    number = 0;

    for (x=0; a[x] != 0; x++);
    while (number != x -1)
    {

      /*Найдем элемент меньше первого.*/
      if (a[number] < first_elem)
      {
        first_elem = a[number];
        min_elem = number;
      }

      number++;
    }

    /*Перенос элемента  "1". */
    a[min_elem]+= 1;
    a[x - 1]-= 1;
    /*Обрежем А*/
    a[min_elem+1] = 0;


    /*Добавим в массив единицы заново.*/
    for (j = min_elem+1; j != n; j++) a[j] = 1;
    a[j-1] = 0;

    /*Найдем сумму элементов и обрежем массив А.*/
    sum = 0;
    for (j = 0; a[j] != 0; j++)
    {
      sum = sum + a[j];
      if (sum == n) a[j+1] = 0;
    }

  }
  while (a[0] != n);
  printf("%d\n", n);
}

/*Функция перестановок.*/
int permute(int * argv) {

  int k = 0;
  int number = 0;
  int j = 0;
  int x = 0; //Хранит длину строки.
  int c = 0; //Для обмена.
  int reverse_argv[100] = {};


  /*Упорядочим алфавит*/
  for (; argv[x]; x++);

  for (number = 0; number < x; number++) {
    for (j = x - 1; j > number; j--) {
      if (argv[j - 1] > argv[j]) {
        c = argv[j - 1];
        argv[j - 1] = argv[j];
        argv[j] = c;
      }
    }
  }

  /*Перевернем массив, чтобы остановить его позже в цикле*/
  number = x - 1;
  while (k < x) {
    reverse_argv[k] = argv[number];
    number--;
    k++;
  }

  /*Порождаем перестановки*/

  while (true) {

    for (j = 0; argv[j] != 0; j++) printf("%d ", argv[j]);
    printf("\n");

    for (k = 0; k != x + 1; k++) {
      if (reverse_argv[k] == argv[k] && argv[k] == 0) return 0;
      if (reverse_argv[k] != argv[k]) break;
    }

    number = x - 2;
    /*Ищем новую перестановку*/
    while (argv[number] >= argv[number + 1]) number--;
    j = x - 1;
    while (argv[j] <= argv[number]) j--;

    /*Обмен*/
    c = argv[j];
    argv[j] = argv[number];
    argv[number] = c;
    number++;
    /*Оборачиваем хвост*/
    for (j = x - 1; j > number; number++, j--) {
      c = argv[number];
      argv[number] = argv[j];
      argv[j] = c;
    }

  }
}

void compositions(int number) {
  int j = 0;
  int n = 0;
  int x = 0; 		/*Число объектов (в текущий момент) в массиве А*/
  int sum = 0;
  int first_elem = 0;
  int min_elem = 0;


  /*Массив целых.*/
  int a[100] = {};
  for (j = 0; j != number; j++) a[j] = 1;
  a[j] = 0;



  /*Число объектов (первоначально) в массиве А*/
  for (; a[n] != 0; n++);


  while (true) {
    /*Обнулим переменную.*/
    min_elem = 0;

    /*Печать и выход.*/

    permute(a);
    if (a[0] == n) break;

    /*Элемент в нулевом индексе нашего динамического
    массива на текущий момент.*/
    first_elem = a[0];

    number = 0;

    for (x = 0; a[x] != 0; x++);
    while (number != x - 1) {

      /*Найдем элемент меньше первого.*/
      if (a[number] < first_elem) {
        first_elem = a[number];
        min_elem = number;
      }

      number++;
    }

    /*Перенос элемента  "1".*/
    a[min_elem] += 1;
    a[x - 1] -= 1;
    /*Обрежем А*/
    a[min_elem + 1] = 0;


    /*Добавим в массив единицы заново.*/
    for (j = min_elem + 1; j != n; j++) a[j] = 1;
    a[j - 1] = 0;

    /*Найдем сумму элементов и обрежем массив А.*/
    sum = 0;
    for (j = 0; a[j] != 0; j++) {
      sum = sum + a[j];
      if (sum == n) a[j + 1] = 0;
    }

  }
}

int main (int argc, char *argv[]) {
  printf("Порождение разбиений\n");
  int n = 0;
  std::cin >> n;
  partitions_without_break(n);
  printf("Композиции\n");
  std::cin >> n;
  compositions(5);
}
