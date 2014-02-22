CXX      = g++
OBJS     = expr2tree.o read_one_line.o script2asm.o data_type.o identifier_info.o
BIN      = expr2tree.exe
CXXFLAGS = -O2 -std=c++98 -Wall -Wextra -Werror-implicit-function-declaration -finput-charset=cp932 -fexec-charset=cp932
LDFLAGS  = -s -static
LIBS     = 
RM       = rm -f

.PHONY: all clean

all: $(BIN)

expr2tree.exe: $(OBJS)
	$(CXX) $(LDFLAGS) -o expr2tree.exe $(OBJS) $(LIBS)

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

clean:
	$(RM) $(BIN) $(OBJS)
