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
