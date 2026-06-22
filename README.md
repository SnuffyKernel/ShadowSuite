# ShadowSuite

A Windows red team tool suite for educational and authorized penetration testing purposes.

## ⚠️ Disclaimer

**This software is for educational and research purposes only.**

* Use only on systems you own or have explicit written permission to test
* Unauthorized use is illegal
* The authors assume no liability for misuse

## Components

### [ShadowInjection](./ShadowInjection/)

C2 beacon with DLL injection into explorer.exe.

**Features:**

* HTTP C2 communication
* Process injection
* Userinit persistence
* Remote command execution
* Python C2 server included

### [ShadowGuard](./ShadowGuard/)

Persistence monitoring and protection system.

**Features:**

* Registry monitoring
* Process scanning
* DLL injection detection
* Windows Service integration

## Quick Start

### Start C2 Server

```bash
cd ShadowInjection
python c2.py
```

### Build and Run Injector

```bash
cd ShadowInjection
make all
make run    # Run as Administrator
```

### Build and Run Guard

```bash
cd ShadowGuard
make guard
make install    # Install as service (Admin required)
```

## Project Structure

```text
ShadowSuite/
├── ShadowInjection/          # Attack tool
│   ├── src/                  # Source code
│   ├── build/                # Compiled binaries
│   ├── c2.py                 # Python C2 server
│   └── Makefile
├── ShadowGuard/              # Defense tool
│   ├── src/                  # Source code
│   ├── build/                # Compiled binaries
│   └── Makefile
├── LICENSE
└── README.md
```

## Requirements

* Windows 7 / 8 / 10 / 11
* MinGW or MSVC
* Python 3 (for C2 server)
* Administrator privileges

## License

MIT License — see `LICENSE` file for details.

## Authors

* **SnuffyKernel** — GitHub
* **SpongebobGatik** — GitHub

