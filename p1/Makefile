#mastlouis

CXX = gcc

CXXFLAGS = -g -Wall

TARGET = mc2 mc1 mc0

all:	$(TARGET)

clean: 
	rm mc2 mc1 mc0

mc2: mc2.c mc2.h
	$(CXX) $(CXXFLAGS) -o mc2 mc2.c

mc1: mc1.c mc1.h
	$(CXX) $(CXXFLAGS) -o mc1 mc1.c

mc0: mc0.c mc0.h
	$(CXX) $(CXXFLAGS) -o mc0 mc0.c

