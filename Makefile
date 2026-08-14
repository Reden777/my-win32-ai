CXX := /mingw64/bin/g++.exe
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -D_WIN32_WINNT=0x0600
TARGET := native-ai.exe
SOURCE := chatgpt-cpp/main.cpp

.PHONY: all clean run
all: $(TARGET)

$(TARGET): $(SOURCE) chatgpt-cpp/json.h
	$(CXX) $(CXXFLAGS) -mwindows -municode -static -o $@ $(SOURCE) -lcomdlg32 -lwinhttp -lcrypt32

run: $(TARGET)
	./$(TARGET)

clean:
	$(RM) $(TARGET)
