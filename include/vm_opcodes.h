/**
 * Virtual Machine Opcode Definitions
 * Определения опкодов виртуальной машины
 */

#ifndef VM_OPCODES_H
#define VM_OPCODES_H

// Move/Copy operations (0x00-0x02)
// Операции перемещения/копирования
#define OP_MOV 0x01        // Move - перемещение
#define OP_XCHG 0x02       // Exchange - обмен

// Arithmetic operations (0x03-0x05)
// Арифметические операции
#define OP_ADD 0x03        // Addition - сложение
#define OP_SUB 0x04        // Subtraction - вычитание
#define OP_CMP 0x05        // Compare - сравнение

// Logical operations (0x06-0x0D)
// Логические операции
#define OP_AND 0x09        // Logical AND - логическое И
#define OP_OR 0x0A         // Logical OR - логическое ИЛИ
#define OP_XOR 0x0C        // Logical XOR - логическое исключающее ИЛИ

// Shift/Rotate operations (0x0E-0x13)
// Операции сдвига/вращения
#define OP_ROL 0x0E        // Rotate left - вращение влево
#define OP_ROR 0x0F        // Rotate right - вращение вправо
#define OP_SHL 0x12        // Shift left - сдвиг влево
#define OP_SHR 0x13        // Shift right - сдвиг вправо

// Increment/Decrement operations (0x16-0x18)
// Операции инкремента/декремента
#define OP_INC 0x16        // Increment - инкремент
#define OP_DEC 0x17        // Decrement - декремент
#define OP_NOT 0x18        // Bitwise NOT - побитовое НЕ

// Jump operations (0x19-0x27)
// Операции перехода
#define OP_JMP 0x28        // Unconditional jump - безусловный переход
#define OP_CALL 0x29       // Call subroutine - вызов подпрограммы
#define OP_RET 0x2A        // Return from subroutine - возврат из подпрограммы

// Stack operations (implicit)
// Операции со стеком (неявные)
// Uses return_stack[34] - использует return_stack[34]

// I/O operations (0x2D-0x2E)
// Операции ввода/вывода
#define OP_OUT 0x2D        // Output characters - вывод символов
#define OP_IN 0x2E         // Input characters - ввод символов

// System operations (0x2C)
// Системные операции
#define OP_HALT 0x2C       // Halt execution - остановка выполнения

// Extended operations (0x36-0x53)
// Расширенные операции
#define OP_ADC 0x36        // Add with carry - сложение с переносом
#define OP_SBB 0x37        // Subtract with borrow - вычитание с заемом
#define OP_RCL 0x45        // Rotate left through carry - вращение влево через перенос
#define OP_RCR 0x46        // Rotate right through carry - вращение вправо через перенос

// Conditional jumps (0x19-0x27)
// Условные переходы
#define OP_JE  0x19        // Jump if equal (ZF=1) - переход если равно
#define OP_JNE 0x1A        // Jump if not equal (ZF=0) - переход если не равно
#define OP_JL  0x1B        // Jump if less (SF≠OF) - переход если меньше
#define OP_JLE 0x1C        // Jump if less or equal (ZF=1 or SF≠OF) - переход если меньше или равно
#define OP_JB  0x1D        // Jump if below (CF=1) - переход если ниже (без знака)
#define OP_JBE 0x1E        // Jump if below or equal (CF=1 or ZF=1) - переход если ниже или равно
#define OP_JP  0x1F        // Jump if parity (PF=1) - переход если четность
#define OP_JO  0x20        // Jump if overflow (OF=1) - переход если переполнение
#define OP_JS  0x21        // Jump if sign (SF=1) - переход если знак
#define OP_JNL 0x22        // Jump if not less (SF=OF) - переход если не меньше
#define OP_JG  0x23        // Jump if greater (ZF=0 and SF=OF) - переход если больше
#define OP_JAE 0x24        // Jump if above or equal (CF=0) - переход если выше или равно
#define OP_JA  0x25        // Jump if above (CF=0 and ZF=0) - переход если выше
#define OP_JNP 0x26        // Jump if no parity (PF=0) - переход если нет четности
#define OP_JNO 0x27        // Jump if no overflow (OF=0) - переход если нет переполнения

#endif // VM_OPCODES_H