/**
 * Virtual Machine SDK - Core Types
 * Пакет разработки для виртуальной машины - основные типы
 * 
 * This SDK provides tools for working with a custom VM architecture
 * Данный SDK предоставляет инструменты для работы с пользовательской архитектурой VM
 */

#ifndef VM_TYPES_H
#define VM_TYPES_H

#include <stdint.h>
#include <stdbool.h>

// Define the VM memory size (256 bytes based on analysis)
// Определение размера памяти VM (256 байт на основе анализа)
#define VM_MEMORY_SIZE 256
#define VM_STACK_SIZE 34

// VM Register structure
// Структура регистров VM
typedef struct {
    uint8_t zero_flag;      // Z flag (result is zero) - Z-флаг (результат равен нулю)
    uint8_t sign_flag;      // S flag (result is negative) - S-флаг (результат отрицательный)
    uint8_t carry_flag;     // C flag (carry/borrow) - C-флаг (перенос/заем)
    uint8_t overflow_flag;  // V flag (overflow) - V-флаг (переполнение)
} vm_flags_t;

// VM processor state
// Состояние процессора VM
typedef struct {
    uint8_t* memory;        // VM memory - память VM
    uint8_t ip;             // Instruction Pointer - указатель инструкций
    vm_flags_t flags;       // Status flags - флаги состояния
    uint8_t return_stack[VM_STACK_SIZE];  // Return address stack - стек возврата
    int8_t stack_pointer;   // Stack pointer - указатель стека
} vm_context_t;

// Operand addressing modes
// Режимы адресации операндов
typedef enum {
    ADDR_MODE_IMMEDIATE = 0,    // Immediate value - непосредственное значение
    ADDR_MODE_DIRECT = 1,       // Direct memory - прямая память
    ADDR_MODE_INDIRECT = 2      // Indirect memory - косвенная память
} vm_addr_mode_t;

// Instruction structure
// Структура инструкции
typedef struct {
    uint8_t opcode;             // Opcode value - значение опкода
    const char* mnemonic;       // Instruction name - имя инструкции
    uint8_t operand1_mode;      // First operand mode - режим первого операнда
    uint8_t operand2_mode;      // Second operand mode - режим второго операнда
    uint8_t has_operand1;       // Has first operand - наличие первого операнда
    uint8_t has_operand2;       // Has second operand - наличие второго операнда
} vm_instruction_info_t;

#endif // VM_TYPES_H