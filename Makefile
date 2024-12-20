CC=g++

DEPS = *.hpp

ODIR=obj
_OBJ=characterClass.o nfa.o nfaFactory.o regexParser.o \
regexTokenizer.o tokenizerParserTypes.o engineTest.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $<

engineTest: $(OBJ)
	$(CC) -o $@ $^

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o engineTest
