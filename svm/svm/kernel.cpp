#include "kernel.h"

#include <iostream>
#include <algorithm>

namespace svm
{
    Kernel::Kernel(
                Scheduler scheduler,
                std::vector<Memory::ram_type> executables_paths
            )
        : board(),
          processes(),
          priorities(),
          scheduler(scheduler),
          _last_issued_process_id(0),
          _last_ram_position(0),
          _cycles_passed_after_preemption(0),
          _current_process_index(0)
    {
        std::for_each(
            executables_paths.begin(),
            executables_paths.end(),
            [&](Memory::ram_type &executable) {
                CreateProcess(executable);
            }
        );

        if (scheduler == FirstComeFirstServed) {
            board.pic.isr_0 = [&]() {
                // ToDo: Process the timer interrupt for the FCFS
            };

            board.pic.isr_3 = [&]() {
                // ToDo: Process the first software interrupt for the FCFS
                // Unload the current process
            };
        } else if (scheduler == ShortestJob) {
            board.pic.isr_0 = [&]() {
                // ToDo: Process the timer interrupt for the Shortest
                //  Job scheduler
            };

            board.pic.isr_3 = [&]() {
                // ToDo: Process the first software interrupt for the Shortest
                //  Job scheduler

                // Unload the current process
            };
        } else if (scheduler == RoundRobin) {
            board.pic.isr_0 = [&]() {
                // ToDo: Process the timer interrupt for the Round Robin
                //  scheduler
            std::cout << "the timer in";
			if (processes.size() < 2) {
				return;
			}
			++_cycles_passed_after_preemption;
				std::cout << "the current cycle count: " << _cycles_passed_after_preemption << std::endl;
				if (_cycles_passed_after_preemption > _MAX_CYCLES_BEFORE_PREEMPTION) {
					process_list_type::size_type next_process_index = (_current_process_index + 1) % processes.size();
					std::cout << "The process: " << _current_process_index
						<< "has consumed its time slice" << _MAX_CYCLES_BEFORE_PREEMPTION << std::endl
						<< "Swithing to the next process: " << next_process_index
						<< std::endl;
					std::cout << "saving all registers from the CPU to the PCB of the previous process" << std::endl;
					processes[_current_process_index].registers = board.cpu.registers;
					processes[_current_process_index].state = Process::States::Ready;
					std::cout<< "Restoring all the registers from the PCB to CPU of the previous process" << std::endl;
					_current_process_index = next_process_index;
					board.cpu.registers = processes[_current_process_index].registers;
					processes[_current_process_index].state = Process::States::Running;
					_cycles_passed_after_preemption = 0;
				}
			};

            board.pic.isr_3 = [&]() {
                // ToDo: Process the first software interrupt for the
                //  Round Robin scheduler

                // Unload the current process
            };
        } else if (scheduler == Priority) {
            board.pic.isr_0 = [&]() {
                // ToDo: Process the timer interrupt for the Priority Queue
                //  scheduler
            };

            board.pic.isr_3 = [&]() {
                // ToDo: Process the first software interrupt for the Priority
                //  Queue scheduler

                // Unload the current process
            };
        }

        // ToDo

        // ---

        board.Start();
    }

    Kernel::~Kernel() { }

    void Kernel::CreateProcess(Memory::ram_type &executable)
    {
        std::copy(
            executable.begin(),
            executable.end(),
            board.memory.ram.begin() + _last_ram_position
        );

        Process process(
            _last_issued_process_id++,
            _last_ram_position,
            _last_ram_position + executable.size()
        );

        _last_ram_position +=
            executable.size();

        // ToDo: add the new process to an appropriate data structure
        processes.push_back(process);

        // ToDo: process the data structure

        // ---
    }
}
