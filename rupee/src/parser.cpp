#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>

#include "structs.hpp"
#include "extra.hpp"


namespace Parser {

std::string line;
int parsed;
int number;
int othernumber;

bool openFile(parser& p, bool f) {
    p.file = f;
    if(f == Constants::FilePremise) {
        p.inputfile = std::fopen(Parameters::pathPremise.c_str(), "r");
	} else {
        p.inputfile = std::fopen(Parameters::pathProof.c_str(), "r");
	}
    if(p.inputfile == NULL) {
        #ifdef VERBOSE
		Blablabla::log("File was not found.");
        #endif
		Blablabla::comment("Input error.");
		return false;
    }
    return true;
}

bool readHeader(parser& p) {
    while(true) {
        parsed = fscanf(p.inputfile, "p cnf %i %i \n", &number, &othernumber);
        if(parsed == EOF) {
            #ifdef VERBOSE
        	Blablabla::log("No header was found in CNF instance.");
            #endif
        	Blablabla::comment("Input error.");
        	return false;
        }
        if(parsed == 2) {
            #ifdef VERBOSE
			Blablabla::log("Header found.");
            #endif
			return true;
        }
        parsed = fscanf(p.inputfile, "%*s\n");
    }
}

bool inclause;

bool readClauses(parser& p, clause& c, hashtable& h, database& d, proof& r) {
    inclause = false;
    while(true) {
        if(!inclause) {
            if(Stats::isTimeout()) {
                Blablabla::comment("Timeout at " + std::to_string(Parameters::timeout) + "s");
                Blablabla::comment("s TIMEOUT");
                return false;
            }
            if((parsed = fscanf(p.inputfile, " d %i ", &number)) == 1) {
                Clause::setInstructionKind(c, Constants::InstructionDeletion);
            } else if((parsed = fscanf(p.inputfile, " %i ", &number)) == 1) {
                Clause::setInstructionKind(c, Constants::InstructionIntroduction);
                if(p.file == Constants::FilePremise) {
                    ++d.idCount;
                }
            } else if(parsed == EOF) {
                #ifdef VERBOSE
                Blablabla::log("Parsed file");
                #endif
                fclose(p.inputfile);
                return true;
            } else {
                #ifdef VERBOSE
                Blablabla::log("Could not parse file");
                #endif
                Blablabla::comment("Input error");
                return false;
            }
            inclause = true;
        } else {
            parsed = fscanf(p.inputfile, " %i ", &number);
            if(parsed == EOF) {
                #ifdef VERBOSE
                Blablabla::log("Could not parse file");
                #endif
                Blablabla::comment("Input error");
                return false;
            }
        }
        if(parsed == 1) {
            if(!Clause::processLiteral(c, number)) { return false; }
            if(number == Constants::EndOfList) {
                if(p.file == Constants::FilePremise) {
                    ++Stats::premiseLength;
                } else {
                    ++Stats::proofLength;
                }
                if(!Clause::processInstruction(c, h, d, r, p.file)) { return false; }
                inclause = false;
                Clause::resetBuffer(c);
            }
        } else {
            #ifdef VERBOSE
            Blablabla::log("Could not parse file");
            #endif
            Blablabla::comment("Input error");
            return false;
        }
    }
}

int readchar;
int shift;

bool readBinaryClauses(parser& p, clause& c, hashtable& h, database& d, proof& r) {
    inclause = false;
    while(true) {
        if(!inclause) {
            if(Stats::isTimeout()) {
                Blablabla::comment("Timeout at " + std::to_string(Parameters::timeout) + "s");
                Blablabla::comment("s TIMEOUT");
                return false;
            }
            if((readchar = getc(p.inputfile)) == Constants::BinaryDeletion) {
                Clause::setInstructionKind(c, Constants::InstructionDeletion);
            } else if(readchar == Constants::BinaryIntroduction) {
                Clause::setInstructionKind(c, Constants::InstructionIntroduction);
            } else if(readchar == EOF) {
                #ifdef VERBOSE
                Blablabla::log("Parsed file");
                #endif
                fclose(p.inputfile);
                return true;
            } else {
                #ifdef VERBOSE
                Blablabla::log("Could not parse file");
                #endif
                Blablabla::comment("Input error");
                return false;
            }
            inclause = true;
        } else {
            if((readchar = getc(p.inputfile)) == Constants::BinaryEndOfClause) {
                ++Stats::proofLength;
                if(!Clause::processLiteral(c, Constants::EndOfList)) { return false; }
                if(!Clause::processInstruction(c, h, d, r, p.file)) { return false; }
                inclause = false;
                Clause::resetBuffer(c);
            } else if (readchar == EOF) {
                #ifdef VERBOSE
                Blablabla::log("Could not parse file");
                #endif
                Blablabla::comment("Input error");
                return false;
            } else {
                number = readchar & Constants::BinaryMsbMask;
                if(readchar > Constants::BinaryMsbMask) {
                    shift = Constants::BinaryShiftStep;
                    do {
                        if((readchar = getc(p.inputfile)) == Constants::BinaryEndOfClause || readchar == EOF) {
                            #ifdef VERBOSE
                            Blablabla::log("Could not parse file");
                            #endif
                            Blablabla::comment("Input error");
                            return false;
                        } else {
                            number |= (readchar & Constants::BinaryMsbMask) << shift;
                            shift += Constants::BinaryShiftStep;
                        }
                    } while(readchar > Constants::BinaryMsbMask);
                }
                if(number % 2 == 0) {
                    if(!Clause::processLiteral(c, number / 2)) { return false; }
                } else {
                    if(!Clause::processLiteral(c, -(number - 1) / 2)) { return false; }
                }
            }
        }
    }
}

}
