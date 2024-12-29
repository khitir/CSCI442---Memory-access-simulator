/**
 * This file contains implementations for methods in the PageTable class.
 *
 * You'll need to add code here to make the corresponding tests pass.
 */

#include "page_table/page_table.h"

using namespace std;


size_t PageTable::get_present_page_count() const {
    size_t num_present = 0;
    for(auto row: rows){
        if (row.present){
            num_present++;
        }
    }
    return num_present;
}


size_t PageTable::get_oldest_page() const {
    size_t oldestPageIndex = 0; // Initialize with the first page index
    size_t oldestTimestamp = -1; // Initialize with the first page's timestamp

    for (size_t i = 0; i < rows.size(); ++i) {
        if (rows[i].present && (rows[i].loaded_at < oldestTimestamp || oldestTimestamp == -1)) {
            oldestTimestamp = rows[i].loaded_at;
            oldestPageIndex = i;
        }
    }
    return oldestPageIndex;
}



size_t PageTable::get_least_recently_used_page() const {
    size_t lruPageIndex = 0; // Initialize with the first page index
    size_t lruTimestamp = -1; // Initialize with the first page's timestamp

    for (size_t i = 0; i < rows.size(); ++i) {
        if (rows[i].present && (rows[i].last_accessed_at < lruTimestamp || lruTimestamp == -1)) {
            lruTimestamp = rows[i].last_accessed_at;
            lruPageIndex = i;
        }
    }
    return lruPageIndex;
}
