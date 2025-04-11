# Multithreaded Shell & Parallel For Loop (C/C++ Systems Project)

## Overview

This project consists of two components:

1. **A multithreaded `parallel_for` library** for executing 1D and 2D loops in parallel using POSIX threads and lambda expressions (C++).
2. **A custom interactive shell** (`SimpleShell`) that executes user-submitted commands with priority-based scheduling, leveraging multi-core support and semaphores (C).

---

## 🔧 Features

### parallel_for (C++)
- Custom parallel loop functions for 1D and 2D loops.
- Uses C++11 lambda expressions for flexible task execution.
- Measures and prints total execution time.
- Demonstrates thread-safe lambda execution using POSIX `pthread`.

### SimpleShell (C)
- Accepts user commands like `submit <cmd> <priority>` and `history`.
- Executes commands based on priority (1 = highest, 4 = lowest).
- Supports multi-core simulation with `NCPU` and time slice arguments.
- Handles process output, wait times, and clean exit via `Ctrl+C`.
- Tracks process history and displays detailed summary upon exit.

---
