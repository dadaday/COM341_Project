#include "cpu.h"

#include <iostream>

namespace svm
{
    Registers::Registers()
        : a(0), b(0), c(0), flags(0), ip(0), sp(0) { }

    CPU::CPU(Memory &memory, PIC &pic)
        : registers(),
          _memory(memory),
          _pic(pic) { }

    CPU::~CPU() { }

    void CPU::Step()
    {
        int ip =
            registers.ip;

        int instruction =
            _memory.ram[ip];
        int data =
            _memory.ram[ip + 1];

        if (instruction ==
                CPU::MOVA_OPCODE) {
            std::cout << "Processing instruction: 'mov a'" << std::endl;
            registers.a = data;
            registers.ip += 2;
        } else if (instruction ==
                       CPU::MOVB_OPCODE) {
            std::cout << "Processing instruction: 'mov b'" << std::endl;
            registers.b = data;
            registers.ip += 2;
        } else if (instruction ==
                       CPU::MOVC_OPCODE) {
            std::cout << "Processing instruction: 'mov c'" << std::endl;
            registers.c = data;
            registers.ip += 2;
        } else if (instruction ==
                       CPU::JMP_OPCODE) {
            std::cout << "Processing instruction: 'jmp'" << std::endl;
            registers.ip += data;
        } else if (instruction ==
                       CPU::INT_OPCODE) {
            std::cout << "Processing instruction: 'int'" << std::endl;
            registers.ip += 2;
            switch (data)
            {
                case 1:
                    _pic.isr_3();
                    break;
                    // ...
                    //case 2:
                    //  _pic.isr_4();
                    //  break;
            }
        } else {
            std::cerr << "CPU: invalid opcode data. Skipping..."
                      << std::endl;
            registers.ip += 2;
        }
    }
}
