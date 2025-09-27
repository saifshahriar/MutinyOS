# MutinyOS: A Mini Operating System

MutinyOS is a terminal-based Mini Operating System simulator designed for
students, beginners, and enthusiasts who want to practice shell usage and
explore OS-like utilities without installing a full operating system. It
combines educational shell functionality with entertainment features such as
games and TCP-based multiplayer chat.

---

## Features

- Interactive Shell

  - Built-in commands: help, cmd, cmds, prog, progs, clear, exit, etc.
  - Executes system commands via fork/exec if not recognized as a built-in
    command.

- Built-in Programs

  - Calculator (calc)
  - Date utility (date)
  - Task manager, calendar (planned/future modules)

- Entertainment
  - Console games using ncurses/pdcurses (Snake, Tetris, Pacman, Sudoku, Maze
    Runner, Car Racing)
  - TCP-based multiplayer group chat using CZMQ

---

## Project Structure

```
MutinyOS/
├── Makefile          # Build rules
├── config.mk         # Compiler/linker configuration
├── shell.c           # Main shell loop
├── cmd.c / cmd.h     # Command/program definitions
├── calc.c            # Calculator frontend
├── date.c            # Date/time utility
├── help.c            # Help text
├── lib/
│   ├── tinyexpr.c    # Expression evaluation library
│   └── compatibility.c # Cross-platform helpers
```

---

## Build & Run

# Clone repository

```bash
git clone https://github.com/saifshahriar/MutinyOS.git
cd MutinyOS
```

# Build MutinyOS

```bash
make
```

# Run the shell

```bash
make run
```

# Clean build artifacts

```bash
make clean
```

---

## Usage

Example session:

```fish
saif@mutiny:$> help
saif@mutiny:$> calc
> 2 + 2 = 4

saif@mutiny:$> date
Sat Sep 6 18:00:00 2025
saif@mutiny:$> exit
```

---

## Requirements

- Hardware: Any modern PC (≥ 512 MB RAM)
- Software: Linux or Windows with GCC and Makefile support
- Libraries: ncurses / pdcurses, CZMQ

---

## Limitations & Future Work

- Currently terminal-based; depends on host OS
- No GUI, kernel, or low-level hardware interaction
- Networking limited to TCP in user-space
- Future: custom kernel, native shell, system calls, full OS-level features

---

## Contributors

- Saif Shahriar
- Abdullah Sammo
- Imam Zafor Sadik
- Sozib Hasan

---

## Licence

See [Licence](./LICENSE)

---

## References

- BusyBox: https://busybox.net/
- DOSBox: https://www.dosbox.com/
- Cygwin: https://www.cygwin.com/
- ncurses: https://invisible-island.net/ncurses/
- pdcurses: https://pdcurses.org/
- CZMQ: https://zeromq.org/languages/czmq/
