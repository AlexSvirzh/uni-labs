section .data
    input_value dd 0xDEADBEEF       ; Исходное значение (тип unsigned int => 4 байта)
    output_value dd 0               ; Результат преобразований 
    input_buffer db '00000000', 0xA ; Буфер для вывода исходного числа 
    output_buffer db '00000000', 0xA ; Буфер для вывода результата 

section .text
    global _start

_start:
    lea ebx, [input_value]      
    xor esi, esi                ; Индекс байта
    mov edx, 8                  ; Минимальная характеристика 
    xor edi, edi                ; Максимальная характеристика 
    xor eax, eax                ; Индекс минимального байта
    xor ebp, ebp                ; Индекс максимального байта
    xor esi, esi

find_min_max:
    cmp esi, 4
    je process_bytes           ; Если все байты обработаны, перейти к инвертированию

    movzx r8d, byte [ebx + esi] ; Текущий байт
    mov ecx, r8d                
    xor r9d, r9d                ; Счётчик нулевых битов

count_zeros:
    test ecx, 1
    jz zero_found ; Если 0 - увеличиваем счётчик
    jmp skip_zero

zero_found:
    inc r9d                     

skip_zero:
    shr ecx, 1                  
    test ecx, ecx               ; Проверяем, есть ли ещё биты
    jnz count_zeros

    ; Сравнение с минимальной характеристикой
    cmp r9d, edx
    jl update_min

    ; Сравнение с максимальной характеристикой
    cmp r9d, edi
    jg update_max
    jmp next_byte

update_min:
    mov edx, r9d                ; Обновляем минимальную характеристику
    mov eax, esi                ; Сохраняем индекс минимального байта
    jmp next_byte

update_max:
    mov edi, r9d                ; Обновляем максимальную характеристику
    mov ebp, esi                ; Сохраняем индекс максимального байта

next_byte:
    inc esi                     ; Переход к следующему байту
    jmp find_min_max

process_bytes:
    xor esi, esi                
    lea ebx, [input_value]      
    lea ecx, [output_value]     

invert_even_bits: ; Инвертируем чётные биты байта, следуя условию
    cmp esi, 4
    je convert          ; Если все байты обработаны, переход к выводу

    movzx r8w, byte [ebx + esi] ; Загружаем текущий байт
    cmp esi, eax                ; Если минимальный байт, пропускаем
    je skip_invert
    cmp esi, ebp                ; Если максимальный байт, пропускаем
    je skip_invert

    xor r8b, 0xAA                ; Инвертируем четные биты
    mov [ecx + esi], r8b
    inc esi
    jmp invert_even_bits

skip_invert:
    mov [ecx + esi], r8b
    inc esi                     ; Переход к следующему байту
    jmp invert_even_bits

convert:; Вывод исходного значения
    mov eax, [input_value]      
    mov ecx, input_buffer       
    mov edi, 32                 
    xor esi, esi                ; Индекс

convert_input_loop:
    test eax, 1                  ; Проверяем младший бит
    jz zero_bit_input            ; Если 0, записываем '0'
    mov byte [ecx + edi - 1], '1' ; Записываем '1'
    jmp next_bit_input

zero_bit_input:
    mov byte [ecx + edi - 1], '0' ; Записываем '0'

next_bit_input:
    shr eax, 1                   
    inc esi                      
    dec edi                      
    jnz convert_input_loop       ; Повторяем, пока не обработаем все 32 бита

    ; Добавляем новую строку
    mov byte [ecx + 32], 0xA     
    mov byte [ecx + 33], 0        ; Завершаем строку нулем

    ; Выводим исходное значение
    mov eax, 4                   
    mov ebx, 1                  
    mov edx, 34                  ; длина строки (32 бита + \n + 0)
    int 0x80

    ; Вывод результата
    mov eax, [output_value]      
    mov ecx, output_buffer       
    mov edi, 32                 
    xor esi, esi                ; Индекс

convert_output_loop:
    test eax, 1                  ; Проверяем младший бит
    jz zero_bit_output           ; Если 0, записываем '0'
    mov byte [ecx + edi - 1], '1' ; Записываем '1'
    jmp next_bit_output

zero_bit_output:
    mov byte [ecx + edi - 1], '0' ; Записываем '0'

next_bit_output:
    shr eax, 1                   
    inc esi                      
    dec edi                      
    jnz convert_output_loop      ; Повторяем, пока не обработаем все 32 бита

    ; Добавляем новую строку
    mov byte [ecx + 32], 0xA     
    mov byte [ecx + 33], 0        ; Завершаем строку нулем

    ; Выводим результат
    mov eax, 4                   
    mov ebx, 1                  
    mov edx, 34                  ; длина строки (32 бита + \n + 0)
    int 0x80

    ; Завершение программы
    mov eax, 1                   
    xor ebx, ebx                
    int 0x80
