#include <iostream>
#include <string>

#include "structs.hpp"
#include "extra.hpp"

namespace Proof {

//-------------------------------
// Memory management
//-------------------------------

bool allocate(proof& r) {
	#ifdef VERBOSE
	Blablabla::log("Allocating proof list.");
	#endif
	r.max = Parameters::proofSize;
	r.used = 0;
	r.array = (long*) malloc (r.max * sizeof(long));
	r.pivot = (int*) malloc (r.max * sizeof(int));
	r.kind = (bool*) malloc (r.max * sizeof(bool));
	if(r.array == NULL || r.pivot == NULL || r.kind == NULL) {
		#ifdef VERBOSE
		Blablabla::log("Error at proof list allocation.");
		#endif
		Blablabla::comment("Memory management error.");
		return false;
	} else {
		return true;
	}
}

bool reallocate(proof& r) {
	#ifdef VERBOSE
	Blablabla::log("Reallocating proof list.");
	#endif
	r.max *= 2;
	r.array = (long*) realloc (r.array, r.max * sizeof(long));
	r.pivot = (int*) realloc (r.pivot, r.max * sizeof(int));
	r.kind = (bool*) realloc (r.kind, r.max * sizeof(bool));
	if(r.array == NULL || r.pivot == NULL || r.kind == NULL) {
		#ifdef VERBOSE
		Blablabla::log("Error at proof list reallocation.");
		#endif
		Blablabla::comment("Memory management error.");
		return false;
	} else {
		return true;
	}
}

void deallocate(proof& r) {
	#ifdef VERBOSE
	Blablabla::log("Deallocating proof list.");
	#endif
	free(r.array);
	free(r.pivot);
	free(r.kind);
}

//-------------------------------
// Instruction storage/retrival
//-------------------------------

// void storeInstruction(proof& r, bool file, long offset, int pivot, bool kind) {
bool storeInstruction(proof& r, long offset, int pivot, bool kind) {
	if(r.used >= r.max) {
		if(!reallocate(r)) { return false; }
	}
	r.array[r.used] = offset;
	r.pivot[r.used] = pivot;
	r.kind[r.used++] = kind;
	return true;
}

bool storeDummy(proof& r, bool kind) {
	return storeInstruction(r, Constants::NoOffset, Constants::ConflictLiteral, kind);
}

void retrieveInstruction(proof& r, int position, long& offset, int& pivot, bool& kind) {
	offset = r.array[position];
	pivot = r.pivot[position];
	kind = r.kind[position];
}

bool nextInstruction(proof& r, int& position, long& offset, int& pivot, bool& kind) {
	if(position < r.used) {
		retrieveInstruction(r, position++, offset, pivot, kind);
		return true;
	} else {
		return false;
	}
}

bool prevInstruction(proof& r, int& position, long& offset, int& pivot, bool& kind) {
	if(position > 0) {
		retrieveInstruction(r, --position, offset, pivot, kind);
		return true;
	} else {
		return false;
	}
}

long off;

bool closeProof(proof& r, database& d, int position) {
	r.used = position + 1;
	if(!Database::deriveContradiction(d, off)) { return false; }
	r.array[position] = off;
	r.pivot[position] = Constants::ConflictLiteral;
	r.kind[position] = Constants::InstructionIntroduction;
	return true;
}

}
