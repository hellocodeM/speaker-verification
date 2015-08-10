CXX = clang++
CXX_FLAGS = -std=c++14 -g
SHARED_LIB = -L/usr/local/lib -lAquila -lOoura_fft
TARGET = noise-reduction

all: $(TARGET).cc
	$(CXX) $(CXX_FLAGS) -o $(TARGET) $(SHARED_LIB) $(TARGET).cc

clean: 
	-rm $(TARGET)
