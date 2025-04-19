#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>

#define ERROR(msg) { fprintf(stderr, "Ошибка: %s\n", msg); exit(1); }

typedef struct Node {
    char *data;
    struct Node *next;
} Node;

typedef struct {
    Node *head;
    int size;
} LinkedList;

Node *create_node(const char *data) {
    Node *new_node = (Node *) malloc(sizeof(Node));
    if (!new_node)
        ERROR("Не удалось выделить память для нового узла");

    new_node->data = strdup(data);
    if (!new_node->data) {
        free(new_node);
        ERROR("Не удалось выделить память для копии данных узла");
    }

    new_node->next = NULL;
    return new_node;
}

void init_list(LinkedList *list) {
    list->head = NULL;
    list->size = 0;
}

void free_list(LinkedList *list) {
    Node *curr = list->head;
    while (curr) {
        Node *next = curr->next;
        free(curr->data);
        free(curr);
        curr = next;
    }
    list->head = NULL;
    list->size = 0;
}

void push_front(LinkedList *list, const char *data) {
    Node *new_node = create_node(data);
    new_node->next = list->head;
    list->head = new_node;
    list->size++;
}

void push_back(LinkedList *list, const char *data) {
    Node *new_node = create_node(data);
    if (!list->head) {
        list->head = new_node;
    } else {
        Node *curr = list->head;
        while (curr->next)
            curr = curr->next;
        curr->next = new_node;
    }
    list->size++;
}

void pop_front(LinkedList *list) {
    if (!list->head) return;

    Node *curr = list->head;
    list->head = list->head->next;
    free(curr->data);
    free(curr);
    list->size--;
}

void pop_back(LinkedList *list) {
    if (!list->head) return;

    if (!list->head->next) {
        free(list->head->data);
        free(list->head);
        list->head = NULL;
        list->size = 0;
        return;
    }

    Node *curr = list->head;
    Node *prev = NULL;

    while (curr->next != NULL) {
        prev = curr;
        curr = curr->next;
    }

    free(curr->data);
    free(curr);
    prev->next = NULL;
    list->size--;
}

void swap(LinkedList *list, int i, int j) {
    if (i < 0 || j < 0 || i >= list->size || j >= list->size || i == j)
        return;

    Node *prev_i = NULL, *prev_j = NULL, *node_i = list->head, *node_j = list->head;
    for (int k = 0; k < i; k++) {
        prev_i = node_i;
        node_i = node_i->next;
    }
    for (int k = 0; k < j; k++) {
        prev_j = node_j;
        node_j = node_j->next;
    }

    if (prev_i) prev_i->next = node_j;
    else list->head = node_j;

    if (prev_j) prev_j->next = node_i;
    else list->head = node_i;

    Node *temp = node_j->next;
    node_j->next = node_i->next;
    node_i->next = temp;
}

void dump(LinkedList *list, const char *filename) {
    FILE *file = filename ? fopen(filename, "w") : stdout;
    if (!file) {
        perror("Ошибка открытия файла");
        return;
    }

    Node *current = list->head;
    while (current) {
        fprintf(file, "%p %p %s\n", (void *) current, (void *) current->next, current->data);
        current = current->next;
    }

    if (file != stdout && file != NULL)
        fclose(file);
}

int info() {
    printf("Вариант: 1-10-17-5.\n");
    return 0;
}

const char *get_utf(const char *str, int n) {
    int count = 0;
    while (*str) {
        if ((*str & 0xC0) != 0x80) {
            // новый символ
            if (count == n) return str;
            count++;
        }
        str++;
    }
    return NULL;
}

bool is_valid_num(const char *str) {
    int char_count = 0;
    for (const char *p = str; *p; p++) {
        if ((*p & 0xC0) != 0x80) char_count++;
    }

    if (char_count != 11) {
        return false;
    }

    const char *rus_pos = get_utf(str, 8);
    if (!rus_pos || strncmp(rus_pos, "RUS", 3) != 0) {
        return false;
    }

    const char *digit1 = get_utf(str, 1);
    const char *digit2 = get_utf(str, 2);
    const char *digit3 = get_utf(str, 3);
    const char *digit4 = get_utf(str, 6);
    const char *digit5 = get_utf(str, 7);

    if (!isdigit(*digit1) || !isdigit(*digit2) || !isdigit(*digit3) || !isdigit(*digit4) || !isdigit(*digit5)) {
        return false;
    }

    const char *let1 = get_utf(str, 0);
    const char *let2 = get_utf(str, 4);
    const char *let3 = get_utf(str, 5);

    const char *allowed_chars = "АВЕКМНОРСТУХ";
    if (!strchr(allowed_chars, *let1) || !strchr(allowed_chars, *let2) || !strchr(allowed_chars, *let3)) {
        return false;
    }

    return true;
}

void trim(char *str) {
    if (str == NULL || *str == '\0') {
        return;
    }

    char *start = str;
    while (*start != '\0' && isspace((unsigned char)*start)) {
        start++;
    }

    if (*start == '\0') {
        *str = '\0';
        return;
    }

    char *end = str + strlen(str) - 1;
    while (end > start && isspace((unsigned char)*end)) {
        end--;
    }

    if (start != str) {
        memmove(str, start, end - start + 2);
    }
    str[end - start + 1] = '\0';
}

bool is_num(char *str) {
    for (size_t i = 0; i < strlen(str); ++i)
        if (str[i] < '0' || str[i] > '9')
            return 0;
    return 1;
}

uint16_t to_index(char *str) {
    uint16_t result = 0;
    for (size_t i = 0; i < strlen(str); ++i) {
        result *= 10;
        result += str[i] - '0';
    }
    return result;
}

int main(int argc, char *argv[]) {
    FILE *file = NULL;
    char *filename = NULL;

    if (argc < 2) {
        fprintf(stderr, "Ошибка: не был передан обязательный параметр.\n");
        return 1;
    }

    if (!strcmp(argv[1], "-v"))
        return info();
    else if (argv[1][0] == '-') {
        fprintf(stderr, "Ошибка: опция не поддерживается.\n");
        return 2;
    } else {
        filename = argv[1];
    }

    LinkedList list;
    init_list(&list);

    uint32_t cnt;
    file = fopen(filename, "rb");
    if (file != NULL) {
        if (fread(&cnt, sizeof(cnt), 1, file) == 1) {
            uint16_t *indices = (uint16_t *) malloc(sizeof(uint16_t) * cnt);
            uint32_t *offsets = (uint32_t *) malloc(sizeof(uint32_t) * cnt);
            uint32_t *sizes = (uint32_t *) malloc(sizeof(uint32_t) * cnt);
            char **strings = (char **) malloc(sizeof(char *) * cnt);

            if (!indices || !offsets || !sizes || !strings) {
                fprintf(stderr, "Ошибка выделения памяти.\n");
                fclose(file);
                return 4;
            }

            if (fread(indices, sizeof(uint16_t), cnt, file) != cnt ||
                fread(offsets, sizeof(uint32_t), cnt, file) != cnt ||
                fread(sizes, sizeof(uint32_t), cnt, file) != cnt) {
                fprintf(stderr, "Ошибка чтения данных из файла.\n");
                free(indices);
                free(offsets);
                free(sizes);
                free(strings);
                fclose(file);
                return 1;
            }

            for (uint16_t i = 0; i < cnt; ++i) {
                char buffer[1024];
                memset(buffer, 0, sizeof(buffer));
                fseek(file, offsets[i], SEEK_SET);
                size_t bytes_read = fread(buffer, 1, sizes[i], file);

                if (bytes_read != sizes[i]) {
                    fprintf(stderr, "Ошибка чтения данных из файла.\n");
                    continue;
                }
                buffer[bytes_read] = '\0';
                trim(buffer);
                strings[i] = strdup(buffer);
                if (!strings[i]) {
                    fprintf(stderr, "Ошибка выделения памяти для строки.\n");
                    for (uint16_t j = 0; j < i; ++j)
                        free(strings[j]);

                    free(indices);
                    free(offsets);
                    free(sizes);
                    free(strings);
                    fclose(file);
                    return 4;
                }
                push_back(&list, strings[i]);
                free(strings[i]);
            }
            free(indices);
            free(offsets);
            free(sizes);
            free(strings);
        } else {
            fprintf(stderr, "Файл пустой.\n");
            init_list(&list);
        }
        fclose(file);
    } else {
        perror("Ошибка открытия файла");
    }
       char command[256];
        bool done = false;
    while (!done) {
        if (fgets(command, sizeof(command), stdin) == NULL) {
            if (feof(stdin)) {
                printf("Конец ввода (Ctrl+D).\n");
                done = true; // устанавливаем флаг для выхода
            } else {
                fprintf(stderr, "Ошибка чтения из stdin.\n");
                free_list(&list);
                return 1;
            }
          continue;
        }
        int len = strcspn(command, "\n");
        if (len >= (int)sizeof(command)) {
            fprintf(stderr, "Ошибка: команда слишком длинная (максимум %zu символов).\n", sizeof(command) - 1);
            continue;
        }
        command[len] = '\0';
        trim(command);

        char *command_copy = strdup(command);
        if (command_copy == NULL) {
            perror("strdup failed");
            return 1;
        }

        char *token = strtok(command_copy, " "); // получаем первый токен
        if (token == NULL) {
            memset(command, 0, sizeof(command));
            free(command_copy);
            continue;
        }

        bool is_push_f = false, is_push_b = false, is_swap = false, is_dump = false;

        if (strcmp(token, "push_front") == 0) is_push_f = true;
        else if (strcmp(token, "push_back") == 0) is_push_b = true;
        else if (strcmp(token, "pop_front") == 0) pop_front(&list);
        else if (strcmp(token, "pop_back") == 0) pop_back(&list);
        else if (strcmp(token, "dump") == 0) is_dump = true;
        else if (strcmp(token, "swap") == 0) is_swap = true;
        else {
            fprintf(stderr, "Ошибка: неподдерживаемая команда: '%s'.\n", token);
          free(command_copy);
            continue; // переходим к следующей строке
        }

        // обработка аргументов в зависимости от команды
        if (is_push_f || is_push_b) {
            while ((token = strtok(NULL, " ")) != NULL) {
                trim(token);
                if (is_valid_num(token)) {
                    if (is_push_f) push_front(&list, token);
                    else push_back(&list, token);
                } else {
                    fprintf(stderr, "Ошибка: некорректный формат автомобильного номера: '%s'.\n", token);
                }
            }
        } else if (is_swap) {
            char *k_str = strtok(NULL, " ");
            char *l_str = strtok(NULL, " ");
            if (k_str != NULL && l_str != NULL && is_num(k_str) && is_num(l_str)) {
                char *endptr_k, *endptr_l;
                errno = 0; // cбрасываем errno перед вызовом strtol
                long k = strtol(k_str, &endptr_k, 10);
                long l = strtol(l_str, &endptr_l, 10);

                // проверяем наличие ошибок при преобразовании
                if (errno != 0 || *endptr_k != '\0' || *endptr_l != '\0') {
                    fprintf(stderr, "Ошибка: некорректные индексы для swap (ошибка преобразования).\n");
                } else {
                    // проверяем, что индексы находятся в допустимом диапазоне
                    if (k >= 0 && k < list.size && l >= 0 && l < list.size) {
                        swap(&list, (int)k, (int)l);
                    } else {
                        fprintf(stderr, "Ошибка: некорректные индексы для swap (вне диапазона).\n");
                    }
                }
            } else {
                fprintf(stderr, "Ошибка: некорректный формат команды swap.\n");
            }
        } else if (is_dump) {
            char *dump_filename = strtok(NULL, " ");
           if (dump_filename == NULL || strlen(dump_filename) == 0) {
                // если файл не указан, выводим дамп в консоль
                dump(&list, NULL);
            } else {
                dump(&list, dump_filename);
            }
        }
    free(command_copy);
    }

  file = fopen(filename, "wb");
    if (!file) {
        perror("Ошибка открытия файла для записи");
        free_list(&list);
        return 1;
    }
    uint32_t size = list.size;

    //запись cnt
    if (fwrite(&size, sizeof(uint32_t), 1, file) != 1) {
        perror("Ошибка записи cnt в файл");
        fclose(file);
        free_list(&list);
        return 1;
    }

    // выделение памяти для массивов индексов, смещений и размеров
    uint16_t *indices = (uint16_t *)malloc(sizeof(uint16_t) * size);
    uint32_t *offsets = (uint32_t *)malloc(sizeof(uint32_t) * size);
    uint32_t *sizes = (uint32_t *)malloc(sizeof(uint32_t) * size);

    if (!indices || !offsets || !sizes) {
        fprintf(stderr, "Ошибка выделения памяти для индексов/смещений/размеров.\n");
        fclose(file);
        free_list(&list);
        free(indices);
        free(offsets);
        free(sizes);
        return 1;
    }

    // заполнение массивов и вычисление смещений
    Node *current = list.head;
    uint32_t current_offset = sizeof(uint32_t) + // cnt
                             sizeof(uint16_t) * size + // indices
                             sizeof(uint32_t) * size + // offsets
                             sizeof(uint32_t) * size; // sizes

    for (uint32_t i = 0; i < size; ++i) {
        indices[i] = i; // индексы просто по порядку (0, 1, 2, ...)
        sizes[i] = strlen(current->data) + 1; // +1 для \n
        offsets[i] = current_offset;
        current_offset += sizes[i];
        current = current->next;
    }

    // запись массивов в файл
    if (fwrite(indices, sizeof(uint16_t), size, file) != size ||
        fwrite(offsets, sizeof(uint32_t), size, file) != size ||
        fwrite(sizes, sizeof(uint32_t), size, file) != size) {
        perror("Ошибка записи массивов индексов/смещений/размеров в файл");
        fclose(file);
        free_list(&list);
        free(indices);
        free(offsets);
        free(sizes);
        return 1;
        }

    // запись строк в файл
    current = list.head;
    for (uint32_t i = 0; i < size; ++i) {
          if (fwrite(current->data, sizeof(char), strlen(current->data), file) != strlen(current->data)) {
              perror("Ошибка записи строки в файл");
              fclose(file);
              free_list(&list);
              free(indices);
              free(offsets);
              free(sizes);
              return 1;
          }
        fprintf(file,"\n");
        current = current->next;
    }

    fclose(file);
    free_list(&list);
    free(indices);
    free(offsets);
    free(sizes);
    return 0;
}
