
CXX = g++
CXXFLAGS = -std=c++17 -Wall -pthread -g


COMMON_SRC = thread-pool.cc Semaphore.cc
COMMON_OBJ = $(COMMON_SRC:.cc=.o)


TARGETS = threadpool tptest tpcustomtest

all: $(TARGETS)


threadpool: main.o $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^


tptest: tptest.o $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

tpcustomtest: tpcustomtest.o $(COMMON_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^


%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f *.o $(TARGETS)

.PHONY: all clean
