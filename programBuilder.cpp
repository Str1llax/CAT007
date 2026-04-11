#define BUILD
#ifdef BUILD

#include <fstream>
#include <string>
#include <unordered_map>

#define LOGGER
#ifdef LOGGER
#include <bitset>
std::fstream LOG_FILE("cat_encryption_log.txt", std::ios::out);
unsigned int LOG_COUNTER = 1;
#endif

#define byte unsigned char
#define byte16 unsigned short

namespace CATEncryption {
    struct Instruction_base {
        byte opcode;
        byte type;
    };

    std::unordered_map<std::string, Instruction_base> Instructions = {
        {"NOP", {0x00, 1}},
        {"HLT", {0x0B, 1}},
        {"IN", {0x01, 2}},
        {"OUT", {0x02, 2}},
        {"MOV", {0x03, 2}},
        {"MOVC", {0x04, 3}},
        {"JMP", {0x05, 4}},
        {"JZ", {0x06, 4}},
        {"JNZ", {0x07, 4}},
        {"JC", {0x08, 4}},
        {"JO", {0x09, 4}},
        {"JFE", {0x0A, 4}},
        {"INC", {0x0C, 3}},
        {"DEC", {0x0D, 3}},
        {"ADD", {0x0E, 2}},
        {"SUB", {0x0F, 2}},
        {"XOR", {0x10, 2}},
        {"OR", {0x11, 2}},
        {"AND", {0x12, 2}},
        {"LSR", {0x13, 2}},
        {"LSL", {0x14, 2}}
    };

    void Create1typeInstruction(byte16& instruct) {
        instruct &= ~0x1FF;
#ifdef LOGGER
        LOG_FILE << LOG_COUNTER << ". Created instruction:" << std::endl
        << "Type: 1" << std::endl
        << "Instruction: " << std::bitset<16>(instruct) << std::endl;
        ++LOG_COUNTER;
#endif
    }

    void Create2typeInstruction(std::fstream& in, byte16& instruct) {
        byte16 mode;
        byte16 res_reg, src_reg, src_reg2;
        in >> mode;
        if (mode == 1) {
            in >> res_reg >> src_reg >> src_reg2;
            instruct += (res_reg << 8);
            instruct |= 0x80;
            instruct += (src_reg << 4);
            instruct += src_reg2;
            instruct &= ~0x4;
        } else {
            in >> res_reg >> src_reg;
            instruct += (res_reg << 8);
            instruct &= ~0x80;
            instruct += (src_reg << 4);
            instruct &= ~0xF;
        }
#ifdef LOGGER
        LOG_FILE << LOG_COUNTER << ". Created instruction:" << std::endl
        << "Type: 2" << std::endl
        << "Mode: " << mode << std::endl
        << "Result register: " << std::bitset<3>(res_reg) << std::endl
        << "Source register: " << std::bitset<3>(src_reg) << std::endl;
        if (mode == 1) LOG_FILE << "2 Source register: " << std::bitset<3>(src_reg2) << std::endl;
        LOG_FILE << "Instruction: " << std::bitset<16>(instruct) << std::endl;
        ++LOG_COUNTER;
#endif
    }

    void Create3typeInstruction(std::fstream& in, byte16& instruct) {
        byte16 res_reg, constant;
        in >> res_reg >> constant;
        instruct |= (res_reg << 8);
        instruct |= static_cast<unsigned char>(constant);
#ifdef LOGGER
        LOG_FILE << LOG_COUNTER << ". Created instruction:" << std::endl
        << "Type: 3" << std::endl
        << "Result register: " << std::bitset<3>(res_reg) << std::endl
        << "Constant: " << std::bitset<8>(constant) << std::endl
        << "Instruction: " << std::bitset<16>(instruct) << std::endl;
        ++LOG_COUNTER;
#endif
    }

    void Create4typeInstruction(std::fstream& in, byte16& instruct) {
        byte16 constant;
        in >> constant;
        instruct |= static_cast<unsigned char>(constant);
        instruct &= 0xF8FF;
#ifdef LOGGER
        LOG_FILE << LOG_COUNTER << ". Created instruction:" << std::endl
        << "Type: 4" << std::endl
        << "Constant: " << static_cast<int>(static_cast<char>(constant)) << ' ' << std::bitset<8>(constant) << std::endl
        << "Instruction: " << std::bitset<16>(instruct) << std::endl;
        ++LOG_COUNTER;
#endif
    }

    void CreateInstructions(std::fstream& in, std::fstream& out) {
        std::string func;
        while (in >> func) {
            byte16 instruction = Instructions[func].opcode << 11;
#ifdef LOGGER
        LOG_FILE << LOG_COUNTER << ". Read name: " << func << std::endl
        << "Read: " << std::bitset<16>(instruction) << std::endl;
        ++LOG_COUNTER;
#endif
            switch (Instructions[func].type) {
                default:
                    printf("Incorrect instruction type (%d) for function %s\n", Instructions[func].type, func.c_str());
                case 1:
                    Create1typeInstruction(instruction);
                    break;
                case 2:
                    Create2typeInstruction(in, instruction);
                    break;
                case 3:
                    Create3typeInstruction(in, instruction);
                    break;
                case 4:
                    Create4typeInstruction(in, instruction);
                    break;
            }
            out.put(static_cast<char>(instruction >> 8));
            out.put(static_cast<char>(instruction));
            //out << instruction;
#ifdef LOGGER
        LOG_FILE << LOG_COUNTER << ". Written:" << std::bitset<16>(instruction) << std::endl;
        ++LOG_COUNTER;
#endif
        }
    }
}
#endif
int main() {
#ifdef BUILD
    std::fstream instruction_file, reference_file;
    instruction_file.open("KittyDecryptorV1.bin", std::ios::out | std::ios::binary | std::ios::trunc);
    reference_file.open("InstructionReference2.txt", std::ios::in);
    if (!reference_file) {
        printf("Can't open InstructionReference.txt file.");
        return 101;
    }

    CATEncryption::CreateInstructions(reference_file, instruction_file);

    instruction_file.close();
    reference_file.close();
#ifdef LOGGER
    LOG_FILE.close();
#endif

#endif
    return 0;
}
