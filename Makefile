CXX=g++
CXXFLAGS=-std=c++14 -fno-exceptions -ftemplate-depth=512
WFLAGS=-Wall -Wextra -pedantic-errors

BIN=FizzBuzz
SOURCES=$(BIN).cxx

all: $(BIN)
$(BIN):
	$(CXX) $(CXXFLAGS) $(WFLAGS) $(SOURCES) -o $(BIN)

.PHONY: clean
clean:
	$(RM) $(BIN)
