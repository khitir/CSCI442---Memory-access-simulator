/**
 * This file contains implementations for methods in the PhysicalAddress class.
 *
 * You'll need to add code here to make the corresponding tests pass.
 */

#include "physical_address/physical_address.h"

using namespace std;

string PhysicalAddress::to_string() const {
    // 
    return std::bitset<FRAME_BITS>(frame).to_string() + std::bitset<OFFSET_BITS>(offset).to_string();
}


ostream& operator <<(ostream& out, const PhysicalAddress& address) {
    // physical address 0000000000101111 [frame: 0; offset: 47]
        out << address.to_string() << " [frame: " << std::to_string(address.frame) << "; offset: " << std::to_string(address.offset) << "]";

    return out;
}
