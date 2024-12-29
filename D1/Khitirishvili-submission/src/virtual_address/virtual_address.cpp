/**
 * This file contains implementations for methods in the VirtualAddress class.
 *
 * You'll need to add code here to make the corresponding tests pass.
 */

#include "virtual_address/virtual_address.h"

using namespace std;

VirtualAddress VirtualAddress::from_string(int process_id, string address) {
    // page info, int
    // offset info, int
    int page = stoi(address.substr(0,PAGE_BITS),nullptr,2);
    int offset = stoi(address.substr(PAGE_BITS,OFFSET_BITS),nullptr,2);
    return VirtualAddress(process_id, page, offset);
    
}


string VirtualAddress::to_string() const {
    // string of page + offset
    return std::bitset<PAGE_BITS>(page).to_string() + std::bitset<OFFSET_BITS>(offset).to_string();
}


ostream& operator <<(ostream& out, const VirtualAddress& address) {
    // PID 42 @ 1000100011101010 [page: 547; offset: 42]
    out << "PID " << std::to_string(address.process_id) << " @ " << address.to_string() << " [page: " << std::to_string(address.page) << "; offset: " << std::to_string(address.offset) << "]";
    return out;
}
