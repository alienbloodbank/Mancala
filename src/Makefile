CXX = g++
CXXFLAGS = -pthread -std=c++17 -I.
LDFLAGS = -pthread
SOURCE = play.cpp
OBJECTS = play.o
TARGET = play
DEPS = mancala.h search.h

default: $(TARGET)

%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(LDFLAGS)

play: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)
	@echo "Build complete!"

clean:
	rm -f $(OBJECTS) $(TARGET)


