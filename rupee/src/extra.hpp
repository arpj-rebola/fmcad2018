#ifndef __EXTRA_H_INCLUDED__
#define __EXTRA_H_INCLUDED__

#include <string>
#include <sys/time.h>

#include "structs.hpp"

namespace Constants {
	const int ExtraCellsDatabase       =  2;
	const int FlagsCellDatabase        = -1;
	const int IdCellDatabase           = -2;

	const int ActivityBit              = 0;
	const int RecheckBit			   = 6;
	const bool ActiveFlag              = true;
	const bool InactiveFlag            = false;
	const bool InstructionDeletion     = InactiveFlag;
    const bool InstructionIntroduction = ActiveFlag;

	const int OriginalityBit           = 1;
	const bool PremiseFlag             = true;
    const bool DerivedFlag             = false;
	const bool FilePremise             = PremiseFlag;
    const bool FileProof               = DerivedFlag;

	const int PersistencyBit           = 2;
	const bool PersistentFlag          = true;
    const bool TemporalFlag            = false;

	const int VerificationBit          = 3;
	const int TrimmingBit              = 7;
	const bool ScheduledFlag           = true;
    const bool SkipFlag                = false;

	const int PseudounitBit            = 4;
	const bool ReasonFlag              = true;
    const bool PassiveFlag             = false;

	const int PartitionBit             = 5;
	const bool RedFlag                 = true;
	const bool BlueFlag                = false;

	const int CopiesShift              = 8;
	const int CopiesMask               = 255;

	const int ArgumentsFilePremise     = 1;
	const int ArgumentsFileProof       = 2;
	const int ArgumentsRest            = 3;

	const int ReservedLiteral          = 1;
	const int ConflictLiteral          = 0;
	const long AssumedReason           = 0;
	const int EndOfList                = 0;
	const long NoOffset                = 0;

	const bool SoftPropagation         = false;
	const bool HardPropagation         = true;

	const bool CheckCorrect            = true;
	const bool CheckIncorrect          = false;

	const int StagePreprocessing       = 0;
	const int StageVerifying           = 20;
	const int StageCorrect             = 40;
	const int StageIncorrect           = 60;
	const int StageRecheck             = 80;
	const int StageReject              = 100;
	const int StageBug                 = 120;

	const int DeletionModeSkip         = 1;
	const int DeletionModeUnrestricted = 3;

	const char BinaryDeletion = 100;
	const char BinaryIntroduction = 97;
	const char BinaryMsbMask = 127;
	const char BinaryEndOfClause = 0;
	const int BinaryShiftStep = 7;
}

namespace Parameters {
	extern std::string pathPremise;
	extern std::string pathProof;
	extern std::string pathWitness;
	extern std::string pathRecheck;
	extern int databaseSize;
	extern int hashDepth;
	extern int variableBound;
	extern int proofSize;
	extern int deletionMode;
	extern bool generateLrat;
	extern bool verbosity;
	extern bool printStats;
	extern bool recheck;
	extern bool lratRmark;
	extern int timeout;
	extern bool binaryDrat;

	void setPremise(std::string path);
	void setProof(std::string path);
	void setWitness(std::string path);
	void setRecheck(std::string path);
}

namespace Blablabla {
	extern int level;

	void increase();
	void decrease();
	void log(std::string str);
	std::string clauseToString(int* ptr);
	void comment(std::string str);
	std::string litToString(int lit);

	void logDatabase(database& d);
	void logProof(proof& r, database& d);
	void logModel(model& m);
	void logReasons(model& m, database& d);
	void logWatchList(watchlist& wl, int literal, database& d);
	void logCone(revision& v);
	void logLatency(latency& lt, database& d);
	void logChain(witness& wt, model& m, database& d);
	void logRevision(revision& v, database& d);
	void logRecheckModel(bool* rc);
}

namespace Stats {
	extern int proofLength;
	extern int premiseLength;
	extern struct timeval startTime;
	extern struct timeval parsingTime;
	extern struct timeval preprocessingTime;
	extern struct timeval checkingTime;
	extern struct timeval endTime;
	extern struct timeval deleteTime;
	extern int variableBound;
	extern int clauseDeletions;
	extern int skippedDeletions;
	extern int reasonDeletions;
	extern int ratIntroductions;

	long msDifference(struct timeval& t0, struct timeval& t1);
	void resetDeleteTime();
	void startDeleteTime();
	void stopDeleteTime();
	bool isTimeout();
}

#endif
