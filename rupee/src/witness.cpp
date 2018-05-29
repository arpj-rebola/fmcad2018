#include <stdlib.h>
#include "structs.hpp"
#include "extra.hpp"

namespace Witness {

//-------------------------------
// Memory management
//-------------------------------

int it;

bool allocate(witness& wt) {
    #ifdef VERBOSE
    Blablabla::log("Allocating witness");
    #endif
    wt.intmax = wt.delmax = Parameters::databaseSize / 2;
    wt.intused = wt.delused = 0;
    wt.intarray = (long*) malloc(wt.intmax * sizeof(long));
    wt.delarray = (long*) malloc(wt.delmax * sizeof(long));
    wt.lits = (bool*) malloc((2 * Stats::variableBound + 1) * sizeof(bool));
    wt.chain = (int*) malloc((2 * Stats::variableBound + 1) * sizeof(int));
    wt.head = wt.last = wt.chain;
    if(wt.intarray == NULL || wt.delarray == NULL || wt.lits == NULL || wt.chain == NULL) {
        #ifdef VERBOSE
        Blablabla::log("Error at witness allocation");
        #endif
        Blablabla::comment("Memory management error");
        return false;
    }
    wt.lits += Stats::variableBound;
    for(it = -Stats::variableBound; it <= Stats::variableBound; ++it) {
        wt.lits[it] = false;
    }
    closeWitness(wt);
    return true;
}

bool reallocate(witness& wt, bool array) {
    #ifdef VERBOSE
    Blablabla::log("Reallocating witness");
    #endif
    if(array == Constants::InstructionDeletion) {
        wt.delmax *= 2;
        wt.delarray = (long*) realloc(wt.delarray, wt.delmax * sizeof(long));
    } else {
        wt.intmax *= 2;
        wt.intarray = (long*) realloc(wt.intarray, wt.intmax * sizeof(long));
    }
    if(wt.delarray == NULL || wt.intarray == NULL) {
        #ifdef VERBOSE
        Blablabla::log("Error at witness allocation");
        #endif
        Blablabla::comment("Memory management error");
        return false;
    }
    return true;
}

void deallocate(witness& wt) {
    #ifdef VERBOSE
    Blablabla::log("Deallocating witness");
    #endif
    wt.lits -= Stats::variableBound;
    free(wt.delarray);
    free(wt.intarray);
    free(wt.lits);
    free(wt.chain);
}

//-------------------------------
// Witness construction
//-------------------------------

long off;
int* cls;

bool insertIntroduction(witness& wt, long value) {
    if(wt.intused + 1 >= wt.intmax) {
        if(!reallocate(wt, Constants::InstructionIntroduction)) { return false; }
    }
    wt.intarray[wt.intused++] = value;
    return true;
}

bool insertDeletion(witness& wt, long value) {
    if(wt.delused + 1 >= wt.delmax) {
        if(!reallocate(wt, Constants::InstructionDeletion)) { return false; }
    }
    wt.delarray[wt.delused++] = value;
    return true;
}

bool openWitness(witness& wt, long offset) {
    return insertIntroduction(wt, offset);
}

bool setRupWitness(witness& wt) {
    return insertIntroduction(wt, (long) Constants::ReservedLiteral);
}

bool setPremiseWitness(witness& wt) {
    return insertIntroduction(wt, (long) -Constants::ReservedLiteral);
}

bool setRatWitness(witness& wt, int literal) {
    return insertIntroduction(wt, (long) literal);
}

bool setResolventWitness(witness& wt, long offset) {
    return insertIntroduction(wt, -offset);
}

bool dumpChain(witness& wt, model& m, database& d) {
    wt.head = wt.last;
    while(wt.chain <= --wt.head) {
        off = m.reason[*wt.head];
        if(!insertIntroduction(wt, off)) { return false; }
        cls = Database::getPointer(d, off);
        if(Database::isFlag(cls, Constants::VerificationBit, Constants::SkipFlag)) {
            Database::setFlag(cls, Constants::VerificationBit, Constants::ScheduledFlag);
            insertDeletion(wt, off);
        }
    }
    return true;
}

bool closeWitness(witness& wt) {
    return (insertIntroduction(wt, Constants::EndOfList) && insertDeletion(wt, Constants::EndOfList));
}

//-------------------------------
// Witness extraction
//-------------------------------

int nopremises;
long* intptr;
long* delptr;
long* wtptr;
long wtoff;
long clsoff;
int* clsptr;
int clspivot;
bool deletepremises;
std::ofstream output;

void extractWitness(witness& wt, database& d) {
    if(Parameters::generateLrat) {
        nopremises = d.idCount;
        intptr = wt.intarray + (wt.intused - 1);
        delptr = wt.delarray + (wt.delused - 1);
        deletepremises = false;
        output.open(Parameters::pathWitness, std::ofstream::out | std::ofstream::trunc);
        while(stepBack(wt, intptr, delptr)) {
            clsoff = intptr[1];
            clsptr = Database::getPointer(d, clsoff);
            clspivot = (int) intptr[2];
            if(clspivot == -Constants::ReservedLiteral) {
                extractPremise(d, clsptr, deletepremises, output);
            } else {
                if(deletepremises) {
                    output << "0" << std::endl;
                    deletepremises = false;
                }
                wtptr = intptr + 3;
                if(Parameters::lratRmark) {
                    output << "r ";
                }
                clsptr[Constants::IdCellDatabase] = ++d.idCount;
                output << d.idCount << " ";
                extractClause(clsptr, clspivot, output);
                extractChain(d, wtptr, output);
                wtptr = delptr + 1;
                extractDeletions(d, wtptr, output);
            }
        }
        output.close();
        Blablabla::comment("c LRAT proof written to " + Parameters::pathWitness);
    }
}

bool stepBack(witness& wt, long*& ip, long*& dp) {
    if(--ip < wt.intarray) {
        return false;
    } else {
        --dp;
        while(*ip != Constants::EndOfList) {
            --ip;
        }
        while(*dp != Constants::EndOfList) {
            --dp;
        }
    }
    return true;
}

void extractPremise(database& d, int*& clsp, bool& delpr, std::ofstream& op) {
    if(Database::isFlag(clsp, Constants::VerificationBit, Constants::SkipFlag)) {
        if(!delpr) {
            delpr = true;
            op << d.idCount << " d ";
        }
        op << clsp[Constants::IdCellDatabase] << " ";
    }
}

void extractClause(int*& clsp, int pv, std::ofstream& op) {
    if(pv != Constants::ReservedLiteral) {
        op << Blablabla::litToString(pv) << " ";
    }
    while(*clsp != Constants::EndOfList) {
        if(*clsp != pv && *clsp != -Constants::ReservedLiteral) {
            op << Blablabla::litToString(*clsp) << " ";
        }
        clsp++;
    }
    op << "0 ";
}

void extractChain(database& d, long*& wptr, std::ofstream& op) {
    while((wtoff = *wptr++) != Constants::EndOfList) {
        if(wtoff > 0) {
            op << Database::getPointer(d, wtoff)[Constants::IdCellDatabase] << " ";
        } else {
            op << -Database::getPointer(d, -wtoff)[Constants::IdCellDatabase] << " ";        }
    }
    op << "0" << std::endl;
}

void extractDeletions(database& d, long*& wptr, std::ofstream& op) {
    if(*wptr != Constants::EndOfList) {
        op << d.idCount << " d ";
        while((wtoff = *wptr++) != Constants::EndOfList) {
            op << Database::getPointer(d, wtoff)[Constants::IdCellDatabase] << " ";
        }
        op << "0" << std::endl;
    }
}

//-------------------------------
// Chain generation
//-------------------------------

int pivot;
int lit;
int* ptr;

void extractDependencies(witness& wt, model& m, database& d, int literal) {
    resetChain(wt);
    addChainLiteral(wt, literal);
    wt.head = wt.chain;
    while(wt.head < wt.last) {
        findShift(wt, m, d);
    }
    sortChain(wt, m);
    #ifdef VERBOSE
    Blablabla::logChain(wt, m, d);
    #endif
}

void findShift(witness& wt, model& m, database& d) {
    pivot = *(wt.head++);
    ptr = Database::getPointer(d, m.reason[pivot]);
    while((lit = *(ptr++)) != Constants::EndOfList) {
        if(lit != pivot && !wt.lits[-lit] && m.reason[-lit] != Constants::AssumedReason) {
            addChainLiteral(wt, -lit);
        }
    }
}

void resetChain(witness& wt) {
    wt.head = wt.last;
    while(wt.head > wt.chain) {
        wt.lits[*(--wt.head)] = false;
    }
    wt.last = wt.chain;
}



//-------------------------------
// Low-level commands
//-------------------------------

int** pos;
int* c1;
int* c2;

void addChainLiteral(witness& wt, int literal) {
    wt.lits[literal] = true;
    *(wt.last++) = literal;
}

void sortChain(witness& wt, model& m) {
    pos = m.position;
    qsort(wt.chain, wt.last - wt.chain, sizeof(int), compare);
}

int compare(const void *a, const void *b) {
    c1 = pos[*(int*) a];
    c2 = pos[*(int*) b];
    return (int)(c1 < c2) - (int)(c2 < c1);
}

}
