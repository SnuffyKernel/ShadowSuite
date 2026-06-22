CC = g++
CFLAGS = -static-libgcc -static-libstdc++ -O2 -s -mwindows
LDFLAGS = -lwinhttp -lws2_32
BUILD = build
SRC = src

all: folders beacon_dll beacon_loader injector

folders:
	@if not exist $(BUILD) mkdir $(BUILD)

common:
	$(CC) -c $(SRC)/common.cpp -o $(BUILD)/common.o $(CFLAGS)

beacon_dll: common
	$(CC) -shared -o $(BUILD)/beacon.dll $(SRC)/beacon.cpp $(BUILD)/common.o $(CFLAGS) $(LDFLAGS) -Wl,--kill-at

beacon_loader: common
	$(CC) -o $(BUILD)/loader.exe $(SRC)/beacon_loader.cpp $(BUILD)/common.o $(CFLAGS) $(LDFLAGS)

injector: common
	$(CC) -o $(BUILD)/injector.exe $(SRC)/injector.cpp $(BUILD)/common.o $(CFLAGS) $(LDFLAGS)

clean:
	@if exist $(BUILD) rmdir /s /q $(BUILD)

rebuild: clean all

run: all
	cd $(BUILD) && injector.exe

help: