#include <stdlib.h>
#include <iostream>
#include <string>
#include <sys/time.h>

#include "structs.hpp"
#include "extra.hpp"

# define timerclear(tvp)                                  \
  ((tvp)->tv_sec = (tvp)->tv_usec = 0)
# define timeradd(a, b, result)						      \
  do {									                  \
    (result)->tv_sec = (a)->tv_sec + (b)->tv_sec;		  \
    (result)->tv_usec = (a)->tv_usec + (b)->tv_usec;	  \
    if ((result)->tv_usec >= 1000000)					  \
      {									                  \
	++(result)->tv_sec;						              \
	(result)->tv_usec -= 1000000;					      \
      }									                  \
  } while (0)
# define timersub(a, b, result)						      \
  do {									                  \
    (result)->tv_sec = (a)->tv_sec - (b)->tv_sec;		  \
    (result)->tv_usec = (a)->tv_usec - (b)->tv_usec;	  \
    if ((result)->tv_usec < 0) {					      \
      --(result)->tv_sec;						          \
      (result)->tv_usec += 1000000;					      \
    }									                  \
} while (0)

namespace Objects {
	clause Clause;
	parser Parser;
	database Database;
	hashtable HashTable;
	proof Proof;
	checker Checker;
}

namespace Parameters {
std::string pathPremise;
std::string pathProof;
std::string pathWitness;
std::string pathRecheck;
int databaseSize = 10000;
int hashDepth = 10;
int noVariables = 1000;
int proofSize = 5000;
int deletionMode = Constants::DeletionModeUnrestricted;
bool generateLrat = true;
bool lratRmark = false;
bool verbosity = false;
bool printStats = false;
bool recheck = false;
int timeout = 5000;
bool binaryDrat = false;

void setPremise(std::string path) {
    pathPremise = path;
	#ifdef VERBOSE
    Blablabla::log("Premise: " + path);
	#endif
}

void setProof(std::string path) {
    pathProof = path;
	#ifdef VERBOSE
    Blablabla::log("Proof: " + path);
	#endif
}

void setWitness(std::string path) {
    pathWitness = path;
	#ifdef VERBOSE
    Blablabla::log("Witness: " + path);
	#endif
}

void setRecheck(std::string path) {
    pathRecheck = path;
	#ifdef VERBOSE
    Blablabla::log("Recheck: " + path);
	#endif
}

}

namespace Blablabla {

int level = 0;

int widthcount;
int spacecount;
int* stackit;
long* watchit;
int* clause;
int lit;

// #ifdef VERBOSE
void increase() {
    Blablabla::level++;
}

void decrease() {
    Blablabla::level--;
}

void log(std::string str) {
    if(Parameters::verbosity) {
        for(spacecount = 0; spacecount < level; ++spacecount) {
            std::cout << ":   ";
        }
        std::cout << str << std::endl;
    }
}
// #endif

void comment(std::string str) {
    std::cout << str << std::endl;
}

std::string litToString(int lit) {
    if(lit == Constants::ReservedLiteral) {
        return "0";
    } else if (lit == -Constants::ReservedLiteral){
        return "-0";
    } else if (lit == Constants::ConflictLiteral) {
        return "@";
    } else if(lit < 0) {
        return std::to_string(lit + Constants::ReservedLiteral);
    } else {
        return std::to_string(lit - Constants::ReservedLiteral);
    }
}

// #ifdef VERBOSE
std::string clauseToString(int* ptr) {
    std::string ans = "[ ";
    while(*ptr != Constants::EndOfList) {
        ans = ans + litToString(*ptr) + " ";
        ++ptr;
    }
    ans = ans + "]";
    return ans;
}

int* ppptr;
int ppit;
int ppsize;
int pplit;
long ppoff;
long* ppwatch;
std::string ppflags;
std::string ppline;

void logDatabase(database& d) {
	if(Parameters::verbosity) {
		Blablabla::log("DATABASE");
		Blablabla::increase();
		ppptr = d.array + Constants::ExtraCellsDatabase;
		while(ppptr - d.array < d.used) {
			ppflags = "";
			if(Database::isFlag(ppptr, Constants::ActivityBit, Constants::ActiveFlag)) {
				ppflags += "A";
			}
			if(Database::isFlag(ppptr, Constants::OriginalityBit, Constants::PremiseFlag)) {
				ppflags += "O";
			}
			if(Database::isFlag(ppptr, Constants::PersistencyBit, Constants::PersistentFlag)) {
				ppflags += "P";
			}
			if(Database::isFlag(ppptr, Constants::VerificationBit, Constants::ScheduledFlag)) {
				ppflags += "X";
			}
			if(Database::isFlag(ppptr, Constants::PseudounitBit, Constants::ReasonFlag)) {
				ppflags += "R";
			}
			Blablabla::log(Blablabla::clauseToString(ppptr) + "   " + std::to_string(Database::getOffset(d, ppptr)) + "   " + ppflags);
			while(*ppptr != Constants::EndOfList) { ++ppptr; }
			ppptr += 3;
		}
		Blablabla::log("");
		Blablabla::decrease();
	}
}

void logProof(proof& r, database& d) {
	if(Parameters::verbosity) {
		Blablabla::log("PROOF");
		Blablabla::increase();
		for(ppit = 0; ppit < r.used; ++ppit) {
			if(r.kind[ppit] == Constants::InstructionDeletion) {
				ppline = "---";
			} else {
				ppline = "+++";
			}
			ppline += Blablabla::clauseToString(Database::getPointer(d, r.array[ppit])) + "  (" + Blablabla::litToString(r.pivot[ppit]) + ")";
			Blablabla::log(ppline);
		}
		Blablabla::log("");
		Blablabla::decrease();
	}
}

void logModel(model& m) {
	if(Parameters::verbosity) {
		ppline = "[ ";
	    for(ppptr = m.array; ppptr < m.used; ++ppptr) {
	        if(ppptr == m.forced) {
	            ppline += "|| ";
	        }
	        ppline += Blablabla::litToString(*ppptr);
	        if(ppptr == m.head) {
	            ppline += "*";
	        }
	        ppline += " ";
	    }
	    ppline += "]";
	    if(ppptr == m.forced) {
	        ppline += "||";
	    }
	    if(ppptr == m.head) {
	        ppline += "*";
	    }
	    Blablabla::log(ppline);
	}
}

void logReasons(model& m, database& d) {
	if(Parameters::verbosity) {
		for(ppptr = m.array; ppptr < m.used; ++ppptr) {
	        pplit = *ppptr;
	        ppline = Blablabla::litToString(pplit) + ":  \t";
	        if((ppoff = m.reason[pplit]) != Constants::AssumedReason) {
	            ppline += Blablabla::clauseToString(Database::getPointer(d, ppoff));
	        }
	        Blablabla::log(ppline);
	    }
	}
}

void logWatchList(watchlist& wl, int literal, database& d) {
	if(Parameters::verbosity) {
	    for(ppwatch = wl.array[literal]; *(ppwatch) != Constants::EndOfList; ++ppwatch) {
	        Blablabla::log(Blablabla::clauseToString(Database::getPointer(d, *ppwatch)));
	    }
	}
}

void logCone(revision& v) {
	if(Parameters::verbosity) {
	    ppline = "Cone: [ ";
	    for(ppptr = v.cone; ppptr < v.current; ++ppptr) {
	        ppline += Blablabla::litToString(*ppptr) + " ";
	    }
	    ppline += "]";
	    Blablabla::log(ppline);
	}
}

void logLatency(latency& lt, database& d) {
	if(Parameters::verbosity) {
		for(ppit = 0; ppit < lt.used; ++ppit) {
	        Blablabla::log(Blablabla::clauseToString(Database::getPointer(d, lt.array[ppit])));
	    }
	}
}

void logChain(witness& wt, model& m, database& d) {
	if(Parameters::verbosity) {
	    ppptr = wt.chain;
	    Blablabla::log("Resolution chain:");
	    Blablabla::increase();
	    while(ppptr < wt.last) {
	        Blablabla::log(Blablabla::litToString(*ppptr) + ":\t" + Blablabla::clauseToString(Database::getPointer(d, m.reason[*ppptr])));
	        ++ppptr;
	    }
	    Blablabla::decrease();
	}
}

void logRevision(revision& v, database& d) {
	if(Parameters::verbosity) {
	    ppsize = (int) v.array[v.used - 1];
	    Blablabla::log("Revision:");
	    Blablabla::increase();
	    while(ppsize > 0) {
	        Blablabla::log("#" + std::to_string(v.array[v.used - 1 - 3 * ppsize]) + ": " + Blablabla::litToString(v.array[v.used - 3 * ppsize]) + "\t" + Blablabla::clauseToString(Database::getPointer(d, v.array[v.used + 1 - 3 * ppsize])));
			ppsize--;
	    }
	    Blablabla::decrease();
	}
}

void logRecheckModel(bool* rc) {
	if(Parameters::verbosity) {
		ppline = "";
		for(ppit = -Stats::variableBound; ppit <= Stats::variableBound; ++ppit) {
			if(rc[ppit]) {
				ppline += Blablabla::litToString(ppit) + " ";
			}
		}
		Blablabla::log(ppline);
	}
}

// #endif

}

namespace Stats {
    int variableBound = Parameters::noVariables;
	int proofLength = 0;
	int premiseLength = 0;
	int deletionLength = 0;
	struct timeval startTime;
	struct timeval parsingTime;
	struct timeval preprocessingTime;
	struct timeval checkingTime;
	struct timeval endTime;
	struct timeval deleteTime;
	struct timeval scratchTimeOne;
	struct timeval scratchTimeTwo;
	struct timeval scratchTimeThree;
	int clauseDeletions = 0;
	int skippedDeletions = 0;
	int reasonDeletions = 0;
	int ratIntroductions = 0;
    int timeoutCalls = 0 ;

	long msDifference(struct timeval& t0, struct timeval& t1) {
		timersub(&t1, &t0, &scratchTimeThree);
		return (long) ((scratchTimeThree.tv_sec * 1000L) + (scratchTimeThree.tv_usec / 1000L));
	}

	void resetDeleteTime() {
		timerclear(&deleteTime);
	}

	void startDeleteTime() {
		gettimeofday(&scratchTimeOne, NULL);
	}

	void stopDeleteTime() {
		gettimeofday(&scratchTimeTwo, NULL);
		timersub(&scratchTimeTwo, &scratchTimeOne, &scratchTimeThree);
		timeradd(&scratchTimeThree, &deleteTime, &scratchTimeOne);
		deleteTime = scratchTimeOne;
	}

    bool isTimeout() {
        gettimeofday(&scratchTimeTwo, NULL);
        return (int) msDifference(startTime, scratchTimeTwo) > 1000 * Parameters::timeout;
    }
}
