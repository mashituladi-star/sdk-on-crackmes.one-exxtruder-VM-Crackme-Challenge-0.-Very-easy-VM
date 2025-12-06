/**
 * Virtual Machine Core API
 * Базовый API виртуальной машины
 */

#ifndef VM_CORE_H
#define VM_CORE_H

#include "vm_types.h"
#include <stdio.h>

// Initialize VM context
// Инициализация контекста VM
vm_context_t* vm_init(uint8_t* memory_buffer);

// Execute VM program
// Выполнение программы VM
int vm_execute(vm_context_t* context, uint8_t start_address);

// Free VM resources
// Освобождение ресурсов VM
void vm_cleanup(vm_context_t* context);

// Read from VM memory with bounds checking
// Чтение из памяти VM с проверкой границ
uint8_t vm_memory_read(vm_context_t* context, uint8_t address);

// Write to VM memory with bounds checking
// Запись в память VM с проверкой границ
bool vm_memory_write(vm_context_t* context, uint8_t address, uint8_t value);

// Set VM flags based on operation result
// Установка флагов VM на основе результата операции
void vm_set_flags(vm_context_t* context, uint8_t result, bool carry, bool overflow);

// Read operand based on addressing mode
// Чтение операнда на основе режима адресации
bool vm_read_operand(vm_context_t* context, vm_addr_mode_t mode, 
                     uint8_t operand, uint8_t* result);

// Write operand based on addressing mode
// Запись операнда на основе режима адресации
bool vm_write_operand(vm_context_t* context, vm_addr_mode_t mode,
                      uint8_t operand, uint8_t value);

// Check if address is valid
// Проверка валидности адреса
bool vm_check_address(uint8_t address);

// Rotate right operation
// Операция вращения вправо
uint8_t vm_ror(uint8_t value, uint8_t shift);

// Rotate left operation
// Операция вращения влево
uint8_t vm_rol(uint8_t value, uint8_t shift);

// Handle input operation (OP_IN)
// Обработка операции ввода
void vm_handle_input(vm_context_t* context);

// Handle output operation (OP_OUT)
// Обработка операции вывода
void vm_handle_output(vm_context_t* context);

#endif // VM_CORE_H