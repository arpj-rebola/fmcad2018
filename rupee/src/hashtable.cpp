#include <stdlib.h>

#include "structs.hpp"
#include "extra.hpp"

namespace HashTable {
//-------------------------------
// Memory management
//-------------------------------

bool error;
int it;

bool allocate(hashtable& h) {
    error = false;
    #ifdef VERBOSE
	Blablabla::log("Allocating hash table.");
    #endif
	h.array = (long**) malloc (Parameters::databaseSize * sizeof (long*));
	h.used = (int*) malloc (Parameters::databaseSize * sizeof (int));
	h.max = (int*) malloc (Parameters::databaseSize * sizeof (int));
	if(h.array == NULL || h.used == NULL || h.max == NULL) {
		error = true;
	}
	for(it = 0; it < Parameters::databaseSize && !error; ++it) {
		h.array[it] = (long*) malloc (Parameters::hashDepth * sizeof (long));
		h.max[it] = Parameters::hashDepth;
		h.used[it] = 0;
		if(h.array[it] == NULL) {
			error = true;
		}
	}
	if(error) {
        #ifdef VERBOSE
		Blablabla::log("Error at hash table allocation.");
        #endif
		Blablabla::comment("Memory management error.");
		return false;
	} else {
		return true;
	}
}

bool reallocateRow(hashtable& h, int row) {
    #ifdef VERBOSE
    Blablabla::log("Reallocating hash table.");
    #endif
    h.max[row] *= 2;
    h.array[row] = (long*) realloc(h.array[row], h.max[row] * sizeof(long));
    if(h.array[row] == NULL) {
        #ifdef VERBOSE
        Blablabla::log("Error at hash table reallocation.");
        #endif
        Blablabla::comment("Memory management error.");
        return false;
    } else {
        return true;
    }
}

void deallocate(hashtable& h) {
    #ifdef VERBOSE
    Blablabla::log("Deallocating hash table.");
    #endif
	for(it = 0; it < Parameters::databaseSize; ++it) {
		free(h.array[it]);
	}
	free(h.array);
	free(h.used);
	free(h.max);
}

//-------------------------------
// Hash computation
//-------------------------------

unsigned int sum;
unsigned int prod;
unsigned int xoor;

unsigned int getHash(int* ptr) {
    sum = 0;
    prod = 1;
    xoor = 0;
    while(*ptr) {
        prod *= *ptr;
        sum += *ptr;
        xoor ^= *ptr;
        ++ptr;
    }
    return (((1023 * sum + prod) ^ (31 * xoor))) % Parameters::databaseSize;
}

//-------------------------------
// Hash table lookup
//-------------------------------

long* list;
int length;
int* pointer;

bool match(hashtable& h, database& d, clause& c, unsigned int hash, long& offset, int& pos) {
    list = h.array[hash];
    length = h.used[hash];
    for(pos = 0; pos < length; ++pos) {
        pointer = Database::getPointer(d, list[pos]);
        if(Clause::equals(c, pointer)) {
            offset = list[pos];
            return true;
        }
    }
    return false;
}

//-------------------------------
// Hash table modification
//-------------------------------

bool insertOffset(hashtable& h, unsigned int hash, long offset) {
    if(h.used[hash] >= h.max[hash]) {
        if(!reallocateRow(h, hash)) { return false; }
    }
    h.array[hash][h.used[hash]++] = offset;
    return true;
}

void removeOffset(hashtable& h, unsigned int hash, int pos) {
    h.array[hash][pos] = h.array[hash][--(h.used[hash])];
}

}
