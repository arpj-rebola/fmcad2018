#include <stdlib.h>
#include <string>

#include "structs.hpp"
#include "extra.hpp"


namespace Model {

//----------------------
// Memory management
//----------------------

int it;

// Allocates model <m> and initializes it with: || [ Constants::ReservedLiteral ]
bool allocate(model& m) {
    #ifdef VERBOSE
    Blablabla::log("Allocating model.");
    #endif
    m.array = (int*) malloc((2 * Stats::variableBound + 1) * sizeof(int));
    m.lits = (bool*) malloc((2 * Stats::variableBound + 1) * sizeof(bool));
    m.reason = (long*) malloc((2 * Stats::variableBound + 1) * sizeof(long));
    m.position = (int**) malloc((2 * Stats::variableBound + 1) * sizeof(int*));
    if(m.array == NULL || m.lits == NULL || m.reason == NULL || m.position == NULL) {
        #ifdef VERBOSE
        Blablabla::log("Error at model allocation.");
        #endif
		Blablabla::comment("Memory management error.");
        return false;
    }
    m.lits += Stats::variableBound;
    m.position += Stats::variableBound;
    m.reason += Stats::variableBound;
    for(it = 0; it <= Stats::variableBound; ++it) {
        m.lits[it] = m.lits[-it] = false;
        m.position[it] = m.position[-it] = NULL;
    }
    m.used = m.array;
    m.lits[Constants::ReservedLiteral] = true;
    m.reason[Constants::ReservedLiteral] = Constants::AssumedReason;
    m.position[Constants::ReservedLiteral] = m.used;
    *(m.used++) = Constants::ReservedLiteral;
    m.head = m.forced = m.used;
    return true;
}

// Deallocates model <m>
void deallocate(model& m) {
    #ifdef VERBOSE
    Blablabla::log("Deallocating model.");
    #endif
    m.lits -= Stats::variableBound;
    m.position -= Stats::variableBound;
    m.reason -= Stats::variableBound;
    free(m.array);
    free(m.lits);
    free(m.reason);
    free(m.position);
}

//----------------------
// Literal insertion/deletion
//----------------------

bool assumeLiteral(model& m, int literal) {
    if(isFalsified(m, literal)) { return false; }
    if(!isSatisfied(m, literal)) {
        propagateLiteral(m, literal, Constants::AssumedReason, NULL, Constants::SoftPropagation);
    }
    return true;
}

// Inserts literal <literal> at the end of the used array with reason offset <reason>.
// <reason> must be the offset of <pointer> wrt the relevant database.
void propagateLiteral(model& m, int literal, long reason, int* pointer, bool hard) {
    m.lits[literal] = true;
    m.reason[literal] = reason;
    m.position[literal] = m.used;
    *(m.used++) = literal;
    if(hard == Constants::HardPropagation) {
        Database::setFlag(pointer, Constants::PseudounitBit, Constants::ReasonFlag);
        m.forced = m.used;
    }
    #ifdef VERBOSE
    Blablabla::log("Propagating literal " + Blablabla::litToString(literal));
    Blablabla::logModel(m);
    #endif
}

void unassignLiteral(model& m, int literal, database& d) {
    m.lits[literal] = false;
    Database::setFlag(Database::getPointer(d, m.reason[literal]), Constants::PseudounitBit, Constants::PassiveFlag);
}

//----------------------
// Unit propagation
//----------------------

int* itptr;
int lit;
bool found;

bool propagateModel(model& m, watchlist& wl, database& d, bool hard) {
    #ifdef VERBOSE
    Blablabla::log("Propagating model");
    Blablabla::increase();
    #endif
    while(m.head < m.used && !Model::isContradicted(m)) {
        if(!WatchList::alignList(wl, m, d, -*(m.head), hard)) { return false; }
        ++m.head;
        #ifdef VERBOSE
        Blablabla::logModel(m);
        #endif

    }
    #ifdef VERBOSE
    Blablabla::decrease();
    #endif
    return true;
}

void unpropagateModel(model& m, database& d, int* pointer) {
    if(Database::isFlag(pointer, Constants::PseudounitBit, Constants::ReasonFlag)) {
        itptr = pointer;
        while(!Model::isSatisfied(m, *itptr)) {
            ++itptr;
        }
        lit = *itptr;
        itptr = m.position[lit];
        while(m.used > itptr) {
            unassignLiteral(m, *(--m.used), d);
        }
        m.forced = m.head = m.used;
    }
}

void resetModel(model& m) {
    while(m.used > m.forced) {
        m.lits[*(--m.used)] = false;
    }
    m.head = m.used;
}

//----------------------
// Literal tests
//----------------------

bool isSatisfied(model& m, int literal) {
    return m.lits[literal];
}

bool isFalsified(model& m, int literal) {
    return m.lits[-literal];
}

bool isUnassigned(model& m, int literal) {
    return !m.lits[literal] && !m.lits[-literal];
}

bool isContradicted(model& m) {
    return m.lits[Constants::ConflictLiteral];
}

bool isUnit(model& m, int* pointer) {
    found = false;
    while((lit = *pointer++) != 0) {
        if(!m.lits[-lit]) {
            if(found) {
                return false;
            } else {
                found = true;
            }
        }
    }
    return found;
}

}
