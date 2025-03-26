#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <time.h>

// Подсчёт характеристики столбца
uint16_t xor_characteristic(uint16_t **matrix, uint16_t row, uint16_t column_index) {
    uint16_t xor_result = 0;

    for (uint16_t i = 0; i < row; i++) {
        xor_result ^= matrix[i][column_index];
    }
    return xor_result;
}

// Сдвиг вниз для столбца
void shift_down(uint16_t **matrix, uint16_t row, uint16_t column_index, uint16_t shift) {
    if (row == 0 || shift == 0) return; // Если количество строк равно 0 или сдвиг равен нулю, то ничего не делаем
    shift %= row; // Делаем для того, чтобы сдвиг не вышел за пределы количества столбцов

    uint16_t *temp_arr = (uint16_t *) malloc(shift * sizeof(uint16_t)); // Создаём временный массив

    // Копируем сдвигаемые элементы столбца в начало временного массива
    for (uint16_t i = 0; i < shift; i++) {
        temp_arr[i] = matrix[row - shift + i][column_index];
    }

    // Сдвигаем оставшиеся элементы столбца вниз
    for (int16_t i = row - 1; i >= shift; i--) {
        matrix[i][column_index] = matrix[i - shift][column_index];
    }

    // Копируем сохраненные элементы в верх столбца
    for (uint16_t i = 0; i < shift; i++) {
        matrix[i][column_index] = temp_arr[i];
    }

    free(temp_arr); // Освобождаем временный массив
}

// Функция для проверки ввода на корректность
uint16_t check_input(const char *input) {
    char *endptr;
    if (strchr(input, '-') != NULL) {
        fprintf(stderr, "Ошибка, '%s' является некорректным значением.\n", input);
        exit(EXIT_FAILURE);
    }
    uint16_t value = (uint16_t) strtoul(input, &endptr, 10); // Преобразуем строку в число

    // Проверяем, является ли введенное значение числом и положительным
    if (*endptr != '\0' || value == 0) {
        fprintf(stderr, "Ошибка, '%s' не является числом или является некорректным значением.\n", input);
        exit(EXIT_FAILURE);
    }

    return value;
}

int main(int argc, char *argv[]) {
    uint16_t row, col;
    //Проверка правильности количества аргументов
    if (argc < 2) {
        fprintf(stderr, "Ошибка: ничего не введено.\n");
        return EXIT_FAILURE;
    }

    if ((strchr(argv[1], 'm') == NULL || strchr(argv[1], '-') == NULL) && argc == 4) {
        fprintf(stderr, "Ошибка: опция '%s' не поддерживается\n", argv[1]);
        return EXIT_FAILURE;
    }

    if (strchr(argv[1], 'm') != NULL && argc == 3) {
        fprintf(stderr, "Ошибка: отсутствует обязательный аргумент.\n");
        return EXIT_FAILURE;
    }

    if (argc != 4 && argc != 3) {
        fprintf(stderr, "Не хватает аргументов.\n");
        return EXIT_FAILURE;
    }
    // Получаем количество строк и столбцов матрицы с проверкой на корректность ввода
    if (argc == 3) {
        row = check_input(argv[1]); // Получаем строки
        col = check_input(argv[2]); // Получаем столбцы
    } else if (argc == 4 && strcmp(argv[1], "-m") == 0) {
        row = check_input(argv[2]); // Получаем строки
        col = check_input(argv[3]); // Получаем столбцы
    }

    // Выделение памяти для матрицы
    uint16_t **matrix = (uint16_t **) malloc(row * sizeof(uint16_t *));
    for (uint16_t i = 0; i < row; i++) {
        matrix[i] = (uint16_t *) malloc(col * sizeof(uint16_t));
    }

    // Ручной ввод элементов матрицы, если флаг "-m" установлен
    if (argc == 4 && strcmp(argv[1], "-m") == 0) {
        printf("Введите элементы матрицы (%d строк и %d столбцов):\n", row, col);
        for (uint16_t i = 0; i < row; i++) {
            for (uint16_t j = 0; j < col; j++) {
                if (scanf("%hu", &matrix[i][j]) != 1) {
                    fprintf(stderr, "Ошибка ввода элемента матрицы.\n");
                    return EXIT_FAILURE;
                }
            }
        }
    } else {
        // Автоматическое заполнение матрицы
        printf("Флаг -m не установлен, матрица будет заполнена автоматически.\n");
        srand((uint16_t) time(NULL));
        for (uint16_t i = 0; i < row; i++) {
            for (uint16_t j = 0; j < col; j++) {
                matrix[i][j] = rand() % 65536; // Диапазон типа uint16_t
            }
        }
    }

    // Вывод матрицы
    printf("Исходная матрица:\n");
    for (uint16_t i = 0; i < row; i++) {
        for (uint16_t j = 0; j < col; j++) {
            printf("%hu ", matrix[i][j]);
        }
        printf("\n");
    }

    // Выполнение сдвига для каждого столбца
    for (uint16_t j = 0; j < col; j++) {
        // Вычисление характеристики (XOR) столбца
        uint16_t xor_result = xor_characteristic(matrix, row, j);
        uint16_t xor_six_bits = xor_result & 0x3F; // Извлечение младших 6 бит

        // Подсчёт количества единиц в младших 6 битах характеристики
        uint16_t shift = 0;
        for (uint16_t i = 0; i < 6; i++) {
            shift += (xor_six_bits >> i) & 1;
        }

        // Циклический сдвиг вниз
        shift_down(matrix, row, j, shift);
    }

    // Вывод матрицы после сдвига
    printf("Матрица после сдвига вниз:\n");
    for (uint16_t i = 0; i < row; i++) {
        for (uint16_t j = 0; j < col; j++) {
            printf("%hu ", matrix[i][j]);
        }
        printf("\n");
    }

    // Освобождение памяти
    for (uint16_t i = 0; i < row; i++) {
        free(matrix[i]);
    }
    free(matrix);

    return EXIT_SUCCESS;
}
