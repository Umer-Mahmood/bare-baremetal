# Chapter 01 – Toolchain & Compiler Flags

## Goal

Have a working `arm-none-eabi-gcc` and know *why* each target flag is chosen, so the compiler produces code this exact chip can run.

## Questions

1. What core is inside the nRF52840, and what can it execute?
2. Which compiler flags express that?
3. Where does the compiler come from? (Not from SEGGER Embedded Studio.)

## Findings

### The core (nRF52840 Product Specification, Cortex-M4 section)

| Fact | Value | Flag |
|---|---|---|
| Core | Arm Cortex-M4 | `-mcpu=cortex-m4` |
| Instruction set | Thumb-2 only (mixed 16/32-bit), **no Arm state** | `-mthumb` |
| FPU | single-precision, FPv4-SP | `-mfpu=fpv4-sp-d16` |
| Endianness | little-endian | `-mlittle-endian` (GCC default for this target) |

### The flags

Reference: [GCC ARM Options](https://gcc.gnu.org/onlinedocs/gcc-7.1.0/gcc/ARM-Options.html)

```
-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=<softfp|hard>
```

- `-mcpu` implies tuning, so `-mtune` is redundant.
- `-mthumb` is essential. Without it GCC emits Arm-state (A32) opcodes. A Cortex-M has no Arm state, so the first such instruction would fault. This is also why the reset vector's least significant bit (the Thumb bit) matters (see the vector table chapter).
- `-mfloat-abi`: both `softfp` and `hard` run on this hardware.
  - `softfp`: FPU instructions, but floats are passed in integer registers.
  - `hard`: floats are passed in FPU registers (faster).
  - Every object file and library in one link must use the same ABI.
  - **Decision: TBD** (revisit at link time).

### The compiler

SEGGER Embedded Studio ships its own Clang-based toolchain and does not put `arm-none-eabi-gcc` on `PATH`. Install the GNU one separately.

## Do it (Windows, MSYS2)

```sh
pacman -Ss arm-none-eabi    # -S = sync database, s = search; find the package name
pacman -S <package-name>    # install it
```

Add the MSYS2 environment's `bin` directory (e.g. `C:\msys64\mingw64\bin`) to the Windows `PATH`, so the tool is found outside the MSYS2 shell too.

## Gotchas

- Run `pacman` from an MSYS2 shell, not PowerShell.
- MSYS2 package names carry an environment prefix (e.g. `mingw-w64-...`).
- `gcc` without `-c` compiles *and links*. For now we only want to compile.

## Verify

- `arm-none-eabi-gcc --version` prints a version, from a fresh terminal (confirms `PATH`).

## Sources

- nRF52840 Product Specification v1.11 (`docs/nRF52840_PS_v1.11.pdf`)
- Arm Cortex-M4 Devices Generic User Guide DUI0553 (`docs/`)
- GCC ARM Options (link above)

## Next

[Chapter 02 – First compile](02-first-compile.md)
