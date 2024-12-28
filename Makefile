CC=g++
CCFLAGS = 

DEPS = *.hpp

ODIR=obj
_OBJ=characterClass.o nfa.o nfaFactory.o regexParser.o \
regexTokenizer.o tokenizerParserTypes.o homebrewRegex.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


all: engineTest

debug: CCFLAGS += -g
debug: engineTest

staticlib: $(OBJ)
	ar rcs libhomebrewRegex.a $^

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $(CCFLAGS) $<

engineTest: $(OBJ) engineTest.o
	$(CC) -o $@ $(CCFLAGS) $^

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o engineTest libhomebrewRegex.a
