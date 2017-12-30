#include "memory.h"

namespace svm
{
    Memory::Memory()
        : ram(DEFAULT_RAM_SIZE)
    {
   
		// TODO: initialize data structures for the frame allocator
		for(page_entry_type frame = PAGE_SIZE; frame < DEFAULT_RAM_SIZE; frame += PAGE_SIZE) { 
		
			free_physical_frames.push(frame);
		}
    }

    Memory::~Memory() { }

    Memory::page_table_type* Memory::CreateEmptyPageTable()
    {
    /*
        TODO:

        Return a new page table (for kernel or processes)
        Each entry should be invalid
    */
		Memory::ram_size_type _number_of_pages = DEFAULT_RAM_SIZE/PAGE_SIZE;	
		Memory::page_table_type empty_page_table(_number_of_pages);

		for(Memory::ram_size_type i = 0; i < _number_of_pages; ++i){
			empty_page_table[i] = INVALID_PAGE;
		}	
    
        return &empty_page_table;
    }

    Memory::page_index_offset_pair_type
        Memory::GetPageIndexAndOffsetForVirtualAddress(vmem_size_type virtual_address)
    {
        page_index_offset_pair_type result =
            std::make_pair((page_table_size_type) -1, (ram_size_type) -1);

        /*
            TODO:

             Calculate the page index from the virtual address
             Calculate the offset in the physical memory from the virtual
             address
        */
		result.first = virtual_address / PAGE_SIZE;
		result.second = virtual_address % PAGE_SIZE;
        return result;
    }

    Memory::page_entry_type Memory::AcquireFrame()
    {
        // TODO: find a new free frame (you can use a bitmap or stack)
		if(!free_physical_frames.empty()) {
			page_entry_type result = free_physical_frames.top();
			free_physical_frames.pop();
			return result;
		}

        return INVALID_PAGE;
    }

    void Memory::ReleaseFrame(page_entry_type page)
    {
        // TODO: free the physical frame (you can use a bitmap or stack)
		free_physical_frames.push(page);
    }
}
