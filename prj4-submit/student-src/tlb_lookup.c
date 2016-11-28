#include "tlb.h"
#include "pagetable.h"
#include <assert.h>
/**
 * This function simulates a the TLB lookup, and uses the second chance algorithm
 * to evict an entry
 *
 * @param vpn The virtual page number that has to be translated
 * @param offset The page offset of the virtual address
 * @param rw Specifies if the access is a read or a write
 * @param stats The struct for statistics
 */
uint64_t tlb_lookup(uint64_t vpn, uint64_t offset, char rw, stats_t *stats)
{
    
    // (1) Look for the pfn in the TLB.
    // If you find it here
    // (2) update the frequency count of the page table entry using the
    //     current_pagetable global.
    // (3) Mark the TLB entry as used - for clock sweep
    // (4) Make sure to mark the entry dirty in the TLB if it is a write access
    // (5) return the PFN you just found in the TLB
    

    /********* TODO ************/
	stats->accesses = stats->accesses + 1;
	if (rw == READ) {
		stats->reads = stats->reads + 1;
	} else {
		stats->writes = stats->writes + 1;
	}

	for (uint64_t i = 0; i < (1<< tlb_size); i++) {
		//if found in TLB
		if (tlb[i].vpn == vpn && tlb[i].valid == 1) {
			uint64_t foundpfn = tlb[i].pfn;
			tlb[i].used = 1;
			if (rw == WRITE) {
				tlb[i].dirty = 1;
				current_pagetable[vpn].dirty = 1;
			}
			current_pagetable[vpn].frequency = current_pagetable[vpn].frequency + 1;	
			return (foundpfn << page_size) | offset;
		}
	}



    // The below function is called if it is a TLB miss
	/* DO NOT MODIFY */
	uint64_t pfn = page_lookup(vpn, offset, rw, stats);
	/*****************/

    // (1) Update the relevant stats
    // (2) Update the TLB with this new mapping
    //      (a) Find an invalid block in the TLB - use it
    //      If you cannot find an invalid block
    //      (i) Run the clock sweep algorithm to find a victim
    //      (ii) Update the current_pagetable at that VPN to dirty if
    //           the evicted TLB entry is dirty
    //      (b) Put the new mapping into the TLB - mark it used
    
    /**** THESE WILL HELP YOU MAKE SURE THAT YOUR CODE IS CORRECT *****/
    assert(current_pagetable[vpn].valid);
    assert(pfn == current_pagetable[vpn].pfn);
    /********* TODO ************/
	stats->translation_faults = stats->translation_faults + 1; 

	uint64_t index = -1;
	//try to find an invalid block
	for (uint64_t i = 0; i < (1<<tlb_size); i++) {
		if (tlb[i].valid == 0) {
			index = i;
			i = (1<<tlb_size);
		}
	}
	//try to find a victim
	if (index == -1) {
		for (uint64_t i = 0; i < (1<<tlb_size); i++) {
			if (tlb[i].used == 0) {
				index = i;
				i = (1<<tlb_size);
				
			} else {
				tlb[i].used = 0;
			}
		}

		if (index == -1) {
			index = 0;
		}
		if (tlb[index].dirty == 1) {
			current_pagetable[tlb[index].vpn].dirty = 1;
		}
	}


	tlb[index].vpn = vpn;
	tlb[index].pfn = pfn;
	tlb[index].used = 1;
	tlb[index].valid = 1;
	if (rw == WRITE) {
		tlb[index].dirty = 1;	
	} else {
		tlb[index].dirty = 0;
	}



    /******* TODO *************/
    // Make sure to return the entire address here, this is just a place holder
    return (pfn << page_size) | offset;
}
