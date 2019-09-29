CXX = g++
CXXFLAGS = -Wall -Werror -Wextra -fPIC -DBRANCHING_FACTOR -std=c++17 -I. -Ofast
SOURCE = play.cpp
OBJECTS = play.o
TARGET = play libplay.so
DEPS = mancala.h search.h

default: $(TARGET)

%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

play: $(OBJECTS)
	$(CXX) $< -o $@

libplay.so: $(OBJECTS)
	$(CXX) -shared -Wl,-soname,libplay.so -o libplay.so $<
	@echo "Build complete!"

clean:
	rm -f $(OBJECTS) $(TARGET)


