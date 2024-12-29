/**
 * This file contains implementations for the methods defined in the Simulation
 * class.
 *
 * You'll probably spend a lot of your time here.
 */

#include "simulation/simulation.h"
#include <stdexcept>

Simulation::Simulation(FlagOptions &flags)
{
    this->flags = flags;
    this->frames.reserve(this->NUM_FRAMES);
}

void Simulation::run()
{
    // TODO
    frames.resize(this->NUM_FRAMES);
    for (int i = 0; i < this->NUM_FRAMES; i++)
    {
        this->free_frames.push_back(i);
    }

    for (const auto &address : virtual_addresses)
    {
        char accessed_byte = perform_memory_access(address);
        if (accessed_byte == false)
        {
            return;
        }
        time++;
    }

    print_summary();

    // // cleanup (delete all pages within each process, then delete each process)
    for (auto &entry : processes)
    {
        auto process = entry.second;
        // Delete all pages within the process
        for (auto &page : process->pages)
        {
            delete page;
        }

        delete process;
    }

    // Clear the processes map
    processes.clear();
}

char Simulation::perform_memory_access(const VirtualAddress &virtual_address)
{
    //     perform_memory_access()
    // Check if valid and present
    // Handle page faults if needed
    // Get the actual data

    int pid = virtual_address.process_id;
    size_t page2 = virtual_address.page;

    if (!processes.count(pid))
    {
        std::cout << "Process with PID " << pid << " does not exist!" << std::endl;
        return 0;
    }
    std::cout << virtual_address << "\n";
    Process *process = processes.at(pid);
    if (!process->is_valid_page(page2))
    {
        // segmentatio fault
        computerCrashed = true;
        std::cout << "SEGFAULT - INVALID PAGE" << "\n";
        
        return false;
    }
    
    if (!process->page_table.rows[page2].present)
    {
        std::cout << "\t-> PAGE FAULT" << "\n";
        handle_page_fault(process, page2);
       // process->page_table.rows[page2].loaded_at = time;
        // check if valid offset
        
    }
    else{
        std::cout << "\t-> IN MEMORY" << "\n";
        process->page_table.rows[page2].last_accessed_at = time;
    }

    int frameIndex = process->page_table.rows[page2].frame;
    PhysicalAddress* phys = new PhysicalAddress(frameIndex, virtual_address.offset);
    std::cout << "\t-> physical address " << phys << "\n";

    
        if (frames[frameIndex].contents->is_valid_offset(virtual_address.offset) == false)
        {
         std::cout << "SEGFAULT - INVALID OFFSET" << "\n";

            // segmentation fault
            //time++;
            computerCrashed = false;
            return false;
        }
        std::cout << "\t-> RSS: " << process->get_rss() << "\n";

    process->memory_accesses ++;

    // if (!process->is_valid_page(page2) || !process->page_table.rows[page2].present)
    // {
    //     std::cerr << "Page fault couldn't be handled for PID " << pid << " and page " << page2 << std::endl;
    //     return 0;
    // }
    //std::cout << virtual_address;

    return frames[frameIndex].contents->get_byte_at_offset(virtual_address.offset); //process->pages[page2]->get_byte_at_offset(virtual_address.offset);
}

void Simulation::handle_page_fault(Process *process, size_t page)
{

    
    if (free_frames.empty() || flags.max_frames <= process->get_rss())
    {
        
        size_t frame_to_replace;
        // Perform page replacement (e.g., FIFO, LRU, etc.)
        if(flags.strategy == ReplacementStrategy::FIFO){
            frame_to_replace = process->page_table.get_oldest_page();
        }
        else if (flags.strategy == ReplacementStrategy::LRU){
            frame_to_replace = process->page_table.get_least_recently_used_page();
        }

        process->page_table.rows[frame_to_replace].present = false;
        // Update the frame's contents with the new page.
        frames[process->page_table.rows[frame_to_replace].frame].set_page(process, page);

        // Update the process's page table to mark the page as present in the new frame
        
        process->page_table.rows[page].frame = process->page_table.rows[frame_to_replace].frame;


    }
    else
    {
        // If there are free frames available, load the page into a free frame
        size_t free_frame = free_frames.front();
        free_frames.pop_front(); // Remove the frame from the list of free frames

        // Update the frame's contents with the new page
        frames[free_frame].set_page(process, page);

        // Update the process's page table to mark the page as present in memory
       // process->page_table.rows[page].present = true;
        process->page_table.rows[page].frame = free_frame;
    }
    process->page_table.rows[page].present = true;
    process->page_table.rows[page].loaded_at = time;
    process->page_table.rows[page].last_accessed_at = time;
    process->page_faults++;
    // Increment the page fault count
    ++page_faults;
}

void Simulation::print_summary()
{
    if (!this->flags.csv)
    {
        boost::format process_fmt(
            "Process %3d:  "
            "ACCESSES: %-6lu "
            "FAULTS: %-6lu "
            "FAULT RATE: %-8.2f "
            "RSS: %-6lu\n");

        for (auto entry : this->processes)
        {
            std::cout << process_fmt % entry.first % entry.second->memory_accesses % entry.second->page_faults % entry.second->get_fault_percent() % entry.second->get_rss();
        }

        // Print statistics.
        boost::format summary_fmt(
            "\n%-25s %12lu\n"
            "%-25s %12lu\n"
            "%-25s %12lu\n");

        std::cout << summary_fmt % "Total memory accesses:" % this->virtual_addresses.size() % "Total page faults:" % this->page_faults % "Free frames remaining:" % this->free_frames.size();
    }

    if (this->flags.csv)
    {
        boost::format process_fmt(
            "%d,"
            "%lu,"
            "%lu,"
            "%.2f,"
            "%lu\n");

        for (auto entry : processes)
        {
            std::cout << process_fmt % entry.first % entry.second->memory_accesses % entry.second->page_faults % entry.second->get_fault_percent() % entry.second->get_rss();
        }

        // Print statistics.
        boost::format summary_fmt(
            "%lu,,,,\n"
            "%lu,,,,\n"
            "%lu,,,,\n");

        std::cout << summary_fmt % this->virtual_addresses.size() % this->page_faults % this->free_frames.size();
    }
}

int Simulation::read_processes(std::istream &simulation_file)
{
    int num_processes;
    simulation_file >> num_processes;

    for (int i = 0; i < num_processes; ++i)
    {
        int pid;
        std::string process_image_path;

        simulation_file >> pid >> process_image_path;

        std::ifstream proc_img_file(process_image_path);

        if (!proc_img_file)
        {
            std::cerr << "Unable to read file for PID " << pid << ": " << process_image_path << std::endl;
            return 1;
        }
        this->processes[pid] = Process::read_from_input(proc_img_file);
    }
    return 0;
}

int Simulation::read_addresses(std::istream &simulation_file)
{
    int pid;
    std::string virtual_address;

    try
    {
        while (simulation_file >> pid >> virtual_address)
        {
            this->virtual_addresses.push_back(VirtualAddress::from_string(pid, virtual_address));
        }
    }
    catch (const std::exception &except)
    {
        std::cerr << "Error reading virtual addresses." << std::endl;
        std::cerr << except.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Error reading virtual addresses." << std::endl;
        return 1;
    }
    return 0;
}

int Simulation::read_simulation_file()
{
    std::ifstream simulation_file(this->flags.filename);
    // this->simulation_file.open(this->flags.filename);

    if (!simulation_file)
    {
        std::cerr << "Unable to open file: " << this->flags.filename << std::endl;
        return -1;
    }
    int error = 0;
    error = this->read_processes(simulation_file);

    if (error)
    {
        std::cerr << "Error reading processes. Exit: " << error << std::endl;
        return error;
    }

    error = this->read_addresses(simulation_file);

    if (error)
    {
        std::cerr << "Error reading addresses." << std::endl;
        return error;
    }

    if (this->flags.file_verbose)
    {
        for (auto entry : this->processes)
        {
            std::cout << "Process " << entry.first << ": Size: " << entry.second->size() << std::endl;
        }

        for (auto entry : this->virtual_addresses)
        {
            std::cout << entry << std::endl;
        }
    }

    return 0;
}
