#ifndef ASM_HW4_MAIN_H
#define ASM_HW4_MAIN_H

#include <fstream>
#include <iostream>
#include <pthread.h>
#include <queue>
#include <string>
#include <unistd.h>
#include <vector>

using namespace std;

#define COLORS 1
#define SECTIONS_NUM 3

#if COLORS == 1
// Набор цветов для консоли.
string ANSI_RESET = "\033[0m";
string ANSI_RED = "\033[31m";
string ANSI_GREEN = "\033[32m";
string ANSI_YELLOW = "\033[33m";
string ANSI_PURPLE = "\033[35m";
string ANSI_CYAN = "\033[36m";
#endif

// Пока покупатели не кончились, продавцы работают
bool flag = true;

// Режим ввода
string answer;

// Строка для вывода
string str;

// Накопительная переменная для вывода в файл
string cumulative;

// Мьютекс для защиты операции чтения
pthread_mutex_t mutex_0;
pthread_mutex_t mutex_1;
pthread_mutex_t mutex_2;


#endif //ASM_HW4_MAIN_H