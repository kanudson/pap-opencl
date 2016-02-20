CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))

LD_FLAGS := `wx-config --libs --cxxflags` -lOpenCL
CXXFLAGS += -std=c++11 `wx-config --libs --cxxflags`
clse:	$(OBJ_FILES)
	g++ -o $@ $^ $(LD_FLAGS)

obj/%.o:	src/%.cpp
	g++ $(CXXFLAGS) -c -o $@ $<

clean:	
	rm -f clse
	rm -rf obj/*
