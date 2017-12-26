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
                std::cout << "Nothing to do in timer interrupt" << std::endl;
			};

			board.pic.isr_3 = [&]() {
				// ToDo: Process the first software interrupt for the FCFS
				// Unload the current process

                std::cout << "Processing the first software interrupt" << std::endl;
                std::cout << "Unloading the process " << processes[_current_process_index].id << std::endl;
                processes.erase(processes.begin() + _current_process_index);

                if (processes.empty()) {
                    std::cout << "No more processes. Stopping the machine" << std::endl;
                    board.Stop();
                }
                else {
                    std::cout << "Switching the context to process " << processes[_current_process_index].id << std::endl;
                    board.cpu.registers = processes[_current_process_index].registers;
                    processes[_current_process_index].state = Process::States::Running;
                    _cycles_passed_after_preemption = 0;
                }

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
				if (processes.size() < 2) {
					std::cout << std::endl << "Only one process. No scheduling necessary" << std::endl;
					return;
				}

				std::cout << std::endl << "Processing the timer interrupt" << std::endl;
				++_cycles_passed_after_preemption;
				
				if (_cycles_passed_after_preemption > _MAX_CYCLES_BEFORE_PREEMPTION) {
					process_list_type::size_type next_process_index = (_current_process_index + 1) % processes.size();
					std::cout << "The process " << processes[_current_process_index].id
						<< " has consumed its time slice " << _MAX_CYCLES_BEFORE_PREEMPTION << std::endl
						<< "Switching to the next process: " << processes[next_process_index].id
						<< std::endl << std::endl;
					std::cout << "saving all registers from the CPU to the PCB of the previous process" << std::endl;
					processes[_current_process_index].registers = board.cpu.registers;
					processes[_current_process_index].state = Process::States::Ready;
					std::cout<< "Restoring all the registers from the PCB to CPU of the previous process" << std::endl;
					_current_process_index = next_process_index;
					board.cpu.registers = processes[_current_process_index].registers;
					processes[_current_process_index].state = Process::States::Running;
					_cycles_passed_after_preemption = 1;
				}
				else {
					std::cout << "Allowing the current process " << processes[_current_process_index].id << " to run" << std::endl;
				}
				std::cout << "the current cycle is: " << _cycles_passed_after_preemption << std::endl;
			};

			board.pic.isr_3 = [&]() {
				// ToDo: Process the first software interrupt for the
				//  Round Robin scheduler
				std::cout << std::endl << "Processing the first software interrupt" << std::endl;
				// Unload the current process
				std::cout << "Unloading the process " << processes[_current_process_index].id << std::endl;
				processes.erase(processes.begin() + _current_process_index);

				if (processes.empty()) {
					std::cout << "No more processes. Stopping the machine" << std::endl;
					board.Stop();
				}
				else {
					if (_current_process_index >= processes.size()) {
						_current_process_index = 0;
					}
					std::cout << "Switching the context to process " << processes[_current_process_index].id << std::endl;
					board.cpu.registers = processes[_current_process_index].registers;
					processes[_current_process_index].state = Process::States::Running;
					_cycles_passed_after_preemption = 0;
				}
			};
		} else if (scheduler == Priority) {
			board.pic.isr_0 = [&]() {
				// ToDo: Process the timer interrupt for the Priority Queue
				//  scheduler
                if (processes.size() < 2) {
                    std::cout << std::endl << "Only one process. No scheduling necessary" << std::endl;
                    return;
                }

                std::cout << std::endl << "Processing the timer interrupt" << std::endl;
                ++_cycles_passed_after_preemption;

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
