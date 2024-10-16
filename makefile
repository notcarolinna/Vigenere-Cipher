TARGET = Cipher

CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

SRC = Vigenère.cpp
OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

# Regra para rodar o programa (opcional)
run: $(TARGET)
	./$(TARGET)