/**
 * This file contains implementations for methods in the Frame class.
 *
 * You'll need to add code here to make the corresponding tests pass.
 */

#include "frame/frame.h"

using namespace std;

void Frame::set_page(Process* process, size_t page_number) {
    // Update frame with the new page and process
    this->page_number = page_number;
    this->process = process;

    // Set the frame's contents to the specified page in the process
    if (process->is_valid_page(page_number)) {
        this->contents = process->pages[page_number];
    }
}
