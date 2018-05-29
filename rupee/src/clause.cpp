#include <stdlib.h>
#include <string>

#include "structs.hpp"
#include "extra.hpp"


namespace Clause {

//-------------------------------
// Memory management
//-------------------------------

int it;

bool allocate(clause& c) {
	#ifdef VERBOSE
    Blablabla::log("Allocating clause buffer");
	#endif
	c.max = Stats::variableBound + 1;
	c.used = 0;
	c.array = (int*) malloc(c.max * sizeof(int));
    c.lits = (bool*) malloc((2 * c.max + 1) * sizeof(bool));
    if(c.array == NULL || c.lits == NULL) {
		#ifdef VERBOSE
		Blablabla::log("Error at clause buffer allocation");
		#endif
		Blablabla::comment("Memory management error");
		return false;
	}
    c.lits += c.max;
    for(it = -c.max; it <= c.max; ++it) {
        c.lits[it] = false;
    }
    closeBuffer(c);
	return true;
}

bool reallocate(clause& c) {
	#ifdef VERBOSE
    Blablabla::log("Reallocating clause buffer");
	#endif
    c.lits -= c.max;
    free(c.lits);
    Stats::variableBound *= 2;
    c.max = Stats::variableBound + 1;
    c.array = (int*) realloc(c.array, c.max * sizeof(int));
    c.lits = (bool*) malloc((2 * c.max + 1) * sizeof(bool));
    if(c.array == NULL || c.lits == NULL) {
		#ifdef VERBOSE
		Blablabla::log("Error at clause buffer allocation.");
		#endif
		Blablabla::comment("Memory management error.");
		return false;
	}
    c.lits += c.max;
    for(it = -c.max; it <= c.max; ++it) {
        c.lits[it] = false;
    }
    for(it = 0; it < c.used; ++it) {
        c.lits[c.array[it]] = true;
    }
	return true;
}

void deallocate(clause& c) {
	#ifdef VERBOSE
    Blablabla::log("Deallocating clause buffer");
	#endif
    c.lits -= c.max;
    free(c.lits);
    free(c.array);
}

//-------------------------------
// Buffer manipulation
//-------------------------------

int* ptr;

bool addLiteral(clause& c, int literal) {
    while(abs(literal) > Stats::variableBound) {
        if(!reallocate(c)) { return false; }
    }
    if(!c.lits[-literal]) {
        if(!c.lits[literal]) {
            c.lits[literal] = true;
            c.array[c.used++] = literal;
        }
    } else {
        closeBuffer(c);
        c.taut = true;
    }
    return true;
}

void preventUnit(clause& c) {
    if(c.used == 1) {
        addLiteral(c, -Constants::ReservedLiteral);
    }
}

void closeBuffer(clause& c) {
    c.array[c.used] = Constants::EndOfList;
}

void resetBuffer(clause& c) {
    ptr = c.array;
    while(*ptr != Constants::EndOfList)  {
        c.lits[*(ptr++)] = false;
    }
    c.used = 0;
    c.taut = false;
}

//-------------------------------
// Resolvent computation
//-------------------------------

int rlit;
int* rptr;

void setInnerClause(clause& c, int* pointer, int pivot) {
    for(rptr = pointer; (rlit = *rptr) != Constants::EndOfList; ++rptr) {
        // if(rlit != pivot) {
            c.lits[rlit] = true;
            c.array[c.used++] = rlit;
        // }
    }
    c.inner = c.used;
}

void setOuterClause(clause& c, int* pointer, int pivot) {
    for(rptr = pointer; (rlit = *rptr) != Constants::EndOfList; ++rptr) {
        if(rlit != -pivot && !c.lits[rlit]) {
            if(c.lits[-rlit]) {
                c.taut = true;
                closeBuffer(c);
                return;
            } else {
                c.lits[rlit] = true;
                c.array[c.used++] = rlit;
            }
        }
    }
    closeBuffer(c);
}

void resetToInner(clause& c) {
    rptr = c.array + c.inner;
    while(*rptr != Constants::EndOfList)  {
        c.lits[*(rptr++)] = false;
    }
    c.used = c.inner;
    c.taut = false;
}


//-------------------------------
// Instruction parsing
//-------------------------------

bool done;
int lit;
long offset;

void setInstructionKind(clause& c, bool kind) {
	c.kind = kind;
}

bool processLiteral(clause& c, int literal) {
	lit = stringToLiteral(literal);
	if(lit == Constants::EndOfList) {
		Clause::preventUnit(c);
		Clause::closeBuffer(c);
	} else if(!c.taut) {
		if(!addLiteral(c, lit)) { return false; }
	}
	return true;
}

bool processInstruction(clause& c, hashtable& h, database& d, proof& r, bool file) {
	if(c.taut) {
		#ifdef VERBOSE
		Blablabla::log("Parsed a tautology, skipping");
		#endif
	} else {
	    if(c.kind == Constants::InstructionIntroduction) {
	        lit = *(c.array);
			#ifdef VERBOSE
	        Blablabla::log("Parsed instruction: +++" + Blablabla::clauseToString(c.array));
			#endif
	        if(!Database::insertBufferClause(d, c, h, file, offset)) { return false; }
	    } else {
	        lit = 0;
			#ifdef VERBOSE
	        Blablabla::log("Parsed instruction: ---" + Blablabla::clauseToString(c.array));
			#endif
	        Database::removeBufferClause(d, c, h, offset);
	    }
	    if(offset != Constants::NoOffset) {
	        if(!Proof::storeInstruction(r, offset, lit, c.kind)) { return false; }
	    } else {
			if(!Proof::storeDummy(r, c.kind)) { return false; }
		}
	}
    return true;
}

int stringToLiteral(int literal) {
    if(literal < 0) {
        return literal - 1;
    } else if(literal > 0) {
        return literal + 1;
    } else {
        return 0;
    }
}

//-------------------------------
// Buffer comparators
//-------------------------------

int pos;

void sortClause(clause& c) {
    qsort(c.array, c.used, sizeof(int), compare);
}

bool equals(clause& c, int* ptr) {
    for(pos = 0; pos < c.used; ++pos) {
        if(ptr[pos] != c.array[pos]) {       // This check within pos < c.used subsumes ptr[pos] != 0,
            return false;                       // and so it prevents segmentation fault.
        }
    }
    return (ptr[pos] == 0);                      // At this point we know c.used[pos] == 0 holds.
}

int compare(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

}
