CXX := g++
TARGET := plot_waveform
SRC := plot_waveform_rdf.C

ROOTFLAGS := $(shell root-config --cflags)
ROOTLIBS := $(shell root-config --libs)

CXXFLAGS := -O2 -Wall -Wextra -std=c++17 $(ROOTFLAGS)
LDFLAGS := $(ROOTLIBS)

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean
