# nrf52840-from-scratch

This repository is an experimental and educational journey into programming the **Nordic nRF52840** microcontroller **without any SDK or IDE** — just datasheets, the ARM Cortex-M4 reference manual, and the GNU Arm toolchain.

The goal is to **strip away the abstractions** and show:

1. The **bare minimum application**: one C file, linker script, startup file, and Makefile. Example: blinking an LED.
2. The **translation pipeline**: C code → ARM assembly → ARM Cortex-M4 instruction set → machine opcodes → final binary.  
   - We'll map **C functions** directly into their **hex representation**, highlighting bytes in the final `.hex` file that correspond to specific instructions.

---

## Why?

Most embedded projects rely on SDKs and HALs. This hides the beauty of how a C line like:

```c
int add(int a, int b) { return a + b; }
```

turns into assembly:

```
adds r0, r0, r1   ; opcode 0x1808
```

and finally into **bytes in flash memory** that run on the Cortex-M4.

---

## Project Roadmap

### ✅ Phase 1: Environment Setup
- [ ] Install GNU Arm Embedded Toolchain (`arm-none-eabi-gcc`)
- [ ] Prepare a minimal linker script for nRF52840
- [ ] Write startup code and reset handler
- [ ] Makefile to build and generate `.elf`, `.bin`, `.hex`

### ✅ Phase 2: Bare Minimum Blinky
- [ ] Single `main.c` toggling an LED using GPIO registers (no SDK)
- [ ] Build → flash → verify LED blinks

### ✅ Phase 3: From C to Hex
- [ ] Write a simple C function (`add_two_numbers`)
- [ ] Compile with `-S` to generate assembly
- [ ] Use `objdump` to see instructions + opcodes
- [ ] Highlight how `adds r0, r0, r1` → `0x1808` in final hex file
- [ ] Create a visual map: C code → asm → machine code → flash memory

### ✅ Phase 4: Documentation & Educational Material
- [ ] Step-by-step README with diagrams
- [ ] Add references to ARMv7-M architecture manual and nRF52840 datasheet
- [ ] Provide hex dumps with annotated bytes

---

## Tools Used
- **Compiler**: GNU Arm Embedded Toolchain (`arm-none-eabi-gcc`)
- **Flashing**: OpenOCD / pyOCD / nrfjprog (if you have a DK)
- **Disassembly**: `arm-none-eabi-objdump -d`
- **Hex Inspection**: `xxd`, `hexdump`, or any hex editor

---

## References
- [ARMv7-M Architecture Reference Manual](https://developer.arm.com/documentation/ddi0403)
- [Nordic nRF52840 Product Specification](https://infocenter.nordicsemi.com/pdf/nRF52840_PS_v1.1.pdf)
- [GNU Arm Embedded Toolchain](https://developer.arm.com/downloads/-/gnu-rm)

---

## License
MIT License
