#mastlouis #fxwhittall

CXX = gcc

CXXFLAGS = -g -Wall

TARGET = part2 part1

all:	$(TARGET)

clean:
	rm $(TARGET)

part2: part2.c
	$(CXX) $(CXXFLAGS) -pthread -o part2 part2.c -lm

part1: part1.c
	$(CXX) $(CXXFLAGS) -pthread -o part1 part1.c -lm