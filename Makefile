CXXC = clang
CXXFLAGS = -Wall -Wextra -lstdc++ -std=c++17 -I/usr/include/libxml2 -DMULTITHREADING -O3

SRC_DIR = src
FILES = main.cpp $(SRC_DIR)/tokenizer.cpp $(SRC_DIR)/xml-parser.cpp $(SRC_DIR)/dictionary.cpp $(SRC_DIR)/engine.cpp
LIBS = libs/libstemmer.a -lxml2 -lm 

TARGET = se

all: $(TARGET)

$(TARGET): $(FILES)
	$(CXXC) $(CXXFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET) *.out
