# ShadowGuard

Windows persistence monitoring and protection system.

## Features

* Registry monitoring (Userinit)
* Process scanning
* DLL injection detection
* Windows Service integration

## Build

```bash
make guard      # Build guard.exe
make clean      # Clean build
make install    # Install as service (Admin required)
make run        # Run console mode
```

## Usage

### Console Mode

```bash
# Run as Administrator
guard.exe
```

### Service Mode

```bash
# Install service
guard.exe --install

# Start service
sc start ShadowGuard

# Stop service
sc stop ShadowGuard

# Uninstall service
guard.exe --uninstall
```

## Commands

| Command             | Description                |
| ------------------- | -------------------------- |
| `--install`, `-i`   | Install as Windows service |
| `--uninstall`, `-u` | Remove Windows service     |
| `--service`, `-s`   | Run in service mode        |
| `--help`, `-h`      | Show help                  |

## Detection

| Target    | Detected Patterns                          |
| --------- | ------------------------------------------ |
| Registry  | `loader.exe`, `beacon`, `injector`         |
| Processes | `loader.exe`, `injector.exe`, `beacon.exe` |
| DLLs      | `beacon.dll`, `test.dll`, `payload.dll`    |

## Logs

```text
C:\ProgramData\ShadowGuard\guard.log
```
