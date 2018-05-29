#ifndef __STRUCTS_H_INCLUDED__
#define __STRUCTS_H_INCLUDED__

#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>

struct clause {
	int* array;
	int inner;
	int max;
	int used;
    bool* lits;
	bool kind;
	bool taut;
};

struct database {
    int* array;
    int max;
    int used;
    int idCount;
};

struct proof {
    long* array;
    int* pivot;
    bool* kind;
    int max;
    int used;
};

struct hashtable {
    long** array;
    int* used;
    int* max;
};

struct parser {
	bool file;
	std::ifstream input;
	FILE* inputfile;
};

struct model {
    int* array;
    bool* lits;
    long* reason;
    int** position;
    int* used;
    int* forced;
    int* head;
};

struct watchlist {
    long** array;
    int* used;
    int* max;
};

struct revision {
    int* cone;
	int* current;
    bool* lits;
    long* array;
    int used;
    int max;
};

struct witness {
	long* delarray;
	int delused;
	int delmax;
	long* intarray;
	int intused;
	int intmax;
	int* chain;
	int* head;
	int* last;
	bool* lits;
};

struct latency {
    long* array;
    int used;
    int max;
};

struct recheck {
	int instruction;
	long clause;
	long resolvent;
	int pivot;
	bool* natlits;
	bool* reslits;
};

struct checker {
    int position;
    long offset;
    int* pointer;
    int pivot;
    bool kind;
    int stage;
    struct model stack;
    struct watchlist watch;
    struct revision cone;
    struct latency rat;
    struct witness tvr;
	struct clause res;
	struct recheck kk;
};

namespace Objects {
	extern clause Clause;
	extern parser Parser;
	extern database Database;
	extern hashtable HashTable;
	extern proof Proof;
    extern checker Checker;
}


namespace Clause {

bool allocate(clause& c);
bool reallocate(clause& c);
void deallocate(clause& c);

bool addLiteral(clause& c, int literal);
void preventUnit(clause& c);
void closeBuffer(clause& c);
void resetBuffer(clause& c);

void setInnerClause(clause& c, int* pointer, int pivot);
void setOuterClause(clause& c, int* pointer, int pivot);
void resetToInner(clause& c);

void setInstructionKind(clause& c, bool kind);
bool processLiteral(clause& c, int literal);
// bool parseInstruction(clause& c, std::string& s);
bool processInstruction(clause& c, hashtable& h, database& d, proof& r, bool file);
int stringToLiteral(int literal);

void sortClause(clause& c);
bool equals(clause& c, int* ptr);
int compare(const void *a, const void *b);

}


namespace Database {

bool allocate(database& d);
bool reallocate(database& d);
void deallocate(database& d);

long getOffset(database& d, int* ptr);
int* getPointer(database& d, long offset);

int getId(int* ptr);
bool isFlag(int* ptr, int bit, bool value);
void setFlag(int* ptr, int bit, bool value);

void setClauseActive(int* ptr);
void setClauseInactive(int* ptr);

void setCopies(int* ptr, int cps);
void addCopy(database& d, long offset);
bool removeCopy(database& d, long offset);

bool insertBufferClause(database& d, clause& c, hashtable& h, bool file, long& offset);
void removeBufferClause(database& d, clause& c, hashtable& h, long& offset);
bool deriveContradiction(database& d, long& offset);

bool isEmpty(int* ptr);
bool nextNonFalsified(int*& ptr, int& lit, model& m);
bool findWatch(int*& watchlit, int*& bestfalse, int*& bestpos, model& m);
bool containsLiteral(int* ptr, int literal);

// bool recheckActivity(database& d);
// bool checkUpModel(database& d, bool* lits);
// bool copyFormula(database& d);
// bool checkFormulaEquality(database& d);
// bool checkRepetition(database& d);

}


namespace Proof {

bool allocate(proof& r);
bool reallocate(proof& r);
void deallocate(proof& r);

bool storeInstruction(proof& r, long offset, int pivot, bool kind);
bool storeDummy(proof& r, bool kind);
void retrieveInstruction(proof& r, int position, long& offset, int& pivot, bool& kind);
bool nextInstruction(proof& r, int& position, long& offset, int& pivot, bool& kind);
bool prevInstruction(proof& r, int& position, long& offset, int& pivot, bool& kind);

bool closeProof(proof& r, database& d, int position);

}


namespace HashTable {

bool allocate(hashtable& h);
bool reallocateRow(hashtable& h, int row);
void deallocate(hashtable& h);

unsigned int getHash(int* ptr);
bool match(hashtable& h, database& d, clause& c, unsigned int hash, long& offset, int& pos);

bool insertOffset(hashtable& h, unsigned int hash, long offset);
void removeOffset(hashtable& h, unsigned int hash, int pos);

}


namespace Parser {

bool openFile(parser& p, bool f);
bool readHeader(parser& p);
bool readClauses(parser& p, clause& c, hashtable& h, database& d, proof& r);
bool readBinaryClauses(parser& p, clause& c, hashtable& h, database& d, proof& r);

}


namespace Model {

bool allocate(model& m);
void deallocate(model& m);

bool assumeLiteral(model& m, int literal);
void propagateLiteral(model& m, int literal, long reason, int* pointer, bool hard);
void unassignLiteral(model& m, int literal, database& d);

bool propagateModel(model& m, watchlist& wl, database& d, bool hard);
void unpropagateModel(model& m, database& d, int* pointer);
void resetModel(model& m);

bool isSatisfied(model& m, int literal);
bool isFalsified(model& m, int literal);
bool isUnassigned(model& m, int literal);
bool isContradicted(model& m);
bool isUnit(model& m, int* pointer);

}



namespace WatchList {

bool allocate(long*& clarray, int& clused, int& clmax);
bool allocate(watchlist& wl);
bool reallocate(long*& clarray, int& clused, int& clmax);
void deallocate(watchlist& wl);

bool insertWatches(watchlist &wl, model& m, long offset, int* pointer);
void removeWatches(watchlist &wl, long offset, int* pointer);
bool alignWatches(watchlist& wl, model& m, long offset, int* pointer, int literal, long*& watch, bool hard);
bool resetWatches(watchlist& wl, model& m, long offset, int* pointer, int literal, long*& watch);
bool reviseWatches(watchlist& wl, model& m, long offset, int* pointer, int literal, long*& watch);

bool alignList(watchlist& wl, model& m, database& d, int literal, bool hard);
bool resetList(watchlist& wl, model& m, database& d, int literal);
bool reviseList(watchlist& wl, model& m, database& d, int literal);
bool collectList(watchlist& wl, latency& lt, database& d, int pivot, int literal);

bool addWatch(watchlist &wl, int literal, long offset);
void removeWatch(watchlist &wl, int literal, long* watch);
void findAndRemoveWatch(watchlist& wl, int literal, long offset);

bool checkWatchlist(watchlist& wl, database& d);

}



namespace Revision {

bool allocate(revision& v);
bool reallocate(revision& v);
void deallocate(revision& v);

bool addToCone(revision& v, model& m, database& d, int literal, int* position);
bool isInCone(revision& v, long offset, int literal, database& d);
bool getCone(revision& v, model& m, database& d, int* ptr);
bool voidRevision(revision& v);

bool reviseCone(revision& v, watchlist& wl, model& m, database& d);
bool checkCone(revision& v, model& m);
void resetCone(revision& v);

bool applyRevision(revision& v, model& m, database& d);
bool resetWatches(revision& v, watchlist& wl, model& m, database& d);

}



namespace Latency {

bool allocate(latency& lt);
bool reallocate(latency& lt);
void deallocate(latency& lt);

bool collectCandidates(latency& lt, watchlist& wl, database& d, int pivot);
void resetCandidates(latency& lt);
bool addCandidate(latency& lt, long offset);

bool checkResolventsRup(latency& lt, checker& c, clause& cl, database& d, int pivot, bool& result);

}



namespace Witness {

bool allocate(witness& wt);
bool reallocate(witness& wt, bool array);
void deallocate(witness& wt);

bool insertIntroduction(witness& wt, long value);
bool insertDeletion(witness& wt, long value);
bool openWitness(witness& wt, long offset);
bool setRupWitness(witness& wt);
bool setPremiseWitness(witness& wt);
bool setRatWitness(witness& wt, int literal);
bool setResolventWitness(witness& wt, long offset);
bool dumpChain(witness& wt, model& m, database& d);
bool closeWitness(witness& wt);

void extractWitness(witness& wt, database& d);
bool stepBack(witness& wt, long*& ip, long*& dp);
void extractPremise(database& d, int*& clsp, bool& delpr, std::ofstream& op);
void extractClause(int*& clsp, int pv, std::ofstream& op);
void extractChain(database& d, long*& wptr, std::ofstream& op);
void extractDeletions(database& d, long*& wptr, std::ofstream& op);

void extractDependencies(witness& wt, model& m, database& d, int literal);
void findShift(witness& wt, model& m, database& d);
void resetChain(witness& wt);

void addChainLiteral(witness& wt, int literal);
void sortChain(witness& wt, model& m);
int compare(const void *a, const void *b);

}

namespace Recheck {

bool allocate(recheck& rc);
void deallocate(recheck& rc);

void copyModel(model& m, bool* dst);
void extractRecheck(recheck& rc, database& d);

}



namespace Checker {

bool allocate(checker& c);
void deallocate(checker& c);

bool isFinished(checker& c);
bool isVerified(checker& c);
bool isBuggy(checker& c);

bool preprocessProof(checker& c, proof& r, database& d);
bool verifyProof(checker& c, proof& r, database& d);
bool recheckInstruction(checker& c, database& d, proof& r);

bool preprocessIntroduction(checker& c, database& d);
bool preprocessDeletion(checker& c, database& d);

bool verifyIntroduction(checker& c, database& d);
bool verifyDeletion(checker& c, database& d);

void recheckFormula(checker& c, database& d, proof& r);
void recheckNaturalModel(checker& c, database& d);
void recheckResolventModel(checker& c, database& d);

void precheckRup(checker& c, database& d, int* pointer, bool& result);
bool checkRup(checker& c, database& d, bool& result, bool* copy, int counterpivot, long reason);
bool checkInference(checker& c, database& d);

}


#endif
