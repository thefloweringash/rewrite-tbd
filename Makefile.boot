# Please use CMake.

SRCS := main.cpp rewrite.cpp tbdv2.cpp tbdv4.cpp yaml.cpp
OBJS := $(SRCS:.cpp=.o)

CXXFLAGS := -std=c++17 $(shell pkg-config --cflags yaml-0.1)
LDFLAGS := $(shell pkg-config --libs yaml-0.1)

rewrite-tbd: $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

install: rewrite-tbd
	mkdir -p $(PREFIX)/bin
	cp $^ $(PREFIX)/bin
