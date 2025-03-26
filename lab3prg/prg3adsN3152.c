#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h> // Для atoi

#define RED "\033[31m"
#define RESET "\033[0m"

// Функция для вывода версии программы
void print_version() {
    printf("Свиржевский Алексей Денисович, гр. N3152\nВариант: 11-1-4-1\n");
}

bool is_number(const char *str) {
    if (!str || *str == '\0') return false;
    while (*str) {
        if (!isdigit(*str)) return false;
        str++;
    }
    return true;
}

// Функция для очистки номера от лишних символов (\n, пробелов)
void clean_number(const char *original, char *cleaned) {
    size_t j = 0;
    size_t length = strlen(original);
    for (size_t i = 0; i < length && j < 255; i++) {
        if (isdigit(original[i]) || original[i] == '+' || original[i] == '(' ||
            original[i] == ')' || original[i] == '-') {
            cleaned[j++] = original[i];
        }
    }
    cleaned[j] = '\0'; // Завершаем строку
}

// Функция для подсчёта количества строк в тексте
int count_lines(const char *text) {
    int lines = 0;
    for (size_t i = 0; text[i] != '\0'; i++) {
        if (text[i] == '\n') {
            lines++;
        }
    }
    // Если текст не пуст и последний символ не '\n', то добавляем строку
    if (text[0] != '\0' && text[strlen(text) - 1] != '\n') {
        lines++;
    }
    return lines;
}

// Проверка, является ли строка корректным номером телефона
bool is_phone_number(const char *str, size_t len) {
    size_t digit_count = 0;
    bool plus_seen = false;
    bool open_bracket = false;
    bool def_zone = false;
    int dash_positions = 0;


    for (size_t i = 0; i < len; i++) {
        char c = str[i];

        if (isdigit(c)) {
            digit_count++;
        } else if (c == '+') {
            if (i != 0 || plus_seen) return false; // "+" только в начале
            plus_seen = true;
        } else if (c == '(') {
            if (open_bracket || def_zone) return false; // Только одна открывающая скобка до кода зоны
            open_bracket = true;
        } else if (c == ')') {
            if (!open_bracket) return false; // Скобка должна быть открыта
            open_bracket = false;
        } else if (c == '-') {
            // Проверяем, что "-" стоит только перед предпоследней и последней парой цифр
            if (digit_count != 7 && digit_count != 9) return false;
            dash_positions++;
        } else {
            return false; // Недопустимый символ
        }
        // DEF зона
        if (digit_count >= 2 && digit_count <= 4) {
            def_zone = true;
        } else {
            def_zone = false;
        }
    }
    if (str[0] != '8' && str[0] != '+') return false; // Если не начинается с 8 или +
    if (plus_seen && str[0] != '+') return false; // Если есть '+', он должен быть в начале
    if (dash_positions > 2 || dash_positions == 1) return false; // '-' может быть только дважды или отсутствовать

    return digit_count == 11;
}

// Функция для подсчета \n в промежутке
int cnt_lines(const char *text, size_t start, size_t end) {
    int newline_count = 0;
    for (size_t i = start; i < end; ++i) {
        if (text[i] == '\n') {
            newline_count++;
        }
    }
    return newline_count;
}


// Функция пересчета текущей строки
void rollback(size_t *i, size_t inspect_len, int *current_line, const char *text) {
    size_t start = *i - inspect_len + 1;
    size_t end = *i + 1;
    int remove = cnt_lines(text, start, end);
    *current_line -= remove;
    *i -= inspect_len - 1;
}

// Функция обработки текста
void text_processing(const char *text, FILE *output, bool color, bool single_line, int count_lines, int start,
                     int end) {
    size_t len = strlen(text);
    char phone_candidate[256] = ""; // Переменная для хранения потенциального номера
    char clean_candidate[256] = ""; // Очищенный номер
    size_t candidate_len = 0; // Длина потенциального номера
    size_t digit_count = 0; // Счётчик цифр
    bool multiline = false; // Флаг для отслеживания номера, который разделен переносами строк
    int current_line = 1; // Номер текущей строки
    bool highlight = false; // Флаг опций -b и -e

    // Проверяем, верно ли указан диапазон строк
    if (start > count_lines) {
        fprintf(stderr, "Ошибка: начальная строка выходит за пределы текста.\n");
        return;
    }
    if (end != -1 && end < start) {
        fprintf(stderr, "Ошибка: конечная строка меньше начальной.\n");
        return;
    }

    // Движемся посимвольному по всему нашему тексту
    for (size_t i = 0; i < len; ++i) {
        char current_char = text[i];
        // Номер текущей строки
        if (current_char == '\n') current_line++;
        if (current_line < start || (end != -1 && current_line > end)) {
            highlight = false;
        } else {
            highlight = true;
        }

        if (isdigit(current_char)) digit_count++;

        // Накапливаем потенциальный номер телефона
        if (isdigit(current_char) || current_char == '+' || current_char == '(' || current_char == ')' || current_char
            == '-') {
            phone_candidate[candidate_len++] = current_char;
        } else if (current_char == '\n' && digit_count >= 1 && digit_count < 11) {
            multiline = true;
            phone_candidate[candidate_len++] = '\n';
        } else {
            // Вывод
            if (candidate_len > 0 && highlight) {
                if (digit_count == 11) {
                    if (multiline) {
                        clean_number(phone_candidate, clean_candidate);
                        if (is_phone_number(clean_candidate, strlen(clean_candidate))) {
                            if (single_line) {
                                fprintf(output, "%s", phone_candidate);
                            } else {
                                if (color) {
                                    fprintf(output, "%s%s%s", RED, phone_candidate, RESET);
                                } else {
                                    fprintf(output, "~~%s~~", phone_candidate);
                                }
                            }
                        } else {
                            goto goback;
                        }
                    } else {
                        if (is_phone_number(phone_candidate, candidate_len)) {
                            if (color) {
                                fprintf(output, "%s%s%s", RED, phone_candidate, RESET);
                            } else {
                                fprintf(output, "~~%s~~", phone_candidate);
                            }
                        } else {
                            goto goback;
                        }
                    }
                } else {
                    goto goback;
                }
            null:
                digit_count = 0;
                memset(phone_candidate, '\0', 256);
                memset(clean_candidate, '\0', 256);
                multiline = false;
                candidate_len = 0;
                continue;
            } else if (!highlight) goto goback;
            fputc(current_char, output);
            goto null;
        }
        if (digit_count == 11 && candidate_len > 0 && highlight) {
            // Вывод
            if (multiline) {
                clean_number(phone_candidate, clean_candidate);
                if (is_phone_number(clean_candidate, strlen(clean_candidate))) {
                    if (single_line) {
                        fprintf(output, "%s", phone_candidate);
                    } else {
                        if (color) {
                            fprintf(output, "%s%s%s", RED, phone_candidate, RESET);
                        } else {
                            fprintf(output, "~~%s~~", phone_candidate);
                        }
                    }
                } else {
                    goto goback;
                }
            } else {
                if (is_phone_number(phone_candidate, candidate_len)) {
                    if (color) {
                        fprintf(output, "%s%s%s", RED, phone_candidate, RESET);
                    } else {
                        fprintf(output, "~~%s~~", phone_candidate);
                    }
                } else {
                    goto goback;
                }
            }
            // Обнуляем все
            goto null;
        } else if (!highlight) goto goback;

        if (i == len - 1 && candidate_len > 0) {
            fprintf(output, "%s", phone_candidate);
            if (candidate_len == 0) {
                fputc(current_char, output);
            }
            continue;
        }

        if (candidate_len > 0 && (isdigit(text[i + 1]) || text[i + 1] == '+' || text[i + 1] == '(' || text[i + 1] == ')'
                                  || text[i + 1] == '-' || text[i + 1] == '\n')) {
            continue;
        }
        // Откат назад на начало неправильного номера и проходимся заново со след элемента
    goback:
        if ((current_char == '\n' || (!isdigit(current_char) && current_char != '+' && current_char != '(' && current_char != ')' && current_char
             != '-')) && !highlight) {
            fputc(current_char, output);
        }
        fputc(text[i - candidate_len + 1], output);
        rollback(&i, candidate_len, &current_line, text);
        candidate_len = 0;
        digit_count = 0;
        memset(phone_candidate, '\0', 256);
        memset(clean_candidate, '\0', 256);
        multiline = false;
    }
}


int main(int argc, char *argv[]) {
    bool show_version = false, color = false, restrict_to_single_line = false;
    int start_line = 1, end_line = -1;
    char *input_file = NULL, *output_file = NULL;

    for (int arg_index = 1; arg_index < argc; arg_index++) {
        // Проверяем опции
        if (argv[arg_index][0] == '-') {
            if (strcmp(argv[arg_index], "-v") == 0) {
                show_version = true;
                continue;
            }
            if (strcmp(argv[arg_index], "-c") == 0) {
                color = true;
                continue;
            }
            if (strcmp(argv[arg_index], "-n") == 0) {
                restrict_to_single_line = true;
                continue;
            }
            if (strncmp(argv[arg_index], "-b=", 3) == 0) {
                start_line = atoi(argv[arg_index] + 3);
                if (start_line < 1) {
                    fprintf(stderr, "Ошибка: начальная строка должна быть >= 1\n");
                    return 1;
                }
                continue;
            }
            if (strncmp(argv[arg_index], "-e=", 3) == 0) {
                end_line = atoi(argv[arg_index] + 3);
                if (end_line < 1) {
                    fprintf(stderr, "Ошибка: конечная строка должна быть >= 1\n");
                    return 1;
                }
                continue;
            }

            // Если не совпало ни с одной из допустимых опций
            fprintf(stderr, "Ошибка: неподдерживаемая опция %s.\n", argv[arg_index]);
            return 1;
        }

        // Проверка файлов
        if (!input_file) {
            input_file = argv[arg_index];
            continue;
        }

        if (!output_file) {
            output_file = argv[arg_index];
            continue;
        }

        // Если уже заданы input_file и output_file
        fprintf(stderr, "Ошибка: лишний аргумент %s.\n", argv[arg_index]);
        return 1;
    }

    // Опция -b
    if (show_version) {
        print_version();
        return 0;
    }

    if (!input_file && !output_file) {
        // Работа с вводом с клавиатуры
        char line[1024];
        char input_buffer[10240];
        input_buffer[0] = '\0';
        // Чтение строк ввода до Ctrl+D
        int cnt_lines = 0;
        while (fgets(line, sizeof(line), stdin)) {
            if (line[0] == '\0' || line[0] == '\n') {
                break;
            }
            strcat(input_buffer, line);
            cnt_lines++;
        }
        // Обработка текста
        printf("\n\n");
        text_processing(input_buffer, stdout, color, restrict_to_single_line, cnt_lines, start_line, end_line);
        printf("\n");
        return 0;
    }

    if (output_file && strcmp(input_file, output_file) == 0) {
        fprintf(stderr, "Ошибка: входной и выходной файл не могут совпадать.\n");
        return 1;
    }

    FILE *input = fopen(input_file, "r");
    if (!input) {
        fprintf(stderr, "Ошибка: не удалось открыть файл %s для чтения.\n", input_file);
        return 1;
    }

    FILE *output = stdout;
    if (output_file) {
        output = fopen(output_file, "w");
        if (!output) {
            fprintf(stderr, "Ошибка: не удалось открыть файл %s для записи.\n", output_file);
            fclose(input);
            return 1;
        }
    }

    char buffer[10240] = {0};
    size_t total_read = fread(buffer, 1, sizeof(buffer) - 1, input);
    buffer[total_read] = '\0';
    int line_count = count_lines(buffer);

    text_processing(buffer, output, color, restrict_to_single_line, line_count, start_line, end_line);
    fputc('\n', output);

    if (input != stdin) fclose(input);
    if (output != stdout) fclose(output);

    return 0;
}
