CXX      = g++
OBJS     = script2asm.o expr2tree.o process_string.o data_type.o identifier_info.o parse_type.o
BINOBJS  = main.o expr2tree_sa.o
BIN      = script2asm.exe expr2tree_sa.exe
CXXFLAGS = -O2 -std=c++98 -Wall -Wextra -Werror-implicit-function-declaration -finput-charset=cp932 -fexec-charset=cp932
LDFLAGS  = -s -static
LIBS     = 
RM       = rm -f

.PHONY: all clean

all: $(BIN)

script2asm.exe: $(OBJS) main.o
	$(CXX) $(LDFLAGS) -o script2asm.exe $(OBJS) main.o $(LIBS)

expr2tree_sa.exe: $(OBJS) expr2tree_sa.o
	$(CXX) $(LDFLAGS) -o expr2tree_sa.exe $(OBJS) expr2tree_sa.o $(LIBS)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c -o main.o main.cpp

expr2tree.o: expr2tree.cpp
	$(CXX) $(CXXFLAGS) -c -o expr2tree.o expr2tree.cpp

process_string.o: process_string.cpp
	$(CXX) $(CXXFLAGS) -c -o process_string.o process_string.cpp

script2asm.o: script2asm.cpp
	$(CXX) $(CXXFLAGS) -c -o script2asm.o script2asm.cpp

data_type.o: data_type.cpp
	$(CXX) $(CXXFLAGS) -c -o data_type.o data_type.cpp

identifier_info.o: identifier_info.cpp
	$(CXX) $(CXXFLAGS) -c -o identifier_info.o identifier_info.cpp

parse_type.o: parse_type.cpp
	$(CXX) $(CXXFLAGS) -c -o parse_type.o parse_type.cpp

expr2tree_sa.o: expr2tree_sa.cpp
	$(CXX) $(CXXFLAGS) -c -o expr2tree_sa.o expr2tree_sa.cpp

clean:
	$(RM) $(BIN) $(OBJS) $(BINOBJS)
