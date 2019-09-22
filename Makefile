CXX = g++
CXXFLAGS = -pthread -g -std=c++17 -I.
LDFLAGS = -pthread
SOURCE = $(wildcard *.cpp)
OBJECTS = $(SOURCE:.cpp=.o)
TARGET = play
DEPS = $(wildcard *.h)

default: $(TARGET)

%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(LDFLAGS)

play: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJECTS) $(TARGET)


