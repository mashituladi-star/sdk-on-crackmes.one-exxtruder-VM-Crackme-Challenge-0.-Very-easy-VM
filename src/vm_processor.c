/**
 * Virtual Machine Processor Implementation
 * Реализация процессора виртуальной машины
 */

#include "vm_core.h"
#include "vm_opcodes.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Instruction table matching the analyzed binary
// Таблица инструкций, соответствующая анализируемому бинарнику
static const vm_instruction_info_t instruction_table[] = {
    // Format: {opcode, mnemonic, op1_mode, op2_mode, has_op1, has_op2}
    {OP_MOV,  "MOV",  ADDR_MODE_DIRECT, ADDR_MODE_DIRECT, 1, 1},
    {OP_XCHG, "XCHG", ADDR_MODE_DIRECT, ADDR_MODE_DIRECT, 1, 1},
    {OP_ADD,  "ADD",  ADDR_MODE_DIRECT, ADDR_MODE_DIRECT, 1, 1},
    {OP_SUB,  "SUB",  ADDR_MODE_DIRECT, ADDR_MODE_DIRECT, 1, 1},
    {OP_CMP,  "CMP",  ADDR_MODE_DIRECT, ADDR_MODE_DIRECT, 1, 1},
    {OP_AND,  "AND",  ADDR_MODE_DIRECT, ADDR_MODE_DIRECT, 1, 1},
    {OP_OR,   "OR",   ADDR_MODE_DIRECT, ADDR_MODE_DIRECT, 1, 1},
    {OP_XOR,  "XOR",  ADDR_MODE_DIRECT, ADDR_MODE_DIRECT, 1, 1},
    {OP_ROL,  "ROL",  ADDR_MODE_DIRECT, ADDR_MODE_DIRECT, 1, 1},
    {OP_ROR,  "ROR",  ADDR_MODE_DIRECT, ADDR_MODE_DIRECT, 1, 1},
    {OP_SHL,  "SHL",  ADDR_MODE_DIRECT, ADDR_MODE_DIRECT, 1, 1},
    {OP_SHR,  "SHR",  ADDR_MODE_DIRECT, ADDR_MODE_DIRECT, 1, 1},
    {OP_INC,  "INC",  ADDR_MODE_DIRECT, ADDR_MODE_IMMEDIATE, 1, 0},
    {OP_DEC,  "DEC",  ADDR_MODE_DIRECT, ADDR_MODE_IMMEDIATE, 1, 0},
    {OP_NOT,  "NOT",  ADDR_MODE_DIRECT, ADDR_MODE_IMMEDIATE, 1, 0},
    {OP_JE,   "JE",   ADDR_MODE_DIRECT, ADDR_MODE_IMMEDIATE, 1, 0},
    {OP_JNE,  "JNE",  ADDR_MODE_DIRECT, ADDR_MODE_IMMEDIATE, 1, 0},
    {OP_JL,   "JL",   ADDR_MODE_DIRECT, ADDR_MODE_IMMEDIATE, 1, 0},
    {OP_JLE,  "JLE",  ADDR_MODE_DIRECT, ADDR_MODE_IMMEDIATE, 1, 0},
    {OP_JB,   "JB",   ADDR_MODE_DIRECT, ADDR_MODE_IMMEDIATE, 1, 0},
    {OP_JBE,  "JBE",  ADDR_MODE_DIRECT, ADDR_MODE_IMMEDIATE, 1, 0},
    {OP_JP,   "JP",   ADDR_MODE_DIRECT, ADDR_MODE_IMMEDIATE, 1, 0},
    {OP_JO,   "JO",   ADDR_MODE_DIRECT, ADDR_MODE_IMMEDIATE, 1, 0},
    {OP_JS,   "JS",   ADDR_MODE_DIRECT, ADDR_MODE_IMMEDIATE, 1, 0},
    {OP_JNL,  "JNL",  ADDR_MODE_DIRECT, ADDR_MODE_IMMEDIATE, 1, 0},
    {OP_JG,   "JG",   ADDR_MODE_DIRECT, ADDR_MODE_IMMEDIATE, 1, 0},
    {OP_JAE,  "JAE",  ADDR_MODE_DIRECT, ADDR_MODE_IMMEDIATE, 1, 0},
    {OP_JA,   "JA",   ADDR_MODE_DIRECT, ADDR_MODE_IMMEDIATE, 1, 0},
    {OP_JNP,  "JNP",  ADDR_MODE_DIRECT, ADDR_MODE_IMMEDIATE, 1, 0},
    {OP_JNO,  "JNO",  ADDR_MODE_DIRECT, ADDR_MODE_IMMEDIATE, 1, 0},
    {OP_JMP,  "JMP",  ADDR_MODE_DIRECT, ADDR_MODE_IMMEDIATE, 1, 0},
    {OP_CALL, "CALL", ADDR_MODE_DIRECT, ADDR_MODE_IMMEDIATE, 1, 0},
    {OP_RET,  "RET",  ADDR_MODE_IMMEDIATE, ADDR_MODE_IMMEDIATE, 0, 0},
    {OP_OUT,  "OUT",  ADDR_MODE_DIRECT, ADDR_MODE_DIRECT, 1, 1},
    {OP_IN,   "IN",   ADDR_MODE_DIRECT, ADDR_MODE_DIRECT, 1, 1},
    {OP_HALT, "HALT", ADDR_MODE_IMMEDIATE, ADDR_MODE_IMMEDIATE, 0, 0},
    {OP_ADC,  "ADC",  ADDR_MODE_DIRECT, ADDR_MODE_DIRECT, 1, 1},
    {OP_SBB,  "SBB",  ADDR_MODE_DIRECT, ADDR_MODE_DIRECT, 1, 1},
    {OP_RCL,  "RCL",  ADDR_MODE_DIRECT, ADDR_MODE_DIRECT, 1, 1},
    {OP_RCR,  "RCR",  ADDR_MODE_DIRECT, ADDR_MODE_DIRECT, 1, 1},
    {0, NULL, 0, 0, 0, 0} // Terminator - терминатор
};

// Get instruction info by opcode
// Получение информации об инструкции по опкоду
const vm_instruction_info_t* vm_get_instruction_info(uint8_t opcode) {
    for (int i = 0; instruction_table[i].mnemonic != NULL; i++) {
        if (instruction_table[i].opcode == opcode) {
            return &instruction_table[i];
        }
    }
    return NULL;
}

// Initialize VM context
// Инициализация контекста VM
vm_context_t* vm_init(uint8_t* memory_buffer) {
    vm_context_t* context = malloc(sizeof(vm_context_t));
    if (!context) return NULL;
    
    context->memory = memory_buffer;
    context->ip = 0;
    context->stack_pointer = -1;
    
    // Initialize flags
    // Инициализация флагов
    context->flags.zero_flag = 0;
    context->flags.sign_flag = 0;
    context->flags.carry_flag = 0;
    context->flags.overflow_flag = 0;
    
    // Clear return stack
    // Очистка стека возврата
    memset(context->return_stack, 0, VM_STACK_SIZE);
    
    return context;
}

// Execute one instruction
// Выполнение одной инструкции
static bool vm_execute_instruction(vm_context_t* context) {
    // Fetch opcode
    // Извлечение опкода
    uint8_t opcode = vm_memory_read(context, context->ip);
    context->ip++;
    
    // Get instruction info
    // Получение информации об инструкции
    const vm_instruction_info_t* info = vm_get_instruction_info(opcode);
    if (!info) {
        fprintf(stderr, "Unknown opcode: 0x%02X at IP=0x%02X\n", opcode, context->ip - 1);
        return false;
    }
    
    // Read operands based on instruction format
    // Чтение операндов на основе формата инструкции
    uint8_t operand1 = 0;
    uint8_t operand2 = 0;
    
    if (info->has_operand1) {
        operand1 = vm_memory_read(context, context->ip);
        context->ip++;
    }
    
    if (info->has_operand2) {
        operand2 = vm_memory_read(context, context->ip);
        context->ip++;
    }
    
    // Execute the instruction
    // Выполнение инструкции
    switch (opcode) {
        case OP_MOV: {
            uint8_t value;
            if (vm_read_operand(context, info->operand2_mode, operand2, &value)) {
                vm_write_operand(context, info->operand1_mode, operand1, value);
            }
            break;
        }
        
        case OP_ADD: {
            uint8_t src, dst;
            if (vm_read_operand(context, info->operand1_mode, operand1, &dst) &&
                vm_read_operand(context, info->operand2_mode, operand2, &src)) {
                uint16_t result = dst + src;
                bool carry = result > 0xFF;
                bool overflow = ((dst ^ result) & (src ^ result) & 0x80) != 0;
                vm_set_flags(context, (uint8_t)result, carry, overflow);
                vm_write_operand(context, info->operand1_mode, operand1, (uint8_t)result);
            }
            break;
        }
        
        case OP_INC: {
            uint8_t value;
            if (vm_read_operand(context, info->operand1_mode, operand1, &value)) {
                uint8_t result = value + 1;
                bool carry = result == 0;
                bool overflow = value == 0x7F;
                vm_set_flags(context, result, carry, overflow);
                vm_write_operand(context, info->operand1_mode, operand1, result);
            }
            break;
        }
        
        case OP_DEC: {
            uint8_t value;
            if (vm_read_operand(context, info->operand1_mode, operand1, &value)) {
                uint8_t result = value - 1;
                bool carry = value == 0;
                bool overflow = value == 0x80;
                vm_set_flags(context, result, carry, overflow);
                vm_write_operand(context, info->operand1_mode, operand1, result);
            }
            break;
        }
        
        case OP_JMP: {
            context->ip = operand1;
            break;
        }
        
        case OP_CALL: {
            if (context->stack_pointer < VM_STACK_SIZE - 1) {
                context->stack_pointer++;
                context->return_stack[context->stack_pointer] = context->ip;
                context->ip = operand1;
            }
            break;
        }
        
        case OP_RET: {
            if (context->stack_pointer >= 0) {
                context->ip = context->return_stack[context->stack_pointer];
                context->stack_pointer--;
            }
            break;
        }
        
        case OP_OUT:
            vm_handle_output(context);
            break;
            
        case OP_IN:
            vm_handle_input(context);
            break;
            
        case OP_HALT:
            return false;
            
        default:
            fprintf(stderr, "Unimplemented opcode: 0x%02X\n", opcode);
            return false;
    }
    
    return true;
}

// Main VM execution loop
// Главный цикл выполнения VM
int vm_execute(vm_context_t* context, uint8_t start_address) {
    context->ip = start_address;
    
    while (context->ip < VM_MEMORY_SIZE) {
        if (!vm_execute_instruction(context)) {
            break;
        }
    }
    
    return 0;
}

// Set flags based on operation result
// Установка флагов на основе результата операции
void vm_set_flags(vm_context_t* context, uint8_t result, bool carry, bool overflow) {
    context->flags.zero_flag = (result == 0);
    context->flags.sign_flag = (result >> 7);
    context->flags.carry_flag = carry;
    context->flags.overflow_flag = overflow;
}

// Clean up VM resources
// Очистка ресурсов VM
void vm_cleanup(vm_context_t* context) {
    free(context);
}

// Rotate right
// Вращение вправо
uint8_t vm_ror(uint8_t value, uint8_t shift) {
    shift &= 7;
    if (shift == 0) return value;
    return (value >> shift) | (value << (8 - shift));
}

// Rotate left
// Вращение влево
uint8_t vm_rol(uint8_t value, uint8_t shift) {
    shift &= 7;
    if (shift == 0) return value;
    return (value << shift) | (value >> (8 - shift));
}