#include <stdlib.h>

#include "structs.hpp"
#include "extra.hpp"

namespace Revision {

//----------------------
// Memory management
//----------------------

int it;

bool allocate(revision& v) {
    #ifdef VERBOSE
    Blablabla::log("Allocating stack bookkeeper");
    #endif
    v.cone = (int*) malloc ((Stats::variableBound + 1) * sizeof(int));
    v.lits = (bool*) malloc ((2 * Stats::variableBound + 1) * sizeof(bool));
    v.max = Parameters::databaseSize;
    v.used = 0;
    v.array = (long*) malloc (v.max * sizeof(long));
    v.current = v.cone;
    if(v.cone == NULL || v.lits == NULL || v.array == NULL) {
        #ifdef VERBOSE
        Blablabla::log("Error at stack bookkeeper allocation.");
        #endif
		Blablabla::comment("Memory management error.");
        return false;
    }
    v.lits += Stats::variableBound;
    for(it = -Stats::variableBound; it <= Stats::variableBound; ++it) {
        v.lits[it] = false;
    }
    return true;
}

bool reallocate(revision& v) {
    #ifdef VERBOSE
    Blablabla::log("Reallocating stack bookkeeper");
    #endif
    v.max *= 2;
    v.array = (long*) realloc (v.array, v.max * sizeof(long));
    if(v.array == NULL) {
        #ifdef VERBOSE
        Blablabla::log("Error at stack bookkeeper allocation.");
        #endif
		Blablabla::comment("Memory management error.");
        return false;
    }
    return true;
}

void deallocate(revision& v) {
    #ifdef VERBOSE
    Blablabla::log("Deallocating stack bookkeeper");
    #endif
    v.lits -= Stats::variableBound;
    free(v.cone);
    free(v.lits);
    free(v.array);
}

//----------------------
// Cone construction
//----------------------

int lit;
int cllit;
int* leftptr;
int* rightptr;
long count;
int* clause;

void addToCone(revision& v, int literal) {
    v.lits[literal] = true;
    *(v.current++) = literal;
}

bool addToRevision(revision& v, model& m, database& d, int literal, int* position) {
    if(v.max <= v.used + 5) {
        if(!reallocate(v)) { return false; }
    }
    addToCone(v, literal);
    Model::unassignLiteral(m, literal, d);
    v.array[v.used++] = (long) (position - m.array);
    v.array[v.used++] = (long) literal;
    v.array[v.used++] = m.reason[literal];
    return true;
}

bool isInCone(revision& v, long offset, int literal, database& d) {
    clause = Database::getPointer(d, offset);
    while((cllit = *clause) != Constants::EndOfList) {
        if(cllit != literal && v.lits[-cllit]) {
            return true;
        }
        ++clause;
    }
    return false;
}

bool getCone(revision& v, model& m, database& d, int* ptr) {
    #ifdef VERBOSE
    Blablabla::increase();
    Blablabla::logReasons(m, d);
    #endif
    while((lit = *ptr) != Constants::ConflictLiteral && !Model::isSatisfied(m, lit)) {
        ++ptr;
    }
    leftptr = m.position[lit];
    if(!addToRevision(v, m, d, lit, leftptr)) { return false; }
    count = 1;
    rightptr = leftptr + 1;
    while(rightptr < m.used) {
        lit = *rightptr;
        if(isInCone(v, m.reason[lit], lit, d)) {
            if(!addToRevision(v, m, d, lit, rightptr)) { return false; }
            ++count;
        } else {
            m.position[lit] = leftptr;
            *(leftptr++) = *rightptr;
        }
        ++rightptr;
    }
    m.head = m.forced = m.used = leftptr;
    v.array[v.used++] = count;
    #ifdef VERBOSE
    Blablabla::logCone(v);
    Blablabla::logModel(m);
    Blablabla::decrease();
    #endif
    return true;
}

bool voidRevision(revision& v) {
    if(v.max <= v.used) {
        if(!reallocate(v)) { return false; }
    }
    v.array[v.used++] = 0;
    return true;
}

//----------------------
// Cone revision
//----------------------

int* coneit;

bool reviseCone(revision& v, watchlist& wl, model& m, database& d) {
    #ifdef VERBOSE
    Blablabla::log("Revising cone literals");
    Blablabla::increase();
    #endif
    coneit = v.cone;
    while(coneit < v.current) {
        if(!WatchList::reviseList(wl, m, d, *coneit)) { return false; }
        v.lits[*coneit] = false;
        ++coneit;
    }
    #ifdef VERBOSE
    Blablabla::decrease();
    #endif
    return true;
}

bool checkCone(revision& v, model& m) {
    for(coneit = v.cone; coneit < v.current; ++coneit) {
        if(!Model::isSatisfied(m, *coneit)) {
            return false;
        }
    }
    resetCone(v);
    return true;
}

void resetCone(revision& v) {
    v.current = v.cone;
}

//----------------------
// Model revision
//----------------------

int size;
int current;
int leftpos;
int rightpos;
int revlit;
int intcount;

bool applyRevision(revision& v, model& m, database& d) {
    #ifdef VERBOSE
    Blablabla::log("Restoring model");
    Blablabla::increase();
    Blablabla::logModel(m);
    Blablabla::logRevision(v, d);
    #endif
    size = (int) v.array[v.used - 1];
    if(size == 0) {
        --v.used;
        #ifdef VERBOSE
        Blablabla::log("No restoration is required");
        Blablabla::decrease();
        #endif
        return false;
    } else {
        Stats::startDeleteTime();
        v.used -= 3 * size + 1;
        leftpos = 0;
        intcount = 0;
        while(leftpos < size) {
            revlit = (int) v.array[v.used + 3 * (leftpos++) + 1];
            addToCone(v, revlit);
            Database::setFlag(Database::getPointer(d, m.reason[revlit]), Constants::PseudounitBit, Constants::PassiveFlag);
            if(!Model::isSatisfied(m, revlit)) {
                intcount++;
            }
        }
        rightpos = (m.used - m.array) + intcount - 1;
        leftpos = rightpos - size;
        m.head = m.used = m.forced = m.array + rightpos + 1;
        current = v.array[v.used + 3 * size - 3];
        while(size > 0) {
            if(rightpos == current) {
                revlit = (int) v.array[v.used + 3 * size - 2];
                m.array[rightpos] = revlit;
                m.reason[revlit] = v.array[v.used + 3 * size - 1];
                m.lits[revlit] = true;
                Database::setFlag(Database::getPointer(d, m.reason[revlit]), Constants::PseudounitBit, Constants::ReasonFlag);
                if(--size > 0) {
                    current = v.array[v.used + 3 * size - 3];
                }
            } else {
                revlit = (int) m.array[leftpos--];
                m.array[rightpos] = revlit;
            }
            m.position[revlit] = m.array + (rightpos--);
        }
        #ifdef VERBOSE
        Blablabla::logModel(m);
        Blablabla::decrease();
        #endif
        return true;
    }
}

bool resetWatches(revision& v, watchlist& wl, model& m, database& d) {
    #ifdef VERBOSE
    Blablabla::log("Resetting watchlists");
    Blablabla::increase();
    #endif
    while(v.current > v.cone) {
        revlit = *(--v.current);
        v.lits[revlit] = false;
        if(!WatchList::resetList(wl, m, d, -revlit)) { return false; }
        if(!WatchList::resetList(wl, m, d, revlit)) { return false; }
    }
    // for(revlit = -Stats::variableBound; revlit <= Stats::variableBound; ++revlit) {
    //     if(wl.used[revlit] != 0) {
    //         if(!WatchList::resetList(wl, m, d, revlit)) { return false; }
    //     }
    // }
    Stats::stopDeleteTime();
    #ifdef VERBOSE
    Blablabla::decrease();
    #endif
    return true;
}

}
