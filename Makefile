TARGET = arduino-indicator
PREFIX ?= /usr/local
SRCS = $(TARGET).cpp
OBJS = $(SRCS:.cpp=.o)
CXXFLAGS := -std=c++17 -Os

.PHONY: all clean install uninstall

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(CXXFLAGS)

.cpp.o:
	$(CXX) $(CXXFLAGS)  -c $< -o $@

clean:
	rm -rf $(TARGET) $(OBJS)

install:
	install $(TARGET) $(PREFIX)/bin

uninstall:
	rm -rf $(PREFIX)/bin/$(TARGET)
