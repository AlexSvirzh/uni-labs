#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <errno.h>
#include <stdbool.h>

//Перевод из десятичной сс в двоичную. Дописываем незначащие нули, чтобы число соответствовало размерам unsigned int (4 байта)
void dec_to_bin(unsigned int number) {
    for (int byte = 4 - 1; byte >= 0; byte--) {
        for (int bit = 7; bit >= 0; bit--) {
            if (number & (1U << (byte * 8 + bit))) {
                printf("1");
            } else {
                printf("0");
            }
        }
        //разделяем байты пробелами
        if (byte > 0) {
            printf(" ");
        }
    }
    printf("\n");
}

//считаем количество нулевых битов в байте 
unsigned int count_zero_bits(unsigned char byte) {
	unsigned int count = 0;
	for (int i = 0; i < 8; i++) {
		if ((byte & (1 << i)) == 0) {
            count++;
        }
    }
    return count;
}

//проверка на корректность ввода
bool is_corr(const char* num) {
    while (*num != '\0') {
        if (*num < '0' || *num > '9') {
            return false; 
        }
        num++; 
    }
    return true; 
}

//функция инвертирования подходящих битов
void bit_invertation(unsigned int *num) {
	unsigned char *bytes = (unsigned char *)num; //записываем число num по байтам
    unsigned int size[sizeof(unsigned int)];

    // Считаем количество нулевых битов в каждом байте числа и заносим в массив
    for (int i = 0; i < 4; i++) {
        size[i] = count_zero_bits(bytes[i]);
    }

	//Находим максимальную и минимальную характеристику(число нулевых битов)
	unsigned int min = UINT_MAX, max = 0;
    for (int i = 0; i < 4; i++) {
        if (size[i] <= min) {
            min = size[i];
        }
        if (size[i] >= max) {
            max = size[i];
        }
    }
    //Инвертируем чётные биты в подходящих байтах
    for (int i = 0; i < 4; i++) {
    	if (size[i] != min && size[i] != max) {
    		bytes[i] ^= 0xAA;
    	}
    }
}


int main(int argc, char *argv[]) {
	unsigned int num;
	srand(time(NULL));
	
	if (argc < 2) { //пользователь ничего не ввел(1 аргумент - это название программы)
		num = rand()%100000000000000000;
		printf("Вы ничего не ввели, поэтому я выбрал рандомное число:\n%u\n", num);
	}
	else {
        if(is_corr(argv[1]) == false) {
            printf("Ошибка, вы ввели некорректное число!\n");
            return 0; //Проверяем на корректность
        }
        else {
            if (sscanf(argv[1], "%u", &num) == EOF){ //end-of-file - аргумент не является числом
                printf("Ошибка, вы ввели не число!\n");
            }
        }
    }

	printf("Введённое число в двоичной системе счисления:\n");
	dec_to_bin(num);
	
	bit_invertation(&num);
	
	printf("Введённое число после преобразования:\n");
	dec_to_bin(num);
	
    return 0;
}


