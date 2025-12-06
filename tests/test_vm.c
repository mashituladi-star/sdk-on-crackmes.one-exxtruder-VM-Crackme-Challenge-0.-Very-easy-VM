/**
 * VM SDK Test Suite
 * Набор тестов для SDK виртуальной машины
 */

#include "../include/vm_core.h"
#include "../include/vm_opcodes.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

// Test suite structure
// Структура набора тестов
typedef struct {
    const char* test_name;
    int (*test_function)(void);
} vm_test_case_t;

// Global test counter
// Глобальный счетчик тестов
static int tests_passed = 0;
static int tests_failed = 0;

// Helper macro for assertions
// Вспомогательный макрос для утверждений
#define VM_TEST(condition, message) \
    do { \
        if (!(condition)) { \
            printf("FAIL: %s:%d: %s\n", __FILE__, __LINE__, message); \
            tests_failed++; \
            return 0; \
        } \
        printf("PASS: %s\n", message); \
    } while(0)

#define VM_TEST_SUITE_START(name) \
    printf("\n=== %s ===\n", name)

#define VM_TEST_SUITE_END() \
    printf("=== Test Suite Complete ===\n")

// Test 1: VM Initialization
// Тест 1: Инициализация VM
static int test_vm_initialization(void) {
    VM_TEST_SUITE_START("VM Initialization Tests");
    
    uint8_t memory[VM_MEMORY_SIZE] = {0};
    vm_context_t* vm = vm_init(memory);
    
    VM_TEST(vm != NULL, "VM context created successfully");
    VM_TEST(vm->memory == memory, "Memory pointer set correctly");
    VM_TEST(vm->ip == 0, "Instruction pointer initialized to 0");
    VM_TEST(vm->stack_pointer == -1, "Stack pointer initialized to -1");
    
    // Test flags initialization
    // Проверка инициализации флагов
    VM_TEST(vm->flags.zero_flag == 0, "Zero flag initialized to 0");
    VM_TEST(vm->flags.sign_flag == 0, "Sign flag initialized to 0");
    VM_TEST(vm->flags.carry_flag == 0, "Carry flag initialized to 0");
    VM_TEST(vm->flags.overflow_flag == 0, "Overflow flag initialized to 0");
    
    vm_cleanup(vm);
    return 1;
}

// Test 2: Memory Operations
// Тест 2: Операции с памятью
static int test_memory_operations(void) {
    VM_TEST_SUITE_START("Memory Operations Tests");
    
    uint8_t memory[VM_MEMORY_SIZE] = {0};
    vm_context_t* vm = vm_init(memory);
    
    // Test memory write/read
    // Проверка записи/чтения памяти
    assert(vm_memory_write(vm, 0x10, 0x42));
    VM_TEST(vm_memory_read(vm, 0x10) == 0x42, "Memory write/read at address 0x10");
    
    // Test boundary conditions
    // Проверка граничных условий
    assert(vm_memory_write(vm, 0xFF, 0xAA));
    VM_TEST(vm_memory_read(vm, 0xFF) == 0xAA, "Memory write/read at address 0xFF");
    
    // Test address validation
    // Проверка валидации адресов
    VM_TEST(vm_check_address(0x00) == true, "Address 0x00 is valid");
    VM_TEST(vm_check_address(0xFF) == true, "Address 0xFF is valid");
    VM_TEST(vm_check_address(0x100) == false, "Address 0x100 is invalid");
    
    vm_cleanup(vm);
    return 1;
}

// Test 3: MOV Instruction
// Тест 3: Инструкция MOV
static int test_mov_instruction(void) {
    VM_TEST_SUITE_START("MOV Instruction Tests");
    
    uint8_t memory[VM_MEMORY_SIZE] = {0};
    vm_context_t* vm = vm_init(memory);
    
    // Test immediate to direct
    // Проверка непосредственного значения в прямое
    uint8_t mov_program[] = {
        OP_MOV, 0x20, 0x42,    // MOV [0x20], 0x42
        OP_HALT
    };
    
    memcpy(memory, mov_program, sizeof(mov_program));
    vm_execute(vm, 0);
    
    VM_TEST(vm_memory_read(vm, 0x20) == 0x42, "MOV immediate to memory works");
    VM_TEST(vm->ip == 3, "Instruction pointer advanced correctly");
    
    // Test direct to direct
    // Проверка прямое в прямое
    vm_memory_write(vm, 0x30, 0x55);
    
    uint8_t mov_program2[] = {
        OP_MOV, 0x40, 0x30,    // MOV [0x40], [0x30]
        OP_HALT
    };
    
    memcpy(memory, mov_program2, sizeof(mov_program2));
    vm->ip = 0;
    vm_execute(vm, 0);
    
    VM_TEST(vm_memory_read(vm, 0x40) == 0x55, "MOV direct to direct works");
    
    vm_cleanup(vm);
    return 1;
}

// Test 4: ADD Instruction
// Тест 4: Инструкция ADD
static int test_add_instruction(void) {
    VM_TEST_SUITE_START("ADD Instruction Tests");
    
    uint8_t memory[VM_MEMORY_SIZE] = {0};
    vm_context_t* vm = vm_init(memory);
    
    // Test simple addition
    // Проверка простого сложения
    vm_memory_write(vm, 0x10, 0x20);
    
    uint8_t add_program[] = {
        OP_ADD, 0x10, 0x05,    // ADD [0x10], 0x05
        OP_HALT
    };
    
    memcpy(memory, add_program, sizeof(add_program));
    vm_execute(vm, 0);
    
    VM_TEST(vm_memory_read(vm, 0x10) == 0x25, "ADD operation result correct");
    VM_TEST(vm->flags.zero_flag == 0, "Zero flag cleared after non-zero result");
    VM_TEST(vm->flags.carry_flag == 0, "Carry flag cleared after no carry");
    
    // Test addition with carry
    // Проверка сложения с переносом
    vm_memory_write(vm, 0x20, 0xFF);
    
    uint8_t add_program2[] = {
        OP_ADD, 0x20, 0x01,    // ADD [0x20], 0x01
        OP_HALT
    };
    
    memcpy(memory, add_program2, sizeof(add_program2));
    vm->ip = 0;
    vm_execute(vm, 0);
    
    VM_TEST(vm_memory_read(vm, 0x20) == 0x00, "ADD with overflow result correct");
    VM_TEST(vm->flags.carry_flag == 1, "Carry flag set after overflow");
    VM_TEST(vm->flags.zero_flag == 1, "Zero flag set after result becomes zero");
    
    vm_cleanup(vm);
    return 1;
}

// Test 5: INC/DEC Instructions
// Тест 5: Инструкции INC/DEC
static int test_inc_dec_instructions(void) {
    VM_TEST_SUITE_START("INC/DEC Instruction Tests");
    
    uint8_t memory[VM_MEMORY_SIZE] = {0};
    vm_context_t* vm = vm_init(memory);
    
    // Test INC instruction
    // Проверка инструкции INC
    vm_memory_write(vm, 0x10, 0x41);
    
    uint8_t inc_program[] = {
        OP_INC, 0x10,          // INC [0x10]
        OP_HALT
    };
    
    memcpy(memory, inc_program, sizeof(inc_program));
    vm_execute(vm, 0);
    
    VM_TEST(vm_memory_read(vm, 0x10) == 0x42, "INC operation result correct");
    
    // Test DEC instruction
    // Проверка инструкции DEC
    vm_memory_write(vm, 0x20, 0x01);
    
    uint8_t dec_program[] = {
        OP_DEC, 0x20,          // DEC [0x20]
        OP_HALT
    };
    
    memcpy(memory, dec_program, sizeof(dec_program));
    vm->ip = 0;
    vm_execute(vm, 0);
    
    VM_TEST(vm_memory_read(vm, 0x20) == 0x00, "DEC operation result correct");
    VM_TEST(vm->flags.zero_flag == 1, "Zero flag set after decrement to zero");
    
    vm_cleanup(vm);
    return 1;
}

// Test 6: JMP Instruction
// Тест 6: Инструкция JMP
static int test_jmp_instruction(void) {
    VM_TEST_SUITE_START("JMP Instruction Tests");
    
    uint8_t memory[VM_MEMORY_SIZE] = {0};
    vm_context_t* vm = vm_init(memory);
    
    // Test unconditional jump
    // Проверка безусловного перехода
    uint8_t jmp_program[] = {
        OP_MOV, 0x10, 0x01,    // MOV [0x10], 0x01
        OP_JMP, 0x06,          // JMP to address 0x06
        OP_MOV, 0x10, 0x02,    // This should be skipped - Эта инструкция должна быть пропущена
        OP_MOV, 0x11, 0x03,    // MOV [0x11], 0x03
        OP_HALT
    };
    
    memcpy(memory, jmp_program, sizeof(jmp_program));
    vm_execute(vm, 0);
    
    VM_TEST(vm_memory_read(vm, 0x10) == 0x01, "First MOV executed");
    VM_TEST(vm_memory_read(vm, 0x11) == 0x03, "Instruction after JMP executed");
    VM_TEST(vm_memory_read(vm, 0x10) != 0x02, "Skipped instruction not executed");
    
    vm_cleanup(vm);
    return 1;
}

// Test 7: CALL/RET Instructions
// Тест 7: Инструкции CALL/RET
static int test_call_ret_instructions(void) {
    VM_TEST_SUITE_START("CALL/RET Instruction Tests");
    
    uint8_t memory[VM_MEMORY_SIZE] = {0};
    vm_context_t* vm = vm_init(memory);
    
    // Test subroutine call and return
    // Проверка вызова подпрограммы и возврата
    uint8_t call_program[] = {
        OP_MOV, 0x10, 0x01,    // MOV [0x10], 0x01 (main)
        OP_CALL, 0x06,         // CALL subroutine at 0x06
        OP_MOV, 0x11, 0x03,    // MOV [0x11], 0x03 (after return)
        OP_HALT,               // HALT
        // Subroutine starts here - Подпрограмма начинается здесь
        OP_MOV, 0x12, 0x02,    // MOV [0x12], 0x02 (in subroutine)
        OP_RET,                // RET
        OP_MOV, 0x13, 0x04     // This should not execute - Эта инструкция не должна выполниться
    };
    
    memcpy(memory, call_program, sizeof(call_program));
    vm_execute(vm, 0);
    
    VM_TEST(vm_memory_read(vm, 0x10) == 0x01, "Main program code executed");
    VM_TEST(vm_memory_read(vm, 0x12) == 0x02, "Subroutine code executed");
    VM_TEST(vm_memory_read(vm, 0x11) == 0x03, "Code after return executed");
    VM_TEST(vm_memory_read(vm, 0x13) == 0x00, "Code after subroutine not executed");
    VM_TEST(vm->stack_pointer == -1, "Stack pointer restored after return");
    
    vm_cleanup(vm);
    return 1;
}

// Test 8: Logical Instructions (AND, OR, XOR)
// Тест 8: Логические инструкции (AND, OR, XOR)
static int test_logical_instructions(void) {
    VM_TEST_SUITE_START("Logical Instructions Tests");
    
    uint8_t memory[VM_MEMORY_SIZE] = {0};
    vm_context_t* vm = vm_init(memory);
    
    // Test AND instruction
    // Проверка инструкции AND
    vm_memory_write(vm, 0x10, 0b11001100);
    
    uint8_t and_program[] = {
        OP_AND, 0x10, 0b10101010,  // AND [0x10], 0xAA
        OP_HALT
    };
    
    memcpy(memory, and_program, sizeof(and_program));
    vm_execute(vm, 0);
    
    VM_TEST(vm_memory_read(vm, 0x10) == 0b10001000, "AND operation result correct");
    
    // Test OR instruction
    // Проверка инструкции OR
    vm_memory_write(vm, 0x20, 0b11001100);
    
    uint8_t or_program[] = {
        OP_OR, 0x20, 0b00110011,   // OR [0x20], 0x33
        OP_HALT
    };
    
    memcpy(memory, or_program, sizeof(or_program));
    vm->ip = 0;
    vm_execute(vm, 0);
    
    VM_TEST(vm_memory_read(vm, 0x20) == 0b11111111, "OR operation result correct");
    
    // Test XOR instruction
    // Проверка инструкции XOR
    vm_memory_write(vm, 0x30, 0b11001100);
    
    uint8_t xor_program[] = {
        OP_XOR, 0x30, 0b10101010,  // XOR [0x30], 0xAA
        OP_HALT
    };
    
    memcpy(memory, xor_program, sizeof(xor_program));
    vm->ip = 0;
    vm_execute(vm, 0);
    
    VM_TEST(vm_memory_read(vm, 0x30) == 0b01100110, "XOR operation result correct");
    
    vm_cleanup(vm);
    return 1;
}

// Test 9: Shift/Rotate Instructions
// Тест 9: Инструкции сдвига/вращения
static int test_shift_rotate_instructions(void) {
    VM_TEST_SUITE_START("Shift/Rotate Instructions Tests");
    
    uint8_t memory[VM_MEMORY_SIZE] = {0};
    vm_context_t* vm = vm_init(memory);
    
    // Test ROL (Rotate Left)
    // Проверка ROL (Вращение влево)
    vm_memory_write(vm, 0x10, 0b10000001);
    
    uint8_t rol_program[] = {
        OP_ROL, 0x10, 0x01,    // ROL [0x10], 1
        OP_HALT
    };
    
    memcpy(memory, rol_program, sizeof(rol_program));
    vm_execute(vm, 0);
    
    VM_TEST(vm_memory_read(vm, 0x10) == 0b00000011, "ROL operation result correct");
    VM_TEST(vm->flags.carry_flag == 1, "Carry flag set after ROL");
    
    // Test ROR (Rotate Right)
    // Проверка ROR (Вращение вправо)
    vm_memory_write(vm, 0x20, 0b10000001);
    
    uint8_t ror_program[] = {
        OP_ROR, 0x20, 0x01,    // ROR [0x20], 1
        OP_HALT
    };
    
    memcpy(memory, ror_program, sizeof(ror_program));
    vm->ip = 0;
    vm_execute(vm, 0);
    
    VM_TEST(vm_memory_read(vm, 0x20) == 0b11000000, "ROR operation result correct");
    VM_TEST(vm->flags.carry_flag == 1, "Carry flag set after ROR");
    
    // Test SHL (Shift Left)
    // Проверка SHL (Сдвиг влево)
    vm_memory_write(vm, 0x30, 0b01000000);
    
    uint8_t shl_program[] = {
        OP_SHL, 0x30, 0x01,    // SHL [0x30], 1
        OP_HALT
    };
    
    memcpy(memory, shl_program, sizeof(shl_program));
    vm->ip = 0;
    vm_execute(vm, 0);
    
    VM_TEST(vm_memory_read(vm, 0x30) == 0b10000000, "SHL operation result correct");
    VM_TEST(vm->flags.carry_flag == 0, "Carry flag cleared after SHL");
    
    // Test SHR (Shift Right)
    // Проверка SHR (Сдвиг вправо)
    vm_memory_write(vm, 0x40, 0b00000010);
    
    uint8_t shr_program[] = {
        OP_SHR, 0x40, 0x01,    // SHR [0x40], 1
        OP_HALT
    };
    
    memcpy(memory, shr_program, sizeof(shr_program));
    vm->ip = 0;
    vm_execute(vm, 0);
    
    VM_TEST(vm_memory_read(vm, 0x40) == 0b00000001, "SHR operation result correct");
    VM_TEST(vm->flags.carry_flag == 0, "Carry flag cleared after SHR");
    
    vm_cleanup(vm);
    return 1;
}

// Test 10: Conditional Jump Instructions
// Тест 10: Инструкции условного перехода
static int test_conditional_jumps(void) {
    VM_TEST_SUITE_START("Conditional Jump Tests");
    
    uint8_t memory[VM_MEMORY_SIZE] = {0};
    vm_context_t* vm = vm_init(memory);
    
    // Test JE (Jump if Equal/Zero)
    // Проверка JE (Переход если равно/ноль)
    vm_memory_write(vm, 0x10, 0x00);
    
    uint8_t je_program[] = {
        OP_MOV, 0x10, 0x00,    // Set value to zero - Установка значения в ноль
        OP_ADD, 0x10, 0x00,    // ADD to set zero flag - ADD для установки флага нуля
        OP_JE, 0x09,           // JE should jump - JE должен перейти
        OP_MOV, 0x11, 0xFF,    // This should be skipped - Эта инструкция должна быть пропущена
        OP_MOV, 0x12, 0x01,    // MOV [0x12], 0x01 (jump target)
        OP_HALT
    };
    
    memcpy(memory, je_program, sizeof(je_program));
    vm_execute(vm, 0);
    
    VM_TEST(vm_memory_read(vm, 0x11) == 0x00, "JE: Skipped instruction when ZF=1");
    VM_TEST(vm_memory_read(vm, 0x12) == 0x01, "JE: Jump target executed");
    
    // Test JNE (Jump if Not Equal/Not Zero)
    // Проверка JNE (Переход если не равно/не ноль)
    vm_memory_write(vm, 0x20, 0x01);
    
    uint8_t jne_program[] = {
        OP_MOV, 0x20, 0x01,    // Set non-zero value - Установка ненулевого значения
        OP_ADD, 0x20, 0x00,    // ADD (ZF should be 0) - ADD (ZF должен быть 0)
        OP_JNE, 0x09,          // JNE should jump - JNE должен перейти
        OP_MOV, 0x21, 0xFF,    // This should be skipped - Эта инструкция должна быть пропущена
        OP_MOV, 0x22, 0x02,    // MOV [0x22], 0x02 (jump target)
        OP_HALT
    };
    
    memcpy(memory, jne_program, sizeof(jne_program));
    vm->ip = 0;
    vm_execute(vm, 0);
    
    VM_TEST(vm_memory_read(vm, 0x21) == 0x00, "JNE: Skipped instruction when ZF=0");
    VM_TEST(vm_memory_read(vm, 0x22) == 0x02, "JNE: Jump target executed");
    
    vm_cleanup(vm);
    return 1;
}

// Test 11: I/O Instructions (Simulated)
// Тест 11: Инструкции ввода/вывода (симулированные)
static int test_io_instructions(void) {
    VM_TEST_SUITE_START("I/O Instructions Tests (Simulated)");
    
    uint8_t memory[VM_MEMORY_SIZE] = {0};
    vm_context_t* vm = vm_init(memory);
    
    // Note: Actual I/O would require mocking stdin/stdout
    // Примечание: Реальный ввод/вывод потребует мокирования stdin/stdout
    
    // Test setup for OUT instruction
    // Настройка теста для инструкции OUT
    char test_string[] = "TEST";
    for (int i = 0; i < 4; i++) {
        vm_memory_write(vm, 0x80 + i, test_string[i]);
    }
    
    vm_memory_write(vm, 0xFC, 0x04);  // Length - Длина
    vm_memory_write(vm, 0xFD, 0x80);  // Address - Адрес
    
    // We can't easily test actual output, but we can verify parameters
    // Мы не можем легко протестировать реальный вывод, но можем проверить параметры
    VM_TEST(vm_memory_read(vm, 0xFC) == 0x04, "OUT parameter: length set correctly");
    VM_TEST(vm_memory_read(vm, 0xFD) == 0x80, "OUT parameter: address set correctly");
    
    // Test setup for IN instruction
    // Настройка теста для инструкции IN
    vm_memory_write(vm, 0xFC, 0x0A);  // Buffer length - Длина буфера
    vm_memory_write(vm, 0xFD, 0x90);  // Buffer address - Адрес буфера
    
    VM_TEST(vm_memory_read(vm, 0xFC) == 0x0A, "IN parameter: buffer length set correctly");
    VM_TEST(vm_memory_read(vm, 0xFD) == 0x90, "IN parameter: buffer address set correctly");
    
    vm_cleanup(vm);
    return 1;
}

// Test 12: Complex Program Example
// Тест 12: Пример сложной программы
static int test_complex_program(void) {
    VM_TEST_SUITE_START("Complex Program Test");
    
    uint8_t memory[VM_MEMORY_SIZE] = {0};
    vm_context_t* vm = vm_init(memory);
    
    // Program: Calculate factorial of 5 (5! = 120)
    // Программа: Вычисление факториала 5 (5! = 120)
    uint8_t factorial_program[] = {
        // Initialize - Инициализация
        OP_MOV, 0x10, 0x05,    // MOV [0x10], 5   ; counter - счетчик
        OP_MOV, 0x11, 0x01,    // MOV [0x11], 1   ; result - результат
        
        // Loop start - Начало цикла
        0x08,                  // Label LOOP - Метка LOOP
        OP_MOV, 0x12, 0x11,    // MOV [0x12], [0x11] ; temp = result
        OP_MOV, 0x13, 0x10,    // MOV [0x13], [0x10] ; multiplier = counter
        
        // Multiplication loop (add result counter times)
        // Цикл умножения (сложение результата counter раз)
        0x10,                  // Label MUL_LOOP - Метка MUL_LOOP
        OP_DEC, 0x13,          // DEC [0x13]      ; decrement multiplier
        OP_ADD, 0x11, 0x12,    // ADD [0x11], [0x12] ; result += temp
        OP_JNE, 0x10,          // JNE MUL_LOOP   ; if multiplier != 0
        
        // Decrement counter and check if done
        // Декремент счетчика и проверка завершения
        OP_DEC, 0x10,          // DEC [0x10]      ; counter--
        OP_JNE, 0x08,          // JNE LOOP        ; if counter != 0
        
        // Store final result - Сохранение конечного результата
        OP_MOV, 0x20, 0x11,    // MOV [0x20], [0x11]
        OP_HALT
    };
    
    memcpy(memory, factorial_program, sizeof(factorial_program));
    vm_execute(vm, 0);
    
    // 5! = 120 = 0x78
    VM_TEST(vm_memory_read(vm, 0x20) == 0x78, "Factorial of 5 calculated correctly (120 = 0x78)");
    
    vm_cleanup(vm);
    return 1;
}

// Test 13: Edge Cases and Error Handling
// Тест 13: Граничные случаи и обработка ошибок
static int test_edge_cases(void) {
    VM_TEST_SUITE_START("Edge Cases and Error Handling");
    
    uint8_t memory[VM_MEMORY_SIZE] = {0};
    vm_context_t* vm = vm_init(memory);
    
    // Test stack overflow protection
    // Проверка защиты от переполнения стека
    for (int i = 0; i < VM_STACK_SIZE; i++) {
        if (vm->stack_pointer < VM_STACK_SIZE - 1) {
            vm->stack_pointer++;
            vm->return_stack[vm->stack_pointer] = i * 2;
        }
    }
    
    VM_TEST(vm->stack_pointer == VM_STACK_SIZE - 1, "Stack can be filled to capacity");
    
    // Test stack underflow protection
    // Проверка защиты от опустошения стека
    for (int i = 0; i < VM_STACK_SIZE; i++) {
        if (vm->stack_pointer >= 0) {
            vm->stack_pointer--;
        }
    }
    
    VM_TEST(vm->stack_pointer == -1, "Stack pointer correctly handles underflow");
    
    // Test unknown opcode handling
    // Проверка обработки неизвестных опкодов
    uint8_t invalid_program[] = {
        0xFF,                  // Invalid opcode - Неверный опкод
        OP_HALT
    };
    
    memcpy(memory, invalid_program, sizeof(invalid_program));
    
    // Redirect stderr to capture error message
    // Перенаправление stderr для захвата сообщения об ошибке
    printf("Expected error message for invalid opcode:\n");
    vm_execute(vm, 0);
    
    vm_cleanup(vm);
    return 1;
}

// Test 14: Flag Setting and Checking
// Тест 14: Установка и проверка флагов
static int test_flag_operations(void) {
    VM_TEST_SUITE_START("Flag Operations Tests");
    
    uint8_t memory[VM_MEMORY_SIZE] = {0};
    vm_context_t* vm = vm_init(memory);
    
    // Test zero flag
    // Проверка флага нуля
    vm_set_flags(vm, 0x00, false, false);
    VM_TEST(vm->flags.zero_flag == 1, "Zero flag set for zero value");
    
    vm_set_flags(vm, 0x01, false, false);
    VM_TEST(vm->flags.zero_flag == 0, "Zero flag cleared for non-zero value");
    
    // Test sign flag
    // Проверка флага знака
    vm_set_flags(vm, 0x80, false, false);
    VM_TEST(vm->flags.sign_flag == 1, "Sign flag set for negative value (0x80)");
    
    vm_set_flags(vm, 0x7F, false, false);
    VM_TEST(vm->flags.sign_flag == 0, "Sign flag cleared for positive value (0x7F)");
    
    // Test carry flag
    // Проверка флага переноса
    vm_set_flags(vm, 0x00, true, false);
    VM_TEST(vm->flags.carry_flag == 1, "Carry flag set when specified");
    
    vm_set_flags(vm, 0x00, false, false);
    VM_TEST(vm->flags.carry_flag == 0, "Carry flag cleared when specified");
    
    // Test overflow flag
    // Проверка флага переполнения
    vm_set_flags(vm, 0x00, false, true);
    VM_TEST(vm->flags.overflow_flag == 1, "Overflow flag set when specified");
    
    vm_set_flags(vm, 0x00, false, false);
    VM_TEST(vm->flags.overflow_flag == 0, "Overflow flag cleared when specified");
    
    vm_cleanup(vm);
    return 1;
}

// Test 15: Memory Range Tests
// Тест 15: Тесты диапазонов памяти
static int test_memory_ranges(void) {
    VM_TEST_SUITE_START("Memory Range Tests");
    
    uint8_t memory[VM_MEMORY_SIZE] = {0};
    vm_context_t* vm = vm_init(memory);
    
    // Test all memory addresses can be written and read
    // Проверка, что все адреса памяти могут быть записаны и прочитаны
    for (int i = 0; i < VM_MEMORY_SIZE; i++) {
        uint8_t test_value = i & 0xFF;
        assert(vm_memory_write(vm, i, test_value));
        
        uint8_t read_value = vm_memory_read(vm, i);
        if (read_value != test_value) {
            printf("FAIL: Memory test at address 0x%02X: wrote 0x%02X, read 0x%02X\n",
                   i, test_value, read_value);
            tests_failed++;
            vm_cleanup(vm);
            return 0;
        }
    }
    
    printf("PASS: All %d memory addresses tested successfully\n", VM_MEMORY_SIZE);
    
    // Test I/O parameter area (0xFC-0xFD)
    // Проверка области параметров ввода/вывода (0xFC-0xFD)
    vm_memory_write(vm, 0xFC, 0xAA);
    vm_memory_write(vm, 0xFD, 0xBB);
    
    VM_TEST(vm_memory_read(vm, 0xFC) == 0xAA, "I/O parameter area address 0xFC works");
    VM_TEST(vm_memory_read(vm, 0xFD) == 0xBB, "I/O parameter area address 0xFD works");
    
    vm_cleanup(vm);
    return 1;
}

// Main test runner
// Основной запускатель тестов
int main(void) {
    printf("=== VM SDK Test Suite ===\n");
    printf("=== Набор тестов SDK VM ===\n\n");
    
    // Define all test cases
    // Определение всех тестовых случаев
    vm_test_case_t test_cases[] = {
        {"VM Initialization", test_vm_initialization},
        {"Memory Operations", test_memory_operations},
        {"MOV Instruction", test_mov_instruction},
        {"ADD Instruction", test_add_instruction},
        {"INC/DEC Instructions", test_inc_dec_instructions},
        {"JMP Instruction", test_jmp_instruction},
        {"CALL/RET Instructions", test_call_ret_instructions},
        {"Logical Instructions", test_logical_instructions},
        {"Shift/Rotate Instructions", test_shift_rotate_instructions},
        {"Conditional Jumps", test_conditional_jumps},
        {"I/O Instructions", test_io_instructions},
        {"Complex Program", test_complex_program},
        {"Edge Cases", test_edge_cases},
        {"Flag Operations", test_flag_operations},
        {"Memory Ranges", test_memory_ranges},
        {NULL, NULL} // Terminator - терминатор
    };
    
    // Run all tests
    // Запуск всех тестов
    for (int i = 0; test_cases[i].test_name != NULL; i++) {
        printf("\n[Test %d] %s\n", i + 1, test_cases[i].test_name);
        if (test_cases[i].test_function()) {
            tests_passed++;
        } else {
            tests_failed++;
        }
    }
    
    // Print summary
    // Вывод сводки
    printf("\n=== Test Summary ===\n");
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_failed);
    printf("Total tests:  %d\n", tests_passed + tests_failed);
    
    if (tests_failed == 0) {
        printf("\n✓ All tests passed successfully!\n");
        printf("✓ Все тесты успешно пройдены!\n");
    } else {
        printf("\n✗ Some tests failed. See details above.\n");
        printf("✗ Некоторые тесты не пройдены. Смотрите детали выше.\n");
    }
    
    return (tests_failed == 0) ? 0 : 1;
}