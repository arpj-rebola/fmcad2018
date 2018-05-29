#include <iostream>
#include <sys/time.h>

#include "structs.hpp"
#include "extra.hpp"

namespace Frontend {

bool readArguments(int argc, char* argv[]) {
	std::string stringArgument;
	int premiseArgument = Constants::ArgumentsFilePremise;
	Parameters::recheck = true;
	for(int i = 1; i < argc; ++i) {
		stringArgument = argv[i];
		if(stringArgument[0] == '-') {
			stringArgument = stringArgument.substr(1);
			if(stringArgument == "help") {
				Blablabla::comment("Usage:");
				Blablabla::comment("\trupee [options] [DIMACS CNF file] [DRAT proof file]");
				Blablabla::comment("Options:");
				Blablabla::comment("\t-help\t\t\tPrints this help message");
				Blablabla::comment("\t-skip-deletion\t\tLeaves deletion instructions unchecked (DRAT-trim mode)");
				Blablabla::comment("\t-full-deletion\t\tFully checks unrestricted deletion");
				Blablabla::comment("\t-lrat [FILE]\t\tProduces an LRAT witness stored in [FILE]");
				Blablabla::comment("\t-stats\t\tPrints out stats about the run");
				Blablabla::comment("\t-binary\t\tSupports binary DRAT files");
				return false;
			} else if(stringArgument == "skip-deletion") {
				Parameters::deletionMode = Constants::DeletionModeSkip;
			} else if(stringArgument == "full-deletion") {
				Parameters::deletionMode = Constants::DeletionModeUnrestricted;
			} else if(stringArgument == "lrat") {
				stringArgument = argv[++i];
				Parameters::setWitness(stringArgument);
			} else if(stringArgument == "stats") {
				Parameters::printStats = true;
			} else if(stringArgument == "binary") {
				Parameters::binaryDrat = true;
			}  else {
				Blablabla::comment("Error: invalid input.");
				Blablabla::comment("\tType rupee -help for more information.");
				return false;
			}
		} else if(premiseArgument == Constants::ArgumentsFilePremise) {
			Parameters::setPremise(stringArgument);
			premiseArgument = Constants::ArgumentsFileProof;
		} else if(premiseArgument == Constants::ArgumentsFileProof) {
			Parameters::setProof(stringArgument);
			premiseArgument = Constants::ArgumentsRest;
		} else {
			Blablabla::comment("Error: invalid arguments.");
			Blablabla::comment("\tType rupee -help for more information.");
			return false;
		}
	}
	if(premiseArgument != Constants::ArgumentsRest) {
		Blablabla::comment("Error: invalid arguments.");
		Blablabla::comment("\tType rupee -help for more information.");
		return false;
	}
	return true;
}

bool initialize() {
	#ifdef VERBOSE
	Blablabla::log("Initializing basic data structures.");
	Blablabla::increase();
	#endif
		if(!Clause::allocate(Objects::Clause)) { return false; }
		if(!Database::allocate(Objects::Database)) { return false; }
		if(!Proof::allocate(Objects::Proof)) { return false; }
		if(!HashTable::allocate(Objects::HashTable)) { return false; }
	#ifdef VERBOSE
	Blablabla::decrease();
	#endif
	return true;
}

bool parse() {
	Blablabla::comment("c Parsing CNF instance " + Parameters::pathPremise);
	#ifdef VERBOSE
	Blablabla::increase();
	#endif
		if(!Parser::openFile(Objects::Parser, Constants::FilePremise)) { return false; }
		if(!Parser::readHeader(Objects::Parser)) { return false; }
		if(!Parser::readClauses(Objects::Parser, Objects::Clause, Objects::HashTable, Objects::Database, Objects::Proof)) { return false; }
	Blablabla::comment("c Parsing DRAT proof " + Parameters::pathProof);
		if(!Parser::openFile(Objects::Parser, Constants::FileProof)) { return false; }
		if(Parameters::binaryDrat){
			if(!Parser::readBinaryClauses(Objects::Parser, Objects::Clause, Objects::HashTable, Objects::Database, Objects::Proof)) { return false; }
		} else {
			if(!Parser::readClauses(Objects::Parser, Objects::Clause, Objects::HashTable, Objects::Database, Objects::Proof)) { return false; }
		}
	#ifdef VERBOSE
	Blablabla::decrease();
	Blablabla::log("Deallocating parsing data structures");
	Blablabla::increase();
	#endif
		HashTable::deallocate(Objects::HashTable);
		Clause::deallocate(Objects::Clause);
	#ifdef VERBOSE
	Blablabla::decrease();
	#endif
	return true;
}

bool check() {
	Blablabla::comment("c Checking DRAT proof");
	if(!Checker::allocate(Objects::Checker)) { return false; }
	#ifdef VERBOSE
	Blablabla::logDatabase(Objects::Database);
	Blablabla::logProof(Objects::Proof, Objects::Database);
	#endif
	if(!Checker::preprocessProof(Objects::Checker, Objects::Proof, Objects::Database)) { return false; }
	gettimeofday (&Stats::preprocessingTime, NULL);
	if(!Checker::verifyProof(Objects::Checker, Objects::Proof, Objects::Database)) { return false; }
	gettimeofday (&Stats::checkingTime, NULL);
	if(Checker::isFinished(Objects::Checker)) { return true; }
	if(!Checker::isVerified(Objects::Checker)) {
		Objects::Checker.stage = Constants::StageReject;
	}
	return true;
}

void output() {
	gettimeofday(&Stats::endTime, NULL);
	if(Checker::isVerified(Objects::Checker)) {
		Blablabla::comment("s ACCEPTED");
		Witness::extractWitness(Objects::Checker.tvr, Objects::Database);
	} else {
		Blablabla::comment("s REJECTED");
		Recheck::extractRecheck(Objects::Checker.kk, Objects::Database);
	}
	std::cout << "nf " << Stats::premiseLength << std::endl;
	std::cout << "np " << Stats::proofLength << std::endl;
	std::cout << "nr " << Stats::ratIntroductions << std::endl;
	std::cout << "dc " << Stats::clauseDeletions << std::endl;
	std::cout << "ds " << Stats::skippedDeletions << std::endl;
	std::cout << "dr " << Stats::reasonDeletions << std::endl;
	std::cout << "tt " << Stats::msDifference(Stats::startTime, Stats::endTime) << std::endl;
	std::cout << "tp " << Stats::msDifference(Stats::startTime, Stats::parsingTime) << std::endl;
	std::cout << "tf " << Stats::msDifference(Stats::parsingTime, Stats::preprocessingTime) << std::endl;
	std::cout << "tb " << Stats::msDifference(Stats::preprocessingTime, Stats::checkingTime) << std::endl;
	std::cout << "td " << (long) ((Stats::deleteTime.tv_sec * 1000L) + (Stats::deleteTime.tv_usec / 1000L)) << std::endl;
}

void deallocate() {
	#ifdef VERBOSE
	Blablabla::log("Deallocating data structures.");
	Blablabla::increase();
	#endif
	Database::deallocate(Objects::Database);
	Proof::deallocate(Objects::Proof);
	Checker::deallocate(Objects::Checker);
	#ifdef VERBOSE
	Blablabla::decrease();
	#endif
}


}

int main(int argc, char* argv[]) {
	gettimeofday (&Stats::startTime, NULL);
	Stats::resetDeleteTime();
	if(!Frontend::readArguments(argc, argv)) { return 1; }
	if(!Frontend::initialize()) { return 1; }
	if(!Frontend::parse()) {  return 1; }
	gettimeofday (&Stats::parsingTime, NULL);
	if(!Frontend::check()) { return 1; }
	Frontend::output();
	Frontend::deallocate();
}
