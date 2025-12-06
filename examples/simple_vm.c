/**
 * Simple VM Example Program
 * Простой пример программы для VM
 * Demonstrates VM SDK usage - демонстрирует использование SDK VM
 */

#include "../include/vm_core.h"
#include <stdio.h>
#include <string.h>

int main() {
    // Create VM memory buffer
    // Создание буфера памяти VM
    uint8_t memory[VM_MEMORY_SIZE] = {0};
    
    // Sample program: prints "Hello" and reads input
    // Пример программы: выводит "Hello" и читает ввод
    uint8_t program[] = {
        // Store string at address 0x80
        // Сохранение строки по адресу 0x80
        0x01, 0x80, 'H',  // MOV [0x80], 'H'
        0x01, 0x81, 'e',  // MOV [0x81], 'e'
        0x01, 0x82, 'l',  // MOV [0x82], 'l'
        0x01, 0x83, 'l',  // MOV [0x83], 'l'
        0x01, 0x84, 'o',  // MOV [0x84], 'o'
        
        // Setup output parameters
        // Настройка параметров вывода
        0x01, 0xFC, 0x05,  // MOV [0xFC], 5 (length)
        0x01, 0xFD, 0x80,  // MOV [0xFD], 0x80 (address)
        
        // Output the string
        // Вывод строки
        0x2D,              // OUT
        
        // Setup input parameters
        // Настройка параметров ввода
        0x01, 0xFC, 0x0A,  // MOV [0xFC], 10 (max length)
        0x01, 0xFD, 0x90,  // MOV [0xFD], 0x90 (buffer address)
        
        // Read input
        // Чтение ввода
        0x2E,              // IN
        
        // Halt
        // Остановка
        0x2C               // HALT
    };
    
    // Copy program to VM memory
    // Копирование программы в память VM
    memcpy(memory, program, sizeof(program));
    
    // Initialize VM
    // Инициализация VM
    vm_context_t* vm = vm_init(memory);
    if (!vm) {
        fprintf(stderr, "Failed to initialize VM\n");
        return 1;
    }
    
    printf("Starting VM execution...\n");
    printf("Запуск выполнения VM...\n\n");
    
    // Execute VM starting from address 0
    // Выполнение VM начиная с адреса 0
    int result = vm_execute(vm, 0);
    
    // Clean up
    // Очистка
    vm_cleanup(vm);
    
    printf("\nVM execution completed with code: %d\n", result);
    printf("Выполнение VM завершено с кодом: %d\n", result);
    
    return 0;
}