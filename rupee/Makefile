BINPATH = bin/
OBJSPATH = obj/
SRCPATH = src/

MkO = $(OBJSPATH)$(1).o
MkC = $(SRCPATH)$(1).cpp
MkH = $(SRCPATH)$(1).hpp

RUPEEOBJS = $(call MkO,checker) \
		$(call MkO,clause) \
		$(call MkO,database) \
		$(call MkO,extra) \
		$(call MkO,hashtable) \
		$(call MkO,latency) \
		$(call MkO,model) \
		$(call MkO,parser) \
		$(call MkO,proof) \
		$(call MkO,recheck) \
		$(call MkO,revision) \
		$(call MkO,rupee) \
		$(call MkO,watchlist) \
		$(call MkO,witness)

# CHECKOBJS = $(call MkO,check-core) \
# 		$(call MkO,check-lexer) \
# 		$(call MkO,check-hashtable) \
# 		$(call MkO,check-proof) \
# 		$(call MkO,check-database) \
# 		$(call MkO,check-parser)

LD = g++

LDFLAGS = -o

CPP = g++

CPPFLAGS  = -g -O2 -Wall -std=c++11 -c

RUPEEBIN = $(BINPATH)rupee

LRATBIN = $(BINPATH)lratcheck

SICKBIN = $(BINPATH)sickcheck

BRATTODRATBIN = $(BINPATH)brattodrat

all : directories $(RUPEEOBJS) $(call MkO,brattodrat)
	@echo "\n"
	@$(LD) $(RUPEEOBJS) $(LIBS) $(LDFLAGS) $(RUPEEBIN)
	@echo "Binary created in $(RUPEEBIN)\n"
	@$(LD) $(call MkO,brattodrat) $(LIBS) $(LDFLAGS) $(BRATTODRATBIN)
	@echo "\n"

$(call MkO,checker) : $(call MkC,checker) $(call MkH,structs) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,checker) -o $(call MkO,checker)

$(call MkO,clause) : $(call MkC,clause) $(call MkH,structs) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,clause) -o $(call MkO,clause)

$(call MkO,database) : $(call MkC,database) $(call MkH,structs) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,database) -o $(call MkO,database)

$(call MkO,extra) : $(call MkC,extra) $(call MkH,structs) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,extra) -o $(call MkO,extra)

$(call MkO,hashtable) : $(call MkC,hashtable) $(call MkH,structs) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,hashtable) -o $(call MkO,hashtable)

$(call MkO,latency) : $(call MkC,latency) $(call MkH,structs) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,latency) -o $(call MkO,latency)

$(call MkO,model) : $(call MkC,model) $(call MkH,structs) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,model) -o $(call MkO,model)

$(call MkO,parser) : $(call MkC,parser) $(call MkH,structs) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,parser) -o $(call MkO,parser)

$(call MkO,proof) : $(call MkC,proof) $(call MkH,structs) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,proof) -o $(call MkO,proof)

$(call MkO,revision) : $(call MkC,revision) $(call MkH,structs) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,revision) -o $(call MkO,revision)

$(call MkO,recheck) : $(call MkC,recheck) $(call MkH,structs) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,recheck) -o $(call MkO,recheck)

$(call MkO,watchlist) : $(call MkC,watchlist) $(call MkH,structs) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,watchlist) -o $(call MkO,watchlist)

$(call MkO,witness) : $(call MkC,witness) $(call MkH,structs) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,witness) -o $(call MkO,witness)

$(call MkO,rupee) : $(call MkC,rupee) $(call MkH,structs) $(call MkH,extra)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,rupee) -o $(call MkO,rupee)

$(call MkO,brattodrat) : $(call MkC,brattodrat)
	$(CPP) $(LIBS) $(CPPFLAGS) $(call MkC,brattodrat) -o $(call MkO,brattodrat)

clean :
	rm -rfv $(RUPEEOBJS) $(MkO,drattobrat) $(RUPEEBIN) $(BRATTODRATBIN)

directories :
	@mkdir -p bin
	@mkdir -p obj
