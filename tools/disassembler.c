/**
 * VM Disassembler Tool
 * Инструмент дизассемблера VM
 */

#include "../include/vm_core.h"
#include "../include/vm_opcodes.h"
#include <stdio.h>
#include <stdlib.h>

void disassemble_program(uint8_t* program, size_t size) {
    printf("VM Program Disassembly\n");
    printf("Дизассемблирование программы VM\n");
    printf("===============================\n\n");
    
    size_t ip = 0;
    while (ip < size) {
        printf("0x%04zX: ", ip);
        
        uint8_t opcode = program[ip];
        const vm_instruction_info_t* info = vm_get_instruction_info(opcode);
        
        if (!info) {
            printf("DB 0x%02X    ; Unknown opcode\n", opcode);
            ip++;
            continue;
        }
        
        printf("%-8s", info->mnemonic);
        ip++;
        
        if (info->has_operand1) {
            printf("0x%02X", program[ip]);
            ip++;
            
            if (info->has_operand2) {
                printf(", 0x%02X", program[ip]);
                ip++;
            }
        }
        
        printf("\n");
        
        if (opcode == OP_HALT) {
            break;
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <program_file.exe>\n", argv[0]);
        printf("Использование: %s <файл_программы.exe>\n", argv[0]);
        return 1;
    }
    
    FILE* file = fopen(argv[1], "rb");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }
    
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (file_size > VM_MEMORY_SIZE) {
        printf("File too large for VM memory\n");
        printf("Файл слишком велик для памяти VM\n");
        fclose(file);
        return 1;
    }
    
    uint8_t* program = malloc(file_size);
    if (!program) {
        printf("Memory allocation failed\n");
        printf("Ошибка выделения памяти\n");
        fclose(file);
        return 1;
    }
    
    fread(program, 1, file_size, file);
    fclose(file);
    
    disassemble_program(program, file_size);
    
    free(program);
    return 0;
}