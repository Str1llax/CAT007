#include <fstream>
#include <functional>
#include <unordered_map>

#define DEBUG_MODE
#ifdef DEBUG_MODE
#include <bitset>
std::ofstream LOG_FILE("cat_logs.txt");
unsigned int LOG_COUNTER = 1;
#endif

#define byte unsigned char
#define byte16 unsigned short

#define THREE_BITS_MASK 0x7
#define EIGHT_BITS_MASK 0xFF

#define DEFAULT_FLAG 0b00000010
#define OVERFLOW_FLAG_MASK 0b00100000
#define ZERO_FLAG_MASK 0b00010000
#define EOF_FLAG_MASK 0b00000100
#define CARRY_FLAG_MASK 0b00000001

namespace CATLogic {
    // Processor control
    /// @brief Function that does... absolutely nothing. It just ticks.
    /// @param input_file UNUSED. Stream from the input file.
    /// @param output_file UNUSED. Stream from the output file.
    /// @param instruction UNUSED. 16-bit value that represents 1 of 4 instruction types.
    /// @param flags UNUSED. Flag register. Resets every arithmetical/logical function and IN.
    /// @param REG UNUSED. 8 General purpose registers.
    /// @param program_counter UNUSED. Points to the current instruction's address in RAM.
    /// @param RAM Computers virtual RAM that stores instructions and temp data.
    void NOP(std::fstream& input_file, std::fstream& output_file, const byte16& instruction, byte& flags, byte*& REG, byte*& program_counter, byte*& RAM) {
        // nothing...
        ++program_counter;
#ifdef DEBUG_MODE
        LOG_FILE << LOG_COUNTER++ << ". NOP" << std::endl
        << "Instruction: " << std::bitset<16>(instruction) << std::endl;
#endif
    }

    /// @brief Stops machine.
    /// @param input_file UNUSED. Stream from the input file.
    /// @param output_file UNUSED. Stream from the output file.
    /// @param instruction 16-bit value that represents 1 of 4 instruction types.
    /// @param flags UNUSED. Flag register. Resets every arithmetical/logical function and IN.
    /// @param REG UNUSED. 8 General purpose registers.
    /// @param program_counter UNUSED. Points to the current instruction's address in RAM.
    /// @param RAM Computers virtual RAM that stores instructions and temp data.
    void HLT(std::fstream& input_file, std::fstream& output_file, const byte16& instruction, byte& flags, byte*& REG, byte*& program_counter, byte*& RAM) {
        RAM[253] |= 0x01;
#ifdef DEBUG_MODE
        LOG_FILE << LOG_COUNTER++ << ". HLT" << std::endl
        << "Instruction: " << std::bitset<16>(instruction) << std::endl;
#endif
    }
    // Data transfer

    /// @brief Reads 1 symbol(char) from input file.
    /// @param input_file Stream from the input file.
    /// @param output_file UNUSED. Stream from the output file.
    /// @param instruction 16-bit value that represents 1 of 4 instruction types.
    /// @param flags Flag register. Resets every arithmetical/logical function and IN.
    /// @param REG 8 General purpose registers.
    /// @param program_counter Points to the current instruction's address in RAM.
    /// @param RAM Computers virtual RAM that stores instructions and temp data.
    void IN(std::fstream& input_file, std::fstream& output_file, const byte16& instruction, byte& flags, byte*& REG, byte*& program_counter, byte*& RAM) {
        /*
         * 1. Keeping all the unnecessary bits, we need only second.
         * 2. Shifting and masking mode bit from instruction.
         * 3. Moving bit from instruction to his place in RAM[253]
        */
        byte* mode = &RAM[253];
        *mode &= 0b11111101;
        *mode |= (instruction >> 7 & 0x1) << 1;
        byte* result_reg = &RAM[254];
        *result_reg &= 0b00011111;
        *result_reg |= (instruction >> 8 & THREE_BITS_MASK) << 5;
        byte* source_reg = &RAM[253];
        *source_reg &= 0b00011111;
        *source_reg |= (instruction >> 4 & THREE_BITS_MASK) << 5; // 1 source
        *source_reg &= 0b11100011;
        *source_reg |= (instruction & THREE_BITS_MASK) << 2; // 2 source
        byte* symbol = &RAM[255];
        if ((*mode & 0x02) == 0x02) {
            // not defined
        } else {
            if (input_file.read(reinterpret_cast<char*>(symbol), sizeof(byte))) {
                REG[(*result_reg >> 5)] = *symbol;
            } else {
                flags = flags | 0x4;
            }
        }
        ++program_counter;
#ifdef DEBUG_MODE
        LOG_FILE << LOG_COUNTER++ << ". IN" << std::endl
        << "Instruction: " << std::bitset<16>(instruction) << std::endl
        << "Read symbol: " << static_cast<char>(*symbol) << ' ' << std::bitset<8>(*symbol) << std::endl
        << "Written to register: " << static_cast<int>(*result_reg >> 5) << ' ' << std::bitset<3>(*result_reg >> 5) << std::endl;
#endif
    }


    /// @brief Writes 1 symbol to the output file.
    /// @param input_file UNUSED. Stream from the input file.
    /// @param output_file Stream from the output file.
    /// @param instruction 16-bit value that represents 1 of 4 instruction types.
    /// @param flags UNUSED. Flag register. Resets every arithmetical/logical function and IN.
    /// @param REG 8 General purpose registers.
    /// @param program_counter Points to the current instruction's address in RAM.
    /// @param RAM Computers virtual RAM that stores instructions and temp data.
    void OUT(std::fstream& input_file, std::fstream& output_file, const byte16& instruction, byte& flags, byte*& REG, byte*& program_counter, byte*& RAM) {
        /*
         * 1. Keeping all the unnecessary bits, we need only second.
         * 2. Shifting and masking mode bit from instruction.
         * 3. Moving bit from instruction to his place in RAM[253]
        */
        byte* mode = &RAM[253];
        *mode &= 0b11111101;
        *mode |= (instruction >> 7 & 0x1) << 1;
        byte* result_reg = &RAM[254];
        *result_reg &= 0b00011111;
        *result_reg |= (instruction >> 8 & THREE_BITS_MASK) << 5;
        byte* source_reg = &RAM[253];
        *source_reg &= 0b00011111;
        *source_reg |= (instruction >> 4 & THREE_BITS_MASK) << 5; // 1 source
        *source_reg &= 0b11100011;
        *source_reg |= (instruction & THREE_BITS_MASK) << 2; // 2 source
        if ((*mode & 0x02) == 0x02) {
            // not defined
        } else {
            output_file << static_cast<char>(REG[(*source_reg >> 5)]);
        }
        ++program_counter;
#ifdef DEBUG_MODE
        LOG_FILE << LOG_COUNTER++ << ". OUT" << std::endl
        << "Instruction: " << std::bitset<16>(instruction) << std::endl
        << "Wrote symbol: " << static_cast<char>(REG[(*source_reg >> 5)]) << ' ' << std::bitset<8>(REG[(*source_reg >> 5)]) << std::endl
        << "From register: " << static_cast<int>(*source_reg >> 5) << ' ' << std::bitset<3>(*source_reg >> 5) << std::endl;
#endif
    }

    /// @brief Copies value from one register to another.
    /// @param input_file UNUSED. Stream from the input file.
    /// @param output_file UNUSED. Stream from the output file.
    /// @param instruction 16-bit value that represents 1 of 4 instruction types.
    /// @param flags UNUSED. Flag register. Resets every arithmetical/logical function and IN.
    /// @param REG 8 General purpose registers.
    /// @param program_counter Points to the current instruction's address in RAM.
    /// @param RAM Computers virtual RAM that stores instructions and temp data.
    void MOV(std::fstream& input_file, std::fstream& output_file, const byte16& instruction, byte& flags, byte*& REG, byte*& program_counter, byte*& RAM) {
        /*
         * 1. Keeping all the unnecessary bits, we need only second.
         * 2. Shifting and masking mode bit from instruction.
         * 3. Moving bit from instruction to his place in RAM[253]
        */
        byte* mode = &RAM[253];
        *mode &= 0b11111101;
        *mode |= (instruction >> 7 & 0x1) << 1;
        byte* result_reg = &RAM[254];
        *result_reg &= 0b00011111;
        *result_reg |= (instruction >> 8 & THREE_BITS_MASK) << 5;
        byte* source_reg = &RAM[253];
        *source_reg &= 0b00011111;
        *source_reg |= (instruction >> 4 & THREE_BITS_MASK) << 5; // 1 source
        *source_reg &= 0b11100011;
        *source_reg |= (instruction & THREE_BITS_MASK) << 2; // 2 source
        if ((*mode & 0x02) == 0x02) {
            // not defined
        } else {
            REG[(*result_reg >> 5)] = REG[(*source_reg >> 5)];
        }
        ++program_counter;
#ifdef DEBUG_MODE
        LOG_FILE << LOG_COUNTER++ << ". MOV" << std::endl
        << "Instruction: " << std::bitset<16>(instruction) << std::endl
        << "Copied: " << static_cast<char>(REG[(*source_reg >> 5)]) << ' ' << std::bitset<8>(REG[(*source_reg >> 5)]) << std::endl
        << "From register: " << static_cast<int>(*source_reg >> 5) << ' ' << std::bitset<3>(*source_reg >> 5) << std::endl
        << "To register: " << static_cast<int>(*result_reg >> 5) << ' ' << std::bitset<3>(*result_reg >> 5) << std::endl;
#endif
    }

    /// @brief Copies constant value to the specified register.
    /// @param input_file UNUSED. Stream from the input file.
    /// @param output_file UNUSED. Stream from the output file.
    /// @param instruction 16-bit value that represents 1 of 4 instruction types.
    /// @param flags UNUSED. Flag register. Resets every arithmetical/logical function and IN.
    /// @param REG 8 General purpose registers.
    /// @param program_counter Points to the current instruction's address in RAM.
    /// @param RAM Computers virtual RAM that stores instructions and temp data.
    void MOVC(std::fstream& input_file, std::fstream& output_file, const byte16& instruction, byte& flags, byte*& REG, byte*& program_counter, byte*& RAM) {
        byte* result_reg = &RAM[254];
        *result_reg &= 0b00011111;
        *result_reg |= (instruction >> 8 & THREE_BITS_MASK) << 5;
        byte* constant = &RAM[255];
        *constant = instruction & EIGHT_BITS_MASK;
        REG[(*result_reg >> 5)] = *constant;
        ++program_counter;
#ifdef DEBUG_MODE
        LOG_FILE << LOG_COUNTER++ << ". MOVC" << std::endl
        << "Instruction: " << std::bitset<16>(instruction) << std::endl
        << "Copied: " << static_cast<char>(*constant) << ' ' << std::bitset<8>(*constant) << std::endl
        << "To register: " << static_cast<int>(*result_reg >> 5) << ' ' << std::bitset<3>(*result_reg >> 5) << std::endl;
#endif
    }
    // Jumping

    /// @brief Unconditionally increases/decreases program counter by a specific number of bytes.
    /// @param input_file UNUSED. Stream from the input file.
    /// @param output_file UNUSED. Stream from the output file.
    /// @param instruction 16-bit value that represents 1 of 4 instruction types.
    /// @param flags UNUSED. Flag register. Resets every arithmetical/logical function and IN.
    /// @param REG UNUSED. 8 General purpose registers.
    /// @param program_counter Points to the current instruction's address in RAM.
    /// @param RAM Computers virtual RAM that stores instructions and temp data.
    void JMP(std::fstream& input_file, std::fstream& output_file, const byte16& instruction, byte& flags, byte*& REG, byte*& program_counter, byte*& RAM) {
        byte* constant = &RAM[255];
        *constant = instruction & EIGHT_BITS_MASK;
        program_counter += static_cast<char>(*constant)-1;
#ifdef DEBUG_MODE
        LOG_FILE << LOG_COUNTER++ << ". JMP" << std::endl
        << "Instruction: " << std::bitset<16>(instruction) << std::endl
        << "By: " << static_cast<int>(static_cast<char>(*constant)) << ' ' << std::bitset<8>(*constant) << std::endl;
#endif
    }


    /// @brief Increases/decreases program counter by a specific number of bytes, if zero flag is active.
    /// @param input_file UNUSED. Stream from the input file.
    /// @param output_file UNUSED. Stream from the output file.
    /// @param instruction 16-bit value that represents 1 of 4 instruction types.
    /// @param flags Flag register. Resets every arithmetical/logical function and IN.
    /// @param REG UNUSED. 8 General purpose registers.
    /// @param program_counter Points to the current instruction's address in RAM.
    /// @param RAM Computers virtual RAM that stores instructions and temp data.
    void JZ(std::fstream& input_file, std::fstream& output_file, const byte16& instruction, byte& flags, byte*& REG, byte*& program_counter, byte*& RAM) {
        byte* constant = &RAM[255];
        *constant = instruction & EIGHT_BITS_MASK;
        if ((flags & ZERO_FLAG_MASK) == ZERO_FLAG_MASK) program_counter += static_cast<char>(*constant)-1;
        else ++program_counter;
#ifdef DEBUG_MODE
        LOG_FILE << LOG_COUNTER++ << ". JZ" << std::endl
        << "Instruction: " << std::bitset<16>(instruction) << std::endl
        << "Zero flag: " << ((flags & ZERO_FLAG_MASK) == ZERO_FLAG_MASK) << std::endl
        << "By: " << static_cast<int>(static_cast<char>(*constant)) << ' ' << std::bitset<8>(*constant) << std::endl;
#endif
    }

    /// @brief Increases/decreases program counter by a specific number of bytes, if zero flag is inactive.
    /// @param input_file UNUSED. Stream from the input file.
    /// @param output_file UNUSED. Stream from the output file.
    /// @param instruction 16-bit value that represents 1 of 4 instruction types.
    /// @param flags Flag register. Resets every arithmetical/logical function and IN.
    /// @param REG UNUSED. 8 General purpose registers.
    /// @param program_counter Points to the current instruction's address in RAM.
    /// @param RAM Computers virtual RAM that stores instructions and temp data.
    void JNZ(std::fstream& input_file, std::fstream& output_file, const byte16& instruction, byte& flags, byte*& REG, byte*& program_counter, byte*& RAM) {
        byte* constant = &RAM[255];
        *constant = instruction & EIGHT_BITS_MASK;
        if ((flags & ZERO_FLAG_MASK) != ZERO_FLAG_MASK) program_counter += static_cast<char>(*constant)-1;
        else ++program_counter;
#ifdef DEBUG_MODE
        LOG_FILE << LOG_COUNTER++ << ". JNZ" << std::endl
        << "Instruction: " << std::bitset<16>(instruction) << std::endl
        << "Zero flag: " << ((flags & ZERO_FLAG_MASK) == ZERO_FLAG_MASK) << std::endl
        << "By: " << static_cast<int>(static_cast<char>(*constant)) << ' ' << std::bitset<8>(*constant) << std::endl;
#endif
    }

    /// @brief Increases/decreases program counter by a specific number of bytes, if carry flag is active.
    /// @param input_file UNUSED. Stream from the input file.
    /// @param output_file UNUSED. Stream from the output file.
    /// @param instruction 16-bit value that represents 1 of 4 instruction types.
    /// @param flags Flag register. Resets every arithmetical/logical function and IN.
    /// @param REG UNUSED. 8 General purpose registers.
    /// @param program_counter Points to the current instruction's address in RAM.
    /// @param RAM Computers virtual RAM that stores instructions and temp data.
    void JC(std::fstream& input_file, std::fstream& output_file, const byte16& instruction, byte& flags, byte*& REG, byte*& program_counter, byte*& RAM) {
        byte* constant = &RAM[255];
        *constant = instruction & EIGHT_BITS_MASK;
        if ((flags & CARRY_FLAG_MASK) == CARRY_FLAG_MASK) program_counter += static_cast<char>(*constant)-1;
        else ++program_counter;
#ifdef DEBUG_MODE
        LOG_FILE << LOG_COUNTER++ << ". JNZ" << std::endl
        << "Instruction: " << std::bitset<16>(instruction) << std::endl
        << "Carry flag: " << ((flags & CARRY_FLAG_MASK) == CARRY_FLAG_MASK) << std::endl
        << "By: " << static_cast<int>(static_cast<char>(*constant)) << ' ' << std::bitset<8>(*constant) << std::endl;
#endif
    }

    /// @brief Increases/decreases program counter by a specific number of bytes, if overflow flag is active.
    /// @param input_file UNUSED. Stream from the input file.
    /// @param output_file UNUSED. Stream from the output file.
    /// @param instruction 16-bit value that represents 1 of 4 instruction types.
    /// @param flags Flag register. Resets every arithmetical/logical function and IN.
    /// @param REG UNUSED. 8 General purpose registers.
    /// @param program_counter Points to the current instruction's address in RAM.
    /// @param RAM Computers virtual RAM that stores instructions and temp data.
    void JO(std::fstream& input_file, std::fstream& output_file, const byte16& instruction, byte& flags, byte*& REG, byte*& program_counter, byte*& RAM) {
        byte* constant = &RAM[255];
        *constant = instruction & EIGHT_BITS_MASK;
        if ((flags & OVERFLOW_FLAG_MASK) == OVERFLOW_FLAG_MASK) program_counter += static_cast<char>(*constant)-1;
        else ++program_counter;
#ifdef DEBUG_MODE
        LOG_FILE << LOG_COUNTER++ << ". JO" << std::endl
        << "Instruction: " << std::bitset<16>(instruction) << std::endl
        << "Overflow flag: " << ((flags & OVERFLOW_FLAG_MASK) == OVERFLOW_FLAG_MASK) << std::endl
        << "By: " << static_cast<int>(static_cast<char>(*constant)) << ' ' << std::bitset<8>(*constant) << std::endl;
#endif
    }

    /// @brief Increases/decreases program counter by a specific number of bytes, if EOF flag is active.
    /// @param input_file UNUSED. Stream from the input file.
    /// @param output_file UNUSED. Stream from the output file.
    /// @param instruction 16-bit value that represents 1 of 4 instruction types.
    /// @param flags Flag register. Resets every arithmetical/logical function and IN.
    /// @param REG UNUSED. 8 General purpose registers.
    /// @param program_counter Points to the current instruction's address in RAM.
    /// @param RAM Computers virtual RAM that stores instructions and temp data.
    void JFE(std::fstream& input_file, std::fstream& output_file, const byte16& instruction, byte& flags, byte*& REG, byte*& program_counter, byte*& RAM) {
        byte* constant = &RAM[255];
        *constant = instruction & EIGHT_BITS_MASK;
        if ((flags & EOF_FLAG_MASK) == EOF_FLAG_MASK) program_counter += static_cast<char>(*constant)-1;
        else ++program_counter;
#ifdef DEBUG_MODE
        LOG_FILE << LOG_COUNTER++ << ". JFE" << std::endl
        << "Instruction: " << std::bitset<16>(instruction) << std::endl
        << "EOF flag: " << ((flags & EOF_FLAG_MASK) == EOF_FLAG_MASK) << std::endl
        << "By: " << static_cast<int>(static_cast<char>(*constant)) << ' ' << std::bitset<8>(*constant) << std::endl;
#endif
    }
    // Arithmetic and bitwise
    /// @brief Increments value in a specific register.
    /// @note Toggles carry | overflow | zero flags.
    /// @param input_file UNUSED. Stream from the input file.
    /// @param output_file UNUSED. Stream from the output file.
    /// @param instruction 16-bit value that represents 1 of 4 instruction types.
    /// @param flags Flag register. Resets every arithmetical/logical function and IN.
    /// @param REG 8 General purpose registers.
    /// @param program_counter Points to the current instruction's address in RAM.
    /// @param RAM Computers virtual RAM that stores instructions and temp data.
    void INC(std::fstream& input_file, std::fstream& output_file, const byte16& instruction, byte& flags, byte*& REG, byte*& program_counter, byte*& RAM) {
        flags = DEFAULT_FLAG;
        byte* result_reg = &RAM[254];
        *result_reg &= 0b00011111;
        *result_reg |= (instruction >> 8 & THREE_BITS_MASK) << 5;
        if (REG[(*result_reg >> 5)] == 0xFF) flags = flags | 0x1; //carry
        if (REG[(*result_reg >> 5)] == 0x7F) flags = flags | 0x20; //overflow
        ++REG[(*result_reg >> 5)];
        if (REG[(*result_reg >> 5)] == 0x00) flags = flags | 0x10; //zero
        ++program_counter;
#ifdef DEBUG_MODE
        LOG_FILE << LOG_COUNTER++ << ". INC" << std::endl
        << "Instruction: " << std::bitset<16>(instruction) << std::endl
        << "Incremented: " << static_cast<char>(REG[(*result_reg >> 5)]-1) << ' ' << std::bitset<8>(REG[(*result_reg >> 5)]-1) << std::endl
        << "In register: " << static_cast<int>(*result_reg >> 5) << ' ' << std::bitset<3>(*result_reg >> 5) << std::endl
        << "Result: " << REG[(*result_reg >> 5)] << ' ' << std::bitset<8>(REG[(*result_reg >> 5)]) << std::endl;
#endif
    }

    /// @brief Decrements value in a specific register.
    /// @note Toggles carry | overflow | zero flags.
    /// @param input_file UNUSED. Stream from the input file.
    /// @param output_file UNUSED. Stream from the output file.
    /// @param instruction 16-bit value that represents 1 of 4 instruction types.
    /// @param flags Flag register. Resets every arithmetical/logical function and IN.
    /// @param REG 8 General purpose registers.
    /// @param program_counter Points to the current instruction's address in RAM.
    /// @param RAM Computers virtual RAM that stores instructions and temp data.
    void DEC(std::fstream& input_file, std::fstream& output_file, const byte16& instruction, byte& flags, byte*& REG, byte*& program_counter, byte*& RAM) {
        flags = DEFAULT_FLAG;
        byte* result_reg = &RAM[254];
        *result_reg &= 0b00011111;
        *result_reg |= (instruction >> 8 & THREE_BITS_MASK) << 5;
        if (REG[(*result_reg >> 5)] == 0x00) flags = flags | 0x1 | 0x20; // carry | overflow
        if (REG[(*result_reg >> 5)] == 0x80) flags = flags | 0x20; // overflow
        --REG[(*result_reg >> 5)];
        if (REG[(*result_reg >> 5)] == 0x00) flags = flags | 0x10; //zero
        ++program_counter;
#ifdef DEBUG_MODE
        LOG_FILE << LOG_COUNTER++ << ". INC" << std::endl
        << "Instruction: " << std::bitset<16>(instruction) << std::endl
        << "Decremented: " << static_cast<char>(REG[(*result_reg >> 5)]+1) << ' ' << std::bitset<8>(REG[(*result_reg >> 5)]+1) << std::endl
        << "In register: " << static_cast<int>(*result_reg >> 5) << ' ' << std::bitset<3>(*result_reg >> 5) << std::endl
        << "Result: " << REG[(*result_reg >> 5)] << ' ' << std::bitset<8>(REG[(*result_reg >> 5)]) << std::endl;
#endif
    }

    /// @brief Arithmetical addition between specified registers.
    /// @note Toggles carry | overflow | zero flags. Mode specifies whether A=B+C (1) or A+=B (0) will be performed.
    /// @param input_file UNUSED. Stream from the input file.
    /// @param output_file UNUSED. Stream from the output file.
    /// @param instruction 16-bit value that represents 1 of 4 instruction types.
    /// @param flags Flag register. Resets every arithmetical/logical function and IN.
    /// @param REG 8 General purpose registers.
    /// @param program_counter Points to the current instruction's address in RAM.
    /// @param RAM Computers virtual RAM that stores instructions and temp data.
    void ADD(std::fstream& input_file, std::fstream& output_file, const byte16& instruction, byte& flags, byte*& REG, byte*& program_counter, byte*& RAM) {
        flags = DEFAULT_FLAG;
        /*
         * 1. Keeping all the unnecessary bits, we need only second.
         * 2. Shifting and masking mode bit from instruction.
         * 3. Moving bit from instruction to his place in RAM[253]
        */
        byte* mode = &RAM[253];
        *mode &= 0b11111101;
        *mode |= (instruction >> 7 & 0x1) << 1;
        byte* result_reg = &RAM[254];
        *result_reg &= 0b00011111;
        *result_reg |= (instruction >> 8 & THREE_BITS_MASK) << 5;
        byte* source_reg = &RAM[253];
        *source_reg &= 0b00011111;
        *source_reg |= (instruction >> 4 & THREE_BITS_MASK) << 5; // 1 source
        *source_reg &= 0b11100011;
        *source_reg |= (instruction & THREE_BITS_MASK) << 2; // 2 source
        if ((*mode & 0x02) == 0x02) {
            REG[(*result_reg >> 5)] = REG[(*source_reg >> 5)] + REG[((*source_reg & 0x1C) >> 2)];
            if (REG[(*result_reg >> 5)] < REG[(*source_reg >> 5)] || REG[(*result_reg >> 5)] < REG[((*source_reg & 0x1C) >> 2)]) flags = flags | 0x1; //carry
            if ((REG[(*source_reg >> 5)] & 0x80) == (REG[((*source_reg & 0x1C) >> 2)] & 0x80) && (REG[(*result_reg >> 5)] & 0x80) != (REG[(*source_reg >> 5)] & 0x80)) flags = flags | 0x20; //overflow
        } else {
            byte* previous_res = &REG[255];
            *previous_res = REG[(*result_reg >> 5)];
            REG[(*result_reg >> 5)] = REG[(*result_reg >> 5)] + REG[(*source_reg >> 5)];
            if (REG[(*result_reg >> 5)] < REG[(*source_reg >> 5)] || REG[(*result_reg >> 5)] < *previous_res) flags = flags | 0x1; //carry
            if ((*previous_res & 0x80) == (REG[(*result_reg >> 5)] & 0x80) && (REG[(*result_reg >> 5)] & 0x80) != (REG[(*source_reg >> 5)] & 0x80)) flags = flags | 0x20; //overflow
        }
        if (REG[(*result_reg >> 5)] == 0x00) flags = flags | 0x10; //zero
        ++program_counter;
#ifdef DEBUG_MODE
        LOG_FILE << LOG_COUNTER++ << ". ADD" << std::endl
        << "Instruction: " << std::bitset<16>(instruction) << std::endl
        << "Mode: " << std::bitset<1>(*mode & 0x02) << std::endl
        << "To: " << static_cast<char>(*result_reg >> 5) << ' ' << std::bitset<3>(*result_reg >> 5) << std::endl
        << "From: " << static_cast<char>(*source_reg >> 5) << ' ' << std::bitset<3>(*source_reg >> 5) << std::endl
        << "SRC2: " << static_cast<char>((*source_reg & 0x1C) >> 2) << ' ' << std::bitset<3>((*source_reg & 0x1C) >> 2) << std::endl
        << "Result: " << REG[(*result_reg >> 5)] << ' ' << std::bitset<8>(REG[(*result_reg >> 5)]) << std::endl;
#endif
    }

    /// @brief Arithmetical substraction between specified registers.
    /// @note Toggles carry | overflow | zero flags. Mode specifies whether A=B-C (1) or A-=B (0) will be performed.
    /// @param input_file UNUSED. Stream from the input file.
    /// @param output_file UNUSED. Stream from the output file.
    /// @param instruction 16-bit value that represents 1 of 4 instruction types.
    /// @param flags Flag register. Resets every arithmetical/logical function and IN.
    /// @param REG 8 General purpose registers.
    /// @param program_counter Points to the current instruction's address in RAM.
    /// @param RAM Computers virtual RAM that stores instructions and temp data.
    void SUB(std::fstream& input_file, std::fstream& output_file, const byte16& instruction, byte& flags, byte*& REG, byte*& program_counter, byte*& RAM) {
        flags = DEFAULT_FLAG;
        /*
         * 1. Keeping all the unnecessary bits, we need only second.
         * 2. Shifting and masking mode bit from instruction.
         * 3. Moving bit from instruction to his place in RAM[253]
        */
        byte* mode = &RAM[253];
        *mode &= 0b11111101;
        *mode |= (instruction >> 7 & 0x1) << 1;
        byte* result_reg = &RAM[254];
        *result_reg &= 0b00011111;
        *result_reg |= (instruction >> 8 & THREE_BITS_MASK) << 5;
        byte* source_reg = &RAM[253];
        *source_reg &= 0b00011111;
        *source_reg |= (instruction >> 4 & THREE_BITS_MASK) << 5; // 1 source
        *source_reg &= 0b11100011;
        *source_reg |= (instruction & THREE_BITS_MASK) << 2; // 2 source
        if ((*mode & 0x02) == 0x02) {
            REG[(*result_reg >> 5)] = REG[(*source_reg >> 5)] - REG[((*source_reg & 0x1C) >> 2)];
            if (REG[(*result_reg >> 5)] > REG[(*source_reg >> 5)] || REG[(*result_reg >> 5)] > REG[((*source_reg & 0x1C) >> 2)]) flags = flags | 0x1; //carry
            if ((REG[(*source_reg >> 5)] & 0x80) == (REG[((*source_reg & 0x1C) >> 2)] & 0x80) && (REG[(*result_reg >> 5)] & 0x80) != (REG[(*source_reg >> 5)] & 0x80)) flags = flags | 0x20; //overflow
        } else {
            byte* previous_res = &REG[255];
            *previous_res = REG[(*result_reg >> 5)];
            REG[(*result_reg >> 5)] = REG[(*result_reg >> 5)] - REG[(*source_reg >> 5)];
            if (REG[(*result_reg >> 5)] > REG[(*source_reg >> 5)] || REG[(*result_reg >> 5)] > *previous_res) flags = flags | 0x1; //carry
            if ((*previous_res & 0x80) == (REG[(*result_reg >> 5)] & 0x80) && (REG[(*result_reg >> 5)] & 0x80) != (REG[(*source_reg >> 5)] & 0x80)) flags = flags | 0x20; //overflow
        }
        if (REG[(*result_reg >> 5)] == 0x00) flags = flags | 0x10; //zero
        ++program_counter;
#ifdef DEBUG_MODE
        LOG_FILE << LOG_COUNTER++ << ". SUB" << std::endl
        << "Instruction: " << std::bitset<16>(instruction) << std::endl
        << "Mode: " << std::bitset<1>(*mode & 0x02) << std::endl
        << "To: " << static_cast<char>(*result_reg >> 5) << ' ' << std::bitset<3>(*result_reg >> 5) << std::endl
        << "From: " << static_cast<char>(*source_reg >> 5) << ' ' << std::bitset<3>(*source_reg >> 5) << std::endl
        << "SRC2: " << static_cast<char>((*source_reg & 0x1C) >> 2) << ' ' << std::bitset<3>((*source_reg & 0x1C) >> 2) << std::endl
        << "Result: " << REG[(*result_reg >> 5)] << ' ' << std::bitset<8>(REG[(*result_reg >> 5)]) << std::endl;
#endif
    }

    /// @brief Logical XOR operation between specified registers.
    /// @note Toggles zero flag. Mode specifies whether A=B XOR C (1) or A=A XOR B (0) will be performed.
    /// @param input_file UNUSED. Stream from the input file.
    /// @param output_file UNUSED. Stream from the output file.
    /// @param instruction 16-bit value that represents 1 of 4 instruction types.
    /// @param flags Flag register. Resets every arithmetical/logical function and IN.
    /// @param REG 8 General purpose registers.
    /// @param program_counter Points to the current instruction's address in RAM.
    /// @param RAM Computers virtual RAM that stores instructions and temp data.
    void XOR(std::fstream& input_file, std::fstream& output_file, const byte16& instruction, byte& flags, byte*& REG, byte*& program_counter, byte*& RAM) {
        flags = DEFAULT_FLAG;
        /*
         * 1. Keeping all the unnecessary bits, we need only second.
         * 2. Shifting and masking mode bit from instruction.
         * 3. Moving bit from instruction to his place in RAM[253]
         */
        byte* mode = &RAM[253];
        *mode &= 0b11111101;
        *mode |= (instruction >> 7 & 0x1) << 1;
        byte* result_reg = &RAM[254];
        *result_reg &= 0b00011111;
        *result_reg |= (instruction >> 8 & THREE_BITS_MASK) << 5;
        byte* source_reg = &RAM[253];
        *source_reg &= 0b00011111;
        *source_reg |= (instruction >> 4 & THREE_BITS_MASK) << 5; // 1 source
        *source_reg &= 0b11100011;
        *source_reg |= (instruction & THREE_BITS_MASK) << 2; // 2 source
        if ((*mode & 0x02) == 0x02) {
            REG[(*result_reg >> 5)] = REG[(*source_reg >> 5)] ^ REG[((*source_reg & 0x1C) >> 2)];
        } else {
            REG[(*result_reg >> 5)] = REG[(*result_reg >> 5)] ^ REG[(*source_reg >> 5)];
        }
        if (REG[(*result_reg >> 5)] == 0x00) flags = flags | 0x10; //zero
        ++program_counter;
#ifdef DEBUG_MODE
        LOG_FILE << LOG_COUNTER++ << ". XOR" << std::endl
        << "Instruction: " << std::bitset<16>(instruction) << std::endl
        << "Mode: " << std::bitset<1>(*mode & 0x02) << std::endl
        << "To: " << static_cast<char>(*result_reg >> 5) << ' ' << std::bitset<3>(*result_reg >> 5) << std::endl
        << "From: " << static_cast<char>(*source_reg >> 5) << ' ' << std::bitset<3>(*source_reg >> 5) << std::endl
        << "SRC2: " << static_cast<char>((*source_reg & 0x1C) >> 2) << ' ' << std::bitset<3>((*source_reg & 0x1C) >> 2) << std::endl
        << "Result: " << REG[(*result_reg >> 5)] << ' ' << std::bitset<8>(REG[(*result_reg >> 5)]) << std::endl;
#endif
    }

    /// @brief Logical OR operation between specified registers.
    /// @note Toggles zero flag. Mode specifies whether A=B OR C (1) or A=A OR B (0) will be performed.
    /// @param input_file UNUSED. Stream from the input file.
    /// @param output_file UNUSED. Stream from the output file.
    /// @param instruction 16-bit value that represents 1 of 4 instruction types.
    /// @param flags Flag register. Resets every arithmetical/logical function and IN.
    /// @param REG 8 General purpose registers.
    /// @param program_counter Points to the current instruction's address in RAM.
    /// @param RAM Computers virtual RAM that stores instructions and temp data.
    void OR(std::fstream& input_file, std::fstream& output_file, const byte16& instruction, byte& flags, byte*& REG, byte*& program_counter, byte*& RAM) {
        flags = DEFAULT_FLAG;
        /*
         * 1. Keeping all the unnecessary bits, we need only second.
         * 2. Shifting and masking mode bit from instruction.
         * 3. Moving bit from instruction to his place in RAM[253]
        */
        byte* mode = &RAM[253];
        *mode &= 0b11111101;
        *mode |= (instruction >> 7 & 0x1) << 1;
        byte* result_reg = &RAM[254];
        *result_reg &= 0b00011111;
        *result_reg |= (instruction >> 8 & THREE_BITS_MASK) << 5;
        byte* source_reg = &RAM[253];
        *source_reg &= 0b00011111;
        *source_reg |= (instruction >> 4 & THREE_BITS_MASK) << 5; // 1 source
        *source_reg &= 0b11100011;
        *source_reg |= (instruction & THREE_BITS_MASK) << 2; // 2 source
        if ((*mode & 0x02) == 0x02) {
            REG[(*result_reg >> 5)] = REG[(*source_reg >> 5)] | REG[((*source_reg & 0x1C) >> 2)];
        } else {
            REG[(*result_reg >> 5)] = REG[(*result_reg >> 5)] | REG[(*source_reg >> 5)];
        }
        if (REG[(*result_reg >> 5)] == 0x00) flags = flags | 0x10; //zero
        ++program_counter;
#ifdef DEBUG_MODE
        LOG_FILE << LOG_COUNTER++ << ". OR" << std::endl
        << "Instruction: " << std::bitset<16>(instruction) << std::endl
        << "Mode: " << std::bitset<1>(*mode & 0x02) << std::endl
        << "To: " << static_cast<char>(*result_reg >> 5) << ' ' << std::bitset<3>(*result_reg >> 5) << std::endl
        << "From: " << static_cast<char>(*source_reg >> 5) << ' ' << std::bitset<3>(*source_reg >> 5) << std::endl
        << "SRC2: " << static_cast<char>((*source_reg & 0x1C) >> 2) << ' ' << std::bitset<3>((*source_reg & 0x1C) >> 2) << std::endl
        << "Result: " << REG[(*result_reg >> 5)] << ' ' << std::bitset<8>(REG[(*result_reg >> 5)]) << std::endl;
#endif
    }

    /// @brief Logical AND operation between specified registers.
    /// @note Toggles zero flag. Mode specifies whether A=B AND C (1) or A=A AND B (0) will be performed.
    /// @param input_file UNUSED. Stream from the input file.
    /// @param output_file UNUSED. Stream from the output file.
    /// @param instruction 16-bit value that represents 1 of 4 instruction types.
    /// @param flags Flag register. Resets every arithmetical/logical function and IN.
    /// @param REG 8 General purpose registers.
    /// @param program_counter Points to the current instruction's address in RAM.
    /// @param RAM Computers virtual RAM that stores instructions and temp data.
    void AND(std::fstream& input_file, std::fstream& output_file, const byte16& instruction, byte& flags, byte*& REG, byte*& program_counter, byte*& RAM) {
        flags = DEFAULT_FLAG;
        /*
         * 1. Keeping all the unnecessary bits, we need only second.
         * 2. Shifting and masking mode bit from instruction.
         * 3. Moving bit from instruction to his place in RAM[253]
        */
        byte* mode = &RAM[253];
        *mode &= 0b11111101;
        *mode |= (instruction >> 7 & 0x1) << 1;
        byte* result_reg = &RAM[254];
        *result_reg &= 0b00011111;
        *result_reg |= (instruction >> 8 & THREE_BITS_MASK) << 5;
        byte* source_reg = &RAM[253];
        *source_reg &= 0b00011111;
        *source_reg |= (instruction >> 4 & THREE_BITS_MASK) << 5; // 1 source
        *source_reg &= 0b11100011;
        *source_reg |= (instruction & THREE_BITS_MASK) << 2; // 2 source
        if ((*mode & 0x02) == 0x02) {
            REG[(*result_reg >> 5)] = REG[(*source_reg >> 5)] & REG[((*source_reg & 0x1C) >> 2)];
        } else {
            REG[(*result_reg >> 5)] = REG[(*result_reg >> 5)] & REG[(*source_reg >> 5)];
        }
        if (REG[(*result_reg >> 5)] == 0x00) flags = flags | 0x10; //zero
        ++program_counter;
#ifdef DEBUG_MODE
        LOG_FILE << LOG_COUNTER++ << ". AND" << std::endl
        << "Instruction: " << std::bitset<16>(instruction) << std::endl
        << "Mode: " << std::bitset<1>(*mode & 0x02) << std::endl
        << "To: " << static_cast<char>(*result_reg >> 5) << ' ' << std::bitset<3>(*result_reg >> 5) << std::endl
        << "From: " << static_cast<char>(*source_reg >> 5) << ' ' << std::bitset<3>(*source_reg >> 5) << std::endl
        << "SRC2: " << static_cast<char>((*source_reg & 0x1C) >> 2) << ' ' << std::bitset<3>((*source_reg & 0x1C) >> 2) << std::endl
        << "Result: " << REG[(*result_reg >> 5)] << ' ' << std::bitset<8>(REG[(*result_reg >> 5)]) << std::endl;
#endif
    }

    /// @brief Logical Shift to the Right operation for the specified register.
    /// @note Toggles zero flag. Can be used as arithmetical division by 2 for unsigned numbers.
    /// @param input_file UNUSED. Stream from the input file.
    /// @param output_file UNUSED. Stream from the output file.
    /// @param instruction 16-bit value that represents 1 of 4 instruction types.
    /// @param flags Flag register. Resets every arithmetical/logical function and IN.
    /// @param REG 8 General purpose registers.
    /// @param program_counter Points to the current instruction's address in RAM.
    /// @param RAM Computers virtual RAM that stores instructions and temp data.
    void LSR(std::fstream& input_file, std::fstream& output_file, const byte16& instruction, byte& flags, byte*& REG, byte*& program_counter, byte*& RAM) {
        flags = DEFAULT_FLAG;
        /*
         * 1. Keeping all the unnecessary bits, we need only second.
         * 2. Shifting and masking mode bit from instruction.
         * 3. Moving bit from instruction to his place in RAM[253]
        */
        byte* mode = &RAM[253];
        *mode &= 0b11111101;
        *mode |= (instruction >> 7 & 0x1) << 1;
        byte* result_reg = &RAM[254];
        *result_reg &= 0b00011111;
        *result_reg |= (instruction >> 8 & THREE_BITS_MASK) << 5;
        byte* source_reg = &RAM[253];
        *source_reg &= 0b00011111;
        *source_reg |= (instruction >> 4 & THREE_BITS_MASK) << 5; // 1 source
        *source_reg &= 0b11100011;
        *source_reg |= (instruction & THREE_BITS_MASK) << 2; // 2 source
        if ((*mode & 0x02) == 0x02) {
            // not defined
        } else {
            REG[(*result_reg >> 5)] = REG[(*result_reg >> 5)] >> 1;
        }
        if (REG[(*result_reg >> 5)] == 0x00) flags = flags | 0x10; //zero
        ++program_counter;
#ifdef DEBUG_MODE
        LOG_FILE << LOG_COUNTER++ << ". LSR" << std::endl
        << "Instruction: " << std::bitset<16>(instruction) << std::endl
        << "Mode: " << std::bitset<1>(*mode & 0x02) << std::endl
        << "To: " << static_cast<char>(*result_reg >> 5) << ' ' << std::bitset<3>(*result_reg >> 5) << std::endl
        << "From: " << static_cast<char>(*source_reg  >> 5) << ' ' << std::bitset<3>(*source_reg >> 5) << std::endl
        << "Result: " << REG[(*result_reg >> 5)] << ' ' << std::bitset<8>(REG[(*result_reg >> 5)]) << std::endl;
#endif
    }

    /// @brief Logical Shift to the Left operation for the specified register.
    /// @note Toggles zero flag. Can be used as arithmetical multiplication by 2 for unsigned numbers.
    /// @param input_file UNUSED. Stream from the input file.
    /// @param output_file UNUSED. Stream from the output file.
    /// @param instruction 16-bit value that represents 1 of 4 instruction types.
    /// @param flags Flag register. Resets every arithmetical/logical function and IN.
    /// @param REG 8 General purpose registers.
    /// @param program_counter Points to the current instruction's address in RAM.
    /// @param RAM Computers virtual RAM that stores instructions and temp data.
    void LSL(std::fstream& input_file, std::fstream& output_file, const byte16& instruction, byte& flags, byte*& REG, byte*& program_counter, byte*& RAM) {
        flags = DEFAULT_FLAG;
        /*
         * 1. Keeping all the unnecessary bits, we need only second.
         * 2. Shifting and masking mode bit from instruction.
         * 3. Moving bit from instruction to his place in RAM[253]
        */
        byte* mode = &RAM[253];
        *mode &= 0b11111101;
        *mode |= (instruction >> 7 & 0x1) << 1;
        byte* result_reg = &RAM[254];
        *result_reg &= 0b00011111;
        *result_reg |= (instruction >> 8 & THREE_BITS_MASK) << 5;
        byte* source_reg = &RAM[253];
        *source_reg &= 0b00011111;
        *source_reg |= (instruction >> 4 & THREE_BITS_MASK) << 5; // 1 source
        *source_reg &= 0b11100011;
        *source_reg |= (instruction & THREE_BITS_MASK) << 2; // 2 source
        if ((*mode & 0x02) == 0x02) {
            // not defined
        } else {
            REG[(*result_reg >> 5)] = REG[(*result_reg >> 5)] << 1;
        }
        if (REG[(*result_reg >> 5)] == 0x00) flags = flags | 0x10; //zero
        ++program_counter;
#ifdef DEBUG_MODE
        LOG_FILE << LOG_COUNTER++ << ". LSL" << std::endl
        << "Instruction: " << std::bitset<16>(instruction) << std::endl
        << "Mode: " << std::bitset<1>(*mode & 0x02) << std::endl
        << "To: " << static_cast<char>(*result_reg >> 5) << ' ' << std::bitset<3>(*result_reg >> 5) << std::endl
        << "From: " << static_cast<char>(*source_reg  >> 5) << ' ' << std::bitset<3>(*source_reg >> 5) << std::endl
        << "Result: " << REG[(*result_reg >> 5)] << ' ' << std::bitset<8>(REG[(*result_reg >> 5)]) << std::endl;
#endif
    }

    std::unordered_map<byte, std::function<void(std::fstream&, std::fstream&, const byte16&, byte&, byte*&, byte*&, byte*&)>> INSTRUCTION_SET = {
        {0x00, NOP},
        {0x0B, HLT},
        {0x01, IN},
        {0x02, OUT},
        {0x03, MOV},
        {0x04, MOVC},
        {0x05, JMP},
        {0x06, JZ},
        {0x07, JNZ},
        {0x08, JC},
        {0x09, JO},
        {0x0A, JFE},
        {0x0C, INC},
        {0x0D, DEC},
        {0x0E, ADD},
        {0x0F, SUB},
        {0x10, XOR},
        {0x11, OR},
        {0x12, AND},
        {0x13, LSR},
        {0x14, LSL}
    };
}

class CAT007 {
    /**
     * RAM[255] - for symbol reading, instruction constant holding and previous result.\n
     * RAM[254] - 3 bits for result register, 5 bits for opcode.\n
     * RAM[253] - 3 bits for first source register, 3 bits for second source register, 1 bit for mode, 1 bit for halting state.\n
     * Accessing all those values with bitwise shifting and masking.\n
     * 253 bytes in RAM left for instructions. 1 instruction takes 2 bytes,
     * so MAX this computer can handle 126 instruction program and one RAM address will left empty.
     * Otherwise last instructions will be overridden in the process of executing.
     **/
private:
    byte* program_counter;
    byte* RAM;
    byte* REGs;
    byte* opcode;
    byte flag_reg;
    byte16 instruction_reg;
public:
    CAT007() {
        RAM = new byte[256]{};
        REGs = new byte[8]{};
        program_counter = &RAM[0];
        opcode = &RAM[254];
        flag_reg = DEFAULT_FLAG;
        instruction_reg = 0;
    }
    ~CAT007() {
        delete[] RAM;
        delete[] REGs;
    }
    void reset() {
        program_counter = &RAM[0];
    }
    void tick() {
        ++program_counter;
    }
    bool Halted() const {
        return (RAM[253] & 0x01) == 0x01;
    }
    byte* pc() const {
        return program_counter;
    }
    void fetch() {
        instruction_reg = *program_counter << 8;
        ++program_counter;
        instruction_reg += *program_counter;
#ifdef DEBUG_MODE
        LOG_FILE << "\nFetching instruction: " << std::bitset<16>(instruction_reg) << std::endl;
#endif
    }
    void decode() const {
        *opcode = instruction_reg >> 11;
#ifdef DEBUG_MODE
        LOG_FILE << "Decoding: " << std::endl
        << "Instruction: " << std::bitset<16>(instruction_reg) << std::endl
        << "OPCode: " << std::bitset<5>(*opcode) << std::endl;
#endif
    }
    void execute(std::fstream& input_file, std::fstream& output_file) {
        CATLogic::INSTRUCTION_SET[*opcode](input_file, output_file, instruction_reg, flag_reg, REGs, program_counter, RAM);
    }
};

int main() {
    std::fstream bin_file, input_file, output_file;
    bin_file.open("KittyDecryptorV1.bin", std::ios::in | std::ios::binary);
    if (!bin_file) {
        printf("Can not open .bin file.");
        return 101;
    }
    input_file.open("Decrypt.txt", std::ios::in);
    if (!input_file) {
        printf("Can not open encrypted text file.");
        return 102;
    }
    output_file.open("Encrypt.txt", std::ios::out);

    CAT007 Kampukter;
    auto symbol = new byte;
    while (bin_file.read(reinterpret_cast<char*>(symbol), sizeof(byte))) {
        *Kampukter.pc() = *symbol;
        Kampukter.tick();
    }
    delete symbol;
    bin_file.close();
    Kampukter.reset();

    while (!Kampukter.Halted()) {
        Kampukter.fetch();
        Kampukter.decode();
        Kampukter.execute(input_file, output_file);
    }

    input_file.close();
    output_file.close();

#ifdef DEBUG_MODE
    LOG_FILE.close();
#endif

    return 0;
}
