CC = g++
CFLAGS = -static-libgcc -static-libstdc++ -O2 -s
LDFLAGS = -lpsapi -Wl,-s
BUILD = build
SRC = src

all: folders guard

folders:
	@if not exist $(BUILD) mkdir $(BUILD)

guard:
	$(CC) -o $(BUILD)/guard.exe $(SRC)/main.cpp $(SRC)/service.cpp $(SRC)/guard.cpp $(CFLAGS) $(LDFLAGS)

clean:
	@if exist $(BUILD) rmdir /s /q $(BUILD)

rebuild: clean all

install: all
	cd $(BUILD) && guard.exe --install

uninstall:
	cd $(BUILD) && guard.exe --uninstall

run: all
	cd $(BUILD) && guard.exe