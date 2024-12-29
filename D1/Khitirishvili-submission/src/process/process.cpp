/**
 * This file contains implementations for methods in the Process class.
 *
 * You'll need to add code here to make the corresponding tests pass.
 */

#include "process/process.h"

using namespace std;


Process* Process::read_from_input(std::istream& in) {
    // Read process information from the input stream
    // size_t processSize, accesses, faults;
    // double faultRate;
    // char colon, percentSymbol;
    // in >> processSize >> colon >> accesses >> colon >> faults >> colon >> faultRate >> percentSymbol;

    // // Create pages vector with the specified process size
    // std::vector<Page*> pages(processSize);

    // // Create the Process object
    // Process* newProcess = new Process(processSize, pages);
    // newProcess->memory_accesses = accesses;
    // newProcess->page_faults = faults;

    // // Calculate the fault rate for each process and set it
    // newProcess->page_faults = static_cast<size_t>((faultRate / 100) * accesses);

  
    std::vector<Page*> Newpages;
    int count = in.gcount();

    // Read characters until the input stream has characters
    while (! in.eof()) {
        // Read a page from the input stream using Page::read_from_input
        Page* page = Page::read_from_input(in);
        Newpages.push_back(page);
    }

    // Create a new process using the constructor with the number of pages
    Process* newProcess = new Process(count, Newpages);
    return newProcess;

    //gcount()

    //while input stream still has characters
        //call Page::read_from_input(in)
        //add to page* vector
    
    //Create new process using constructor
    //return that new process
}


size_t Process::size() const
{
    // TODO
    size_t totalSize = 0;
    for (auto page: pages){
        totalSize += page->size();
    }

    
    return totalSize;
}


bool Process::is_valid_page(size_t index) const
{
    return (index >= 0 && index < pages.size());
}


size_t Process::get_rss() const
{
     size_t rss = 0;

    for (const auto& row : page_table.rows) {
        if (row.present) {
            rss++;
        }
    }

    return rss;
}


double Process::get_fault_percent() const
{
    // TODO
    if ((double)this->memory_accesses > 0){
        return ((double)this->page_faults / (double)this->memory_accesses) * 100;
    }
    else{
        return 0;
    }
}
