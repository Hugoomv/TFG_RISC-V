TFG: RISC-V Simulator using System-C (C++)

# RISC-V Simulator in SystemC

A modular RISC-V simulator built using **SystemC** (C++) at RTL level. This project focuses on simulating the **RV32** base integer instruction set, including **floating-point operations (F extension, 32-bit only)**. It features a clean architecture with separate modules for the **ALU**, **multiplier**, and **floating-point unit (FPU)**.

## Features

- **Registers**: Implements 32 general-purpose registers and floating-point registers .
- **ALU Module**: Supports arithmetic, logical, shift, and comparison operations.
- **MUL Module**: Handles multiplication instructions (e.g., `MUL`, `DIV`, `REMU`).
- **Floating-Point Unit**: Executes 32-bit (single-precision) floating-point instructions (e.g., `FMV.X.S`, `FCVT.W[U].S`).
- **Modular Design**: Each functional unit is implemented as an independent SystemC module.
- **Instruction Execution**: Supports a variety of RV32I, RV32M and RV32F instructions.
- **Simulation Environment**: Designed to be easily extensible for more RISC-V extensions or pipeline stages.
- **Dependencies Managment**: Includes a system to detect and solve dependencies between intructions and modules.
- **Variable Latency**: Latency in the MUL instructions can be changed without limits. 

## Requirements

- C++17 or later
- SystemC
- Visul Studio (optional but recommended)
