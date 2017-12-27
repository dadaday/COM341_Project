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

        // Memory Management

        /*
         *   TODO:
         *
         *     Initialize data structures for methods `AllocateMemory` and
         *       `FreeMemory`
         */

        // Process page faults (find empty frames)
        board.pic.isr_4 = [&]() {
            std::cout << "Kernel: page fault." << std::endl;

            /*
             *  TODO:
             *
             *    Get the faulting page index from the register 'a'
             *
             *    Try to acquire a new frame from the MMU by calling
             *      `AcquireFrame`
             *    Check if the frame is valid
             *        If valid
             *            Write the frame to the current faulting page in the
             *              MMU page table (at index from register 'a')
             *            or else if invalid
             *              Notify the process or stop the board (out of
             *              physical memory)
             */
        };

        // Process Management

        std::for_each(
            executables_paths.begin(),
            executables_paths.end(),
            [&](Memory::ram_type &executable) {
                CreateProcess(executable);
            }
        );

        /*
         *  TODO:
         *
         *    Switch to the first process on the CPU
         *    Switch the page table in the MMU to the table of the current
         *      process
         *    Set a proper state for the first process
         */

        /*
         *  TODO:
         *
         *    Each scheduler should get the page table switching step
         *    Each exit call handler in `isr_3` should start using `FreeMemory`
         *      to release data in RAM or virtual memory
         */

        if (scheduler == FirstComeFirstServed) {
            board.pic.isr_0 = [&]() {
                // ToDo: Process the timer interrupt for the FCFS
                std::cout << std::endl << "Nothing to do in timer interrupt" << std::endl;
                std::cout << "Allowing the current process " << processes[_current_process_index].id << " to run" << std::endl;
            };

            board.pic.isr_3 = [&]() {
                // ToDo: Process the first software interrupt for the FCFS
                // Unload the current process

                std::cout << std::endl << "Processing the first software interrupt" << std::endl;
                std::cout << "Unloading the process " << processes[_current_process_index].id << std::endl;
                processes.erase(processes.begin() + _current_process_index);

                if (processes.empty()) {
                    std::cout << "No more processes. Stopping the machine" << std::endl;
                    board.Stop();
                }
                else {
                    std::cout << "Switching the context to the next process " << processes[_current_process_index].id << std::endl;
                    board.cpu.registers = processes[_current_process_index].registers;
                    processes[_current_process_index].state = Process::States::Running;
                    _cycles_passed_after_preemption = 0;
                }

            };
        } else if (scheduler == ShortestJob) {
            board.pic.isr_0 = [&]() {
                // ToDo: Process the timer interrupt for the Shortest
                //  Job scheduler
                std::cout << std::endl << "Nothing to do in timer interrupt" << std::endl;
                std::cout << "Allowing the current process " << processes[_current_process_index].id << " to run" << std::endl;
            };

            board.pic.isr_3 = [&]() {
                // ToDo: Process the first software interrupt for the Shortest
                //  Job scheduler
                std::cout << std::endl << "Processing the first software interrupt" << std::endl;

                // Unload the current process
                std::cout << "Unloading the process " << processes[_current_process_index].id << std::endl;
                processes.erase(processes.begin() + _current_process_index);

                if (processes.empty()) {
                    std::cout << "No more processes. Stopping the machine" << std::endl;
                    board.Stop();
                }
                else {
                    std::cout << "Switching the context to the next shortest process " << processes[_current_process_index].id << std::endl;
                    board.cpu.registers = processes[_current_process_index].registers;
                    processes[_current_process_index].state = Process::States::Running;
                    _cycles_passed_after_preemption = 0;
                }

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
                    _cycles_passed_after_preemption = 1; //set to 1 to count this as the cycle of the process to which we switched
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

                if (priorities.size() < 2) {
                    std::cout << std::endl << "Only one process. No scheduling necessary" << std::endl;
                    return;
                }

                std::cout << std::endl << "Processing the timer interrupt" << std::endl;
                std::cout << "Priority of the current process " << priorities.top().id << ": " 
                << priorities.top().priority << std::endl;

                ++_cycles_passed_after_preemption;

                if (_cycles_passed_after_preemption > _MAX_CYCLES_BEFORE_PREEMPTION) {
                    Process temp = priorities.top();
                    std::cout << "Lowering the priority for process " << temp.id << std::endl;
                    priorities.pop();

                    if (temp.priority > 0) {
                        temp.priority--;
                    }
                    else
                        temp.priority = 9;

                    temp.registers = board.cpu.registers;
                    temp.state = Process::States::Ready;
                    if (temp.priority > priorities.top().priority) {
                        std::cout << "Current process " << temp.id << " still has the highest priority" << std::endl;
                        priorities.push(temp);
                    }
                    else {
                        priorities.push(temp);
                        Process t = priorities.top();
                        std::cout << "saving all registers from the CPU to the PCB of the previous process" << std::endl;
                        board.cpu.registers = t.registers;
                        std::cout<< "Restoring all the registers from the PCB to CPU of the process next in priority" << std::endl;
                        t.state = Process::States::Running;
                    }
                    _cycles_passed_after_preemption = 1;
                }

            };

            board.pic.isr_3 = [&]() {
                // ToDo: Process the first software interrupt for the Priority
                //  Queue scheduler
                std::cout << std::endl << "Processing the first software interrupt" << std::endl;
                // Unload the current process
                std::cout << "Unloading the current process " << priorities.top().id <<
                            " with priority " << priorities.top().priority << std::endl;
                priorities.pop();

                if (priorities.empty()) {
                    board.Stop();
                }
                else {
                    Process t = priorities.top();
                    board.cpu.registers = t.registers;
                    priorities.pop();
                    t.state = Process::States::Running;
                    priorities.push(t);
                }
            };
        }

        board.Start();
    }

    Kernel::~Kernel() { }

    void Kernel::CreateProcess(Memory::ram_type &executable)
    {
        Memory::ram_size_type
            new_memory_position = -1; // TODO:
                                      //   allocate memory for the process
                                      //   with `AllocateMemory`
        if (new_memory_position == -1) {
            std::cerr << "Kernel: failed to allocate memory."
                      << std::endl;
        } else {
            // Assume that the executable image size can not be greater than
            //   a page size
            std::copy(
                executable.begin(),
                executable.end(),
                board.memory.ram.begin() + new_memory_position
            );

            Process process(
                _last_issued_process_id++,
                new_memory_position,
                new_memory_position + executable.size()
            );

            // Old sequential allocation
            //
            //   std::copy(
            //       executable.begin(),
            //       executable.end(),
            //       board.memory.ram.begin() + _last_ram_position
            //   );
            //
            //   Process process(
            //       _last_issued_process_id++,
            //       _last_ram_position,
            //       _last_ram_position + executable.size()
            //   );
            //
            //   _last_ram_position +=
            //       executable.size();

            // ToDo: add the new process to an appropriate data structure
            if (scheduler == ShortestJob) {
                processes.push_back(process);
                std::sort(processes.begin(), processes.end(), [](const Process &a, const Process &b) {
                    return a.sequential_instruction_count < b.sequential_instruction_count;
                });
            }
            else if (scheduler == Priority) {
                Process::process_priority_type prio = rand() % 10;
                process.priority = prio;
                std::cout << "SET Priority to " << prio << std::endl;
                priorities.push(process);
                // to avoid messing with svm.cpp code in order to get additional
                // info about the files priorities
                // the priority scheduler for this task 
                // will be simplified in following manner:
                // the priority will be some random number (0-9)
                // higher numbers have higher priority
            }
            else {
                processes.push_back(process);
            }

                // ToDo: process the data structure (e.g., sort if necessary)

                // ---
        }
    }

    Memory::ram_size_type Kernel::AllocateMemory(
                                      Memory::ram_size_type units
                                  )
    {
        /*
         *  TODO:
         *
         *    Task 1: allocate physical memory by using a free list with the
         *      next fit approach.
         *
         *    You can adapt the algorithm from the book The C Programming
         *      Language (Second Edition) by Brian W. Kernighan and Dennis M.
         *      Ritchie (8.7 Example - A Storage Allocator).
         *
         *    Task 2: adapt the algorithm to work with your virtual memory
         *      subsystem.
         */

        return -1;
    }

    void Kernel::FreeMemory(
                     Memory::ram_size_type physical_address
                 )
    {
        /*
         *  TODO:
         *
         *    Task 1: free physical memory
         *
         *    You can adapt the algorithm from the book The C Programming
         *      Language (Second Edition) by Brian W. Kernighan and Dennis M.
         *      Ritchie (8.7 Example - A Storage Allocator).
         *
         *    Task 2: adapt the algorithm to work with your virtual memory
         *      subsystem
         */
    }
}
