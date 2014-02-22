CXX      = g++
OBJS     = expr2tree.o read_one_line.o data_type.o identifier_info.o
BINOBJS  = script2asm.o expr2tree_sa.o
BIN      = script2asm.exe expr2tree_sa.exe
CXXFLAGS = -O2 -std=c++98 -Wall -Wextra -Werror-implicit-function-declaration -finput-charset=cp932 -fexec-charset=cp932
LDFLAGS  = -s -static
LIBS     = 
RM       = rm -f

.PHONY: all clean

all: $(BIN)

script2asm.exe: $(OBJS) script2asm.o
	$(CXX) $(LDFLAGS) -o script2asm.exe $(OBJS) script2asm.o $(LIBS)

expr2tree_sa.exe: $(OBJS) expr2tree_sa.o
	$(CXX) $(LDFLAGS) -o expr2tree_sa.exe $(OBJS) expr2tree_sa.o $(LIBS)

expr2tree.o: expr2tree.cpp
	$(CXX) $(CXXFLAGS) -c -o expr2tree.o expr2tree.cpp

read_one_line.o: read_one_line.cpp
	$(CXX) $(CXXFLAGS) -c -o read_one_line.o read_one_line.cpp

script2asm.o: script2asm.cpp
	$(CXX) $(CXXFLAGS) -c -o script2asm.o script2asm.cpp

data_type.o: data_type.cpp
	$(CXX) $(CXXFLAGS) -c -o data_type.o data_type.cpp

identifier_info.o: identifier_info.cpp
	$(CXX) $(CXXFLAGS) -c -o identifier_info.o identifier_info.cpp

expr2tree_sa.o: expr2tree_sa.cpp
	$(CXX) $(CXXFLAGS) -c -o expr2tree_sa.o expr2tree_sa.cpp

clean:
	$(RM) $(BIN) $(OBJS) $(BINOBJS)
