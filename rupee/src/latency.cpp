#include <stdlib.h>

#include "structs.hpp"
#include "extra.hpp"


namespace Latency {

//-------------------------------
// Memory management
//-------------------------------

bool allocate(latency& lt) {
    #ifdef VERBOSE
    Blablabla::log("Allocating latency array");
    #endif
    lt.max = Parameters::hashDepth;
    lt.used = 0;
    lt.array = (long*) malloc(lt.max * sizeof(long));
    if(lt.array == NULL) {
        #ifdef VERBOSE
        Blablabla::log("Error at latency array allocation");
        #endif
        Blablabla::comment("Memory management error");
        return false;
    }
    return true;
}

bool reallocate(latency& lt) {
    #ifdef VERBOSE
    Blablabla::log("Reallocating latency array");
    #endif
    lt.max *= 2;
    lt.array = (long*) realloc(lt.array, lt.max * sizeof(long));
    if(lt.array == NULL) {
        #ifdef VERBOSE
        Blablabla::log("Error at latency array allocation");
        #endif
        Blablabla::comment("Memory management error");
        return false;
    }
    return true;
}

void deallocate(latency& lt) {
    #ifdef VERBOSE
    Blablabla::log("Deallocating latency array");
    #endif
    free(lt.array);
}

//-------------------------------
// Resolution candidates collection
//-------------------------------

int litit;

bool collectCandidates(latency& lt, watchlist& wl, database& d, int pivot) {
    resetCandidates(lt);
    for(litit = -Stats::variableBound; litit <= Stats::variableBound; ++litit) {
        if(!WatchList::collectList(wl, lt, d, -pivot, litit)) { return false; }
    }
    #ifdef VERBOSE
    Blablabla::logLatency(lt, d);
    #endif
    return true;
}

void resetCandidates(latency& lt) {
    lt.used = 0;
}

bool addCandidate(latency& lt, long offset) {
    if(lt.used >= lt.max) {
        if(!reallocate(lt)) { return false; }
    }
    lt.array[lt.used++] = offset;
    return true;
}

//-------------------------------
// RAT check
//-------------------------------

int it;
bool check;
long off;
int* ptr;

bool checkResolventsRup(latency& lt, checker& c, clause& cl, database& d, int pivot, bool& result) {
    for(it = 0; it < lt.used; ++it) {
        off = lt.array[it];
        ptr = Database::getPointer(d, off);
        #ifdef VERBOSE
        Blablabla::log("Checking RUP on resolvent with " + Blablabla::clauseToString(ptr));
        #endif
        Clause::setOuterClause(cl, ptr, pivot);
        if(!Witness::setResolventWitness(c.tvr, off)) { return false; }
        if(!cl.taut) {
            #ifdef VERBOSE
            Blablabla::log("Resolvent: " + Blablabla::clauseToString(cl.array));
            #endif
            check = Constants::CheckIncorrect;
            Checker::precheckRup(c, d, cl.array, check);
            if(!Checker::checkRup(c, d, check, c.kk.reslits, -pivot, off)) { return false; }
            if(check == Constants::CheckCorrect) {
                if(!Witness::dumpChain(c.tvr, c.stack, d)) { return false; }
            } else {
                result = Constants::CheckIncorrect;
                c.kk.clause = c.offset;
                c.kk.resolvent = off;
                c.kk.pivot = pivot;
                Clause::closeBuffer(cl);
                Clause::resetBuffer(cl);
                return true;
            }
        } else {
            #ifdef VERBOSE
            Blablabla::log("Resolvent is a tautology");
            #endif
        }
        Clause::resetToInner(cl);
    }
    result = Constants::CheckCorrect;
    Clause::closeBuffer(cl);
    Clause::resetBuffer(cl);
    return true;
}

}
