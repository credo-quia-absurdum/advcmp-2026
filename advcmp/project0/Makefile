CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic -Wconversion -Wsign-conversion -Werror

TARGET := load_constant
SRCS := main.cpp sim.cpp load_constant.cpp
OBJS := $(SRCS:.cpp=.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.cpp sim.h load_constant.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)
