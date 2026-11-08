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

## Who this is for

Developers who know C and have done some embedded work, but only through SDKs, HALs or RTOSes (nRF Connect SDK, Zephyr, ...) and never stopped to ask: *wait, how does this actually work?* Each chapter starts from a question, shows what the SDK normally hides, and proves the answer with tools (`objdump`, `readelf`, `nm`, the map file).

## Chapters

Each chapter follows the same shape: Goal, Questions, Findings, Do it, Gotchas, Verify, Sources.

1. [Toolchain & compiler flags](docs/01-toolchain-setup.md)
2. [First compile & inspecting the object file](docs/02-first-compile.md)
3. [Vector table, linker script & image formats](docs/03-vector-table-and-linking.md)
4. [Flashing and first boot](docs/04-first-boot.md)
5. [LED on: GPIO registers](docs/05-led-on.md)
6. [Blink: a delay without a timer](docs/06-blink.md)

Target hardware: a custom nRF52840 board, programmed with a SEGGER J-Link via `nrfjprog`.

---

## Project Roadmap

### Phase 1: Environment Setup
- [x] Install GNU Arm toolchain (`arm-none-eabi-gcc`) and pick target flags ([ch. 1](docs/01-toolchain-setup.md))
- [x] Compile and inspect an empty `main.c` ([ch. 2](docs/02-first-compile.md))
- [x] Minimal linker script and vector table ([ch. 3](docs/03-vector-table-and-linking.md))
- [x] Flash and prove first boot via CPU registers ([ch. 4](docs/04-first-boot.md))
- [ ] Write startup code and reset handler
- [ ] Makefile to build and generate `.elf`, `.bin`, `.hex`

### Phase 2: Bare Minimum Blinky
- [x] LED on via GPIO registers, no SDK ([ch. 5](docs/05-led-on.md))
- [x] Blink (delay without a timer) ([ch. 6](docs/06-blink.md))
- [x] Build → flash → verify LED blinks

### Phase 3: From C to Hex
- [ ] Write a simple C function (`add_two_numbers`)
- [ ] Compile with `-S` to generate assembly
- [ ] Use `objdump` to see instructions + opcodes
- [ ] Highlight how `adds r0, r0, r1` → `0x1808` in final hex file
- [ ] Create a visual map: C code → asm → machine code → flash memory

### Phase 4: Documentation & Educational Material
- [ ] Step-by-step README with diagrams
- [ ] Add references to ARMv7-M architecture manual and nRF52840 datasheet
- [ ] Provide hex dumps with annotated bytes

---

## Tools Used
- **Compiler**: GNU Arm toolchain (`arm-none-eabi-gcc`), installed on Windows through MSYS2 (`pacman`)
- **Binutils**: `arm-none-eabi-ld` (linker), `objcopy` (ELF to `.bin`/`.hex`), `objdump` and `readelf` (inspection)
- **Programmer**: SEGGER J-Link, driven by `nrfjprog` (Nordic nRF Command Line Tools)
- **Hex inspection**: `xxd`
- **Build**: [`tools/build-and-flash.sh`](tools/build-and-flash.sh) for now (a Makefile is planned). Its `nrfjprog --eraseall` wipes whatever chip is connected.

Target: a custom nRF52840 board. The LED pin (P0.11) and its polarity (active low) are specific to that board, so adapt `LED_PIN` and the output level to yours.

---

## References

Documents used, with the sections each chapter relies on. The PDFs are not stored in this repo; download them from the official sites.

**nRF52840 Product Specification v1.11** ([online](https://docs.nordicsemi.com/r/bundle/ps_nrf52840/page/keyfeatures_html5.html); for the PDF, open the page, go to *Attachments* and download `nRF52840_PS_v1.11.pdf`)
- 4.2.3 *Memory map*: RAM and flash base addresses and sizes
- 6.9 *GPIO – General purpose input/output*
  - 6.9.2 *Registers*, table "Instances": GPIO port base addresses
  - 6.9.2.1 *OUT* and 6.9.2.5 *DIR*: the registers used to drive the LED

**Arm Cortex-M4 Devices Generic User Guide (DUI0553)** ([download](https://support.arm.com/documentation/dui0553/b))
- 2.1.3 *Core registers*: especially the Program Status Register and the T bit
- 2.3.4 *Vector table*: layout of the table the core reads at reset
- 3.1 *Instruction set summary*

**GCC documentation**
- [ARM Options](https://gcc.gnu.org/onlinedocs/gcc-7.1.0/gcc/ARM-Options.html): compiler flags for the target (`-mcpu`, `-mthumb`, `-mfpu`, `-mfloat-abi`)
- [Common Attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Attributes.html): `section`, `used`

**Other**
- [ARMv7-M Architecture Reference Manual](https://developer.arm.com/documentation/ddi0403)
- [GNU ld manual](https://sourceware.org/binutils/docs/ld/)
- [Arm GNU Toolchain downloads](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)

---

## License
MIT License
