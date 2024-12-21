CC=g++
CCFLAGS = 

DEPS = *.hpp

ODIR=obj
_OBJ=characterClass.o nfa.o nfaFactory.o regexParser.o \
regexTokenizer.o tokenizerParserTypes.o engineTest.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


all: engineTest

debug: CCFLAGS += -g
debug: engineTest

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $(CCFLAGS) $<

engineTest: $(OBJ)
	$(CC) -o $@ $(CCFLAGS) $^

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o engineTest engineTestDebug
