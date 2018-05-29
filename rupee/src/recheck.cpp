#include <stdlib.h>
#include <iostream>

#include "structs.hpp"
#include "extra.hpp"

namespace Recheck {

//----------------------
// Memory management
//----------------------

int it;

bool allocate(recheck& rc) {
    if(Parameters::recheck) {
        #ifdef VERBOSE
        Blablabla::log("Allocating rechecker");
        #endif
        rc.natlits = (bool*) malloc((2 * Stats::variableBound + 1) * sizeof(bool));
        rc.reslits = (bool*) malloc((2 * Stats::variableBound + 1) * sizeof(bool));
        if(rc.natlits == NULL || rc.reslits == NULL) {
            #ifdef VERBOSE
            Blablabla::log("Error at rechecker allocation");
            #endif
            Blablabla::comment("Memory management error");
            return false;
        } else {
            rc.natlits += Stats::variableBound;
            rc.reslits += Stats::variableBound;
            for(it = -Stats::variableBound; it <= Stats::variableBound; ++it) {
                rc.natlits[it] = false;
                rc.reslits[it] = false;
            }
        }
    }
    return true;
}

void deallocate(recheck& rc) {
    if(Parameters::recheck) {
        rc.natlits -= Stats::variableBound;
        rc.reslits -= Stats::variableBound;
        free(rc.natlits);
        free(rc.reslits);
    }
}

int lit;
int* hd;

void copyModel(model& m, bool* dst) {
    for(lit = -Stats::variableBound; lit <= Stats::variableBound; ++lit) {
        dst[lit] = false;
    }
    hd = m.array;
    while(hd < m.used) {
        dst[*hd] = true;
        ++hd;
    }
}

bool literalTranslation(int in, int& out) {
    if(in == 1 || in == -1) {
        return false;
    } else {
        if(in > 0) {
            out = in - 1;
        } else if(in < 0) {
            out = in + 1;
        } else {
            out = 0;
        }
        return true;
    }
}

// void extractRecheck(recheck& rc, database& d) {
//     std::ofstream output;
//     int* clause;
//     int literal;
//     int translation;

//     output.open(Parameters::pathRecheck, std::ofstream::out | std::ofstream::trunc);
//     literalTranslation(rc.pivot, translation);
//     // output << "v " << Stats::variableBound << " " << translation << " " << d.max << " " << rc.instruction + 1 << std::endl;
//     output << "v " << translation << " " << rc.instruction + 1 << std::endl;
//     clause = Database::getPointer(d, rc.clause);
//     output << "n ";
//     while((literal = *clause) != 0) {
//         if(literalTranslation(literal, translation)) {
//             output << translation << " ";
//         }
//         ++clause;
//     }
//     output << "0 ";
//     for(literal = -Stats::variableBound; literal <= Stats::variableBound; ++literal) {
//         if(rc.natlits[literal] && literal != 0 && literalTranslation(literal, translation)) {
//             output << translation << " ";
//         }
//     }
//     output << "0" << std::endl;
//     if(rc.resolvent != 0) {
//         clause = Database::getPointer(d, rc.resolvent);
//         output << "r ";
//         while((literal = *clause) != 0) {
//             if(literalTranslation(literal, translation)) {
//                 output << translation << " ";
//             }
//             ++clause;
//         }
//         output << "0 ";
//         for(literal = -Stats::variableBound; literal <= Stats::variableBound; ++literal) {
//             if(rc.reslits[literal] && literal != 0 && literalTranslation(literal, translation)) {
//                 output << translation << " ";
//             }
//         }
//         output << "0" << std::endl;
//     } else {
//         output << "r 0 0" << std::endl;
//     }
//     output.close();
// }

void extractRecheck(recheck& rc, database& d) {
    int* clause;
    int literal;
    int translation;
    std::cout << "Detected an incorrect instruction at instruction " << rc.instruction + 1 << "." << std::endl;
    std::cout << "Introduced clause C: ";
    clause = Database::getPointer(d, rc.clause);
    while((literal = *clause) != 0) {
        if(literalTranslation(literal, translation)) {
            std::cout << translation << " ";
        }
        ++clause;
    }
    std::cout << "0" << std::endl;
    std::cout << "The folowing literals were derived by unit propagation on ~C without reaching a conflict:" << std::endl;
    for(literal = -Stats::variableBound; literal <= Stats::variableBound; ++literal) {
        if(rc.natlits[literal] && literal != 0 && literalTranslation(literal, translation)) {
            std::cout << translation << " ";
        }
    }
    std::cout << std::endl << "C was thus decided not to be a RUP clause in the accumulated formula." << std::endl;
    if(rc.resolvent != 0) {
        std::cout << "Furthermore, the following clause D was found in the accumulated formula:" << std::endl;
        clause = Database::getPointer(d, rc.resolvent);
        while((literal = *clause) != 0) {
            if(literalTranslation(literal, translation)) {
                std::cout << translation << " ";
            }
            ++clause;
        }
        std::cout << "0" << std::endl;
        literalTranslation(rc.pivot, translation);
        std::cout << "C and D are resolvable upon literal " << translation << "." << std::endl;
        std::cout << "The folowing literals were derived by unit propagation on ~(C \\/ D \\ {" << -translation << "} without reaching a conflict:" << std::endl;
        for(literal = -Stats::variableBound; literal <= Stats::variableBound; ++literal) {
            if(rc.reslits[literal] && literal != 0 && literalTranslation(literal, translation)) {
                std::cout << translation << " ";
            }
        }
        literalTranslation(rc.pivot, translation);
        std::cout << std::endl << "C was thus decided not to be a RAT clause in the accumulated formula upon the pivot " << translation << " which first occurred in C in the input DRAT proof." << std::endl;
    } else {
        std::cout << "Furthermore, C is the empty clause, so C cannot be a RAT." << std::endl;
    }
}

//
// //----------------------
// // Clausal checks
// //----------------------
//
// int lit;
// int* hd;
//
//
// void retrieveInstruction(proof& r, recheck& rc) {
//     rc.clause = r.array[rc.instruction];
//     rc.pivot = r.pivot[rc.instruction];
// }
//
// bool checkPointer(int* clause, bool* model, int except) {
//     while((lit = *clause++) != Constants::EndOfList) {
//         if(!model[-lit] && lit != except) {
//             #ifdef VERBOSE
//             Blablabla::log("Model does not subsume clause complement");
//             #endif
//             return false;
//         }
//     }
//     return true;
// }
//
// bool checkConsistency(bool* model) {
//     for(lit = 0; lit <= Stats::variableBound; ++lit) {
//         if(model[lit] && model[-lit]) {
//             #ifdef VERBOSE
//             Blablabla::log("Model is inconsistent");
//             #endif
//             return false;
//         }
//     }
//     return true;
// }
//
// bool checkPresence(database& d, recheck& rc) {
//     if(Database::isFlag(Database::getPointer(d, rc.resolvent), Constants::ActivityBit, Constants::ActiveFlag)) {
//         return true;
//     } else {
//         #ifdef VERBOSE
//         Blablabla::log("Resolution clause does not occur in accumulated formula");
//         #endif
//         return false;
//     }
// }
//
// bool checkClause(recheck& rc, database& d) {
//     return checkConsistency(rc.natlits) && checkPointer(Database::getPointer(d, rc.clause), rc.natlits, Constants::EndOfList);
// }
//
// bool checkResolvent(recheck& rc, database& d) {
//     return checkConsistency(rc.reslits) &&
//             checkPresence(d, rc) &&
//             checkPointer(Database::getPointer(d, rc.clause), rc.reslits, rc.pivot) &&
//             checkPointer(Database::getPointer(d, rc.resolvent), rc.reslits, -rc.pivot);
// }




}
