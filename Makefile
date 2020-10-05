CXX=g++
CXXFLAGS=-std=c++17 `wx-config-gtk3 --cxxflags`
LDFLAGS=-lcurl `wx-config-gtk3 --libs`

all: ratptt

ratptt: src/main.o
	$(CXX) src/main.o -o ratptt $(LDFLAGS)

src/main.o: src/main.cc
	$(CXX) $(CXXFLAGS) -c src/main.cc -o src/main.o

clean:
	$(RM) ratptt src/main.o
