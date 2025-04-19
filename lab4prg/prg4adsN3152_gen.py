#!/usr/bin/python3
import random
import string
import sys
import struct


def generate_random_number():
    letters = "авекмнорстух"
    region = random.randint(1, 99)  # диапазон региона
    region_str = str(region).zfill(2)
    plate = f"{random.choice(letters)}{random.randint(100, 999)}{random.choice(letters)}{random.choice(letters)}{region_str}RUS"
    return plate.encode('utf-8')  # кодируем в UTF-8


def generate_empty_file(filename):
    try:
        open(filename, 'wb').close()
        print(f"Создан пустой файл '{filename}'.")
        return 0
    except IOError as e:
        print(f"Ошибка: Не удалось создать пустой файл '{filename}': {e}")
        return 1


def generate_file(filename, count):
    if count == 0:
        return generate_empty_file(filename)

    strings = [generate_random_number() for _ in range(count)]
    num_strings = len(strings)
    # вычисляем смещения
    offsets = []
    curr_offset = 4 + 2 * num_strings + 4 * num_strings + 4 * num_strings  # cnt + indices + offsets + sizes
    for s in strings:
        offsets.append(curr_offset)
        curr_offset += len(s)

    try:
        file = open(filename, 'wb')
        # записываем cnt
        file.write(struct.pack('<I', num_strings))  # cnt - uint
        # записываем индексы
        for i in range(num_strings):
            file.write(struct.pack("<H", i))  # indices unsigned short

        # записываем смещения
        for offset in offsets:
            file.write(struct.pack("<I", offset))  # offsets - uint

        # записываем размеры строк
        for s in strings:
            file.write(struct.pack("<I", len(s)))  # sizes - uint

        # записываем строки
        for s in strings:
            file.write(s)

        file.close()

    except IOError as e:
        print(f"Ошибка: не удалось записать в файл '{filename}': {e}")
        return 1

        # выводим информацию о созданном файле
    print(f"Файл '{filename}' успешно создан с {num_strings} записями.")
    print("Содержимое файла:")
    for i in range(num_strings):
        print(f"Индекс: {i}, Смещение: {offsets[i]}, Размер: {len(strings[i])}, Строка: {strings[i].decode('utf-8')}")

    return 0

def info():
    print("Вариант: 1-10-17-5")
    return 0


def main():
    args = sys.argv[1:]

    if len(args) == 0:
        print("Ошибка: Необходимо указать имя файла.")
        return 1

    if "-v" in args:
        return info()

    filename = None
    num = None

    i = 0
    while i < len(args):
        if args[i] == '-n':
            if i + 1 < len(args):
                try:
                    num = int(args[i+1])
                    i += 2
                except ValueError:
                    print('Ошибка: неверный формат числа')
                    return 1
            else:
                print('Ошибка: необходимо указать число')
                return 2
        else:
            filename = args[i]
            i += 1

    if filename is None:
        print('Необходимо указать название файла')
        return 3

    if num is None:
        num = random.randint(10,1000)

    return generate_file(filename, num)


if __name__ == '__main__':
    main()
