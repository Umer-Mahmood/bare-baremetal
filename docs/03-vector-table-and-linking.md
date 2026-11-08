# Chapter 03 – Vector Table, Linker Script & Image Formats

## Goal

Produce a linked image whose first 8 bytes are exactly what the Cortex-M4 reads at reset, with code right after it, and convert it to flashable formats.

## Questions

1. What does the core read at reset, and from where?
2. What is the Thumb bit and why does it matter here?
3. How do we get a data object to the start of flash?
4. What do the linked ELF, `.bin` and `.hex` actually contain?

## Findings

### What the core does at reset (Cortex-M4 Generic User Guide, exception model)

| Address | Content |
|---|---|
| `0x00000000` | initial stack pointer **value** (loaded into MSP) |
| `0x00000004` | **address** of the reset handler, **bit 0 = 1** (loaded into PC) |

The table holds values, not code. The code can live anywhere the reset entry points to.

### The Thumb bit

- Thumb instructions are halfword-aligned, so bit 0 of a real instruction address is always 0. Arm reuses it to mean "instruction set state".
- `BX`/`BLX` (and the hardware, on reset and exception entry) split a target value: bits 31:1 are the address, bit 0 becomes the **T bit** in EPSR.
- Cortex-M supports only Thumb, so T must be 1. Bit 0 = 0 means "Arm state", which doesn't exist, so it raises a UsageFault (INVSTATE).
- The Thumb bit applies to values that become the PC (the reset entry, `lr`). **Not** to the stack pointer, and not to the address where code is placed (`.text` at `0x8`, entry value `0x9`).

### Memory (nRF52840 Product Specification, memory chapter)

- Flash: base `0x00000000`, 1 MB.
- RAM: base `0x20000000`, 256 KB = `0x40000`. (Not `0x20000000`–`0x3FFFFFFF`, which is just the architecturally reserved SRAM window.)
- Initial SP = `0x20000000 + 0x40000` = `0x20040000`, one past the last RAM byte, which works because the stack is full-descending and 8-byte aligned.

### The vector table in C

A struct with two fields. GCC attributes (see GCC *Common Variable Attributes*):

- `section(".vector_table")` puts it in a named section.
- `used` stops it being discarded.

The section name only labels it. **Placement is the linker script's job.** In an object file every section starts at 0 (`objdump -h`).

### Linker script (`src/linker.ld`)

```
SECTIONS
{
    . = 0x00000000;
    .vector_table : {*(.vector_table)} /* Interrupt vector table */
    . = 0x00000008;
    .text : {*(.text)}
    .data : {*(.data)}
    .bss  : {*(.bss)}
}
```

- `.` is the location counter, an **address**. Setting it to `0x9` (with the Thumb bit) was wrong: that is a value for the vector entry, not a placement address.
- Reference: [GNU ld manual](https://sourceware.org/binutils/docs/ld/), SECTIONS page. Comments are `/* */`, not `//`.
- Known limitation: no `MEMORY` regions yet, so `.data`/`.bss` are not in RAM. Fixed in a later chapter.

## Do it

```sh
arm-none-eabi-ld main.o --script src/linker.ld            # -> a.out (ELF)
arm-none-eabi-objdump a.out --full-contents
arm-none-eabi-objcopy --output-target ihex   a.out firmware.hex
arm-none-eabi-objcopy --output-target binary a.out firmware.bin
xxd -g 4 -c 4 firmware.bin
```

## Findings: what's in the image

```
00000000: 00000420    initial SP  = 0x20040000 (little-endian bytes 00 00 04 20)
00000004: 09000000    reset entry = 0x00000009 (0x8 + Thumb bit)
00000008: 03207047    movs r0,#3 ; bx lr
```

Intel hex records:

```
:08 0000 00 0000042009000000 CB    8 data bytes at 0x0000, type 00 (data), checksum
:04 0008 00 03207047 1A            code at 0x0008
:04 0000 03 00000008 F1            type 03: start address = ELF entry point (0x8), ignored by the core
:00 0000 01 FF                     end of file
```

- Checksum = two's complement of the sum of all preceding bytes of the record (`0x100 - 0x35 = 0xCB`).
- The entry point is the start of `.text` by default (no `_start` symbol). The core ignores it; loaders and debuggers use it.
- Endianness: `objcopy --info` shows `ihex` as "endianness unknown". That's fine: the format is a byte stream and never interprets multi-byte values. Bytes carry through in memory order.
- A `.bin` has no addresses at all. Convenient for `xxd`, but the flasher must be told the base address.

## Gotchas

- `objdump --disassemble` skips data sections. Use `--full-contents` (`-s`) for data. `--disassemble-all` shows data as bogus `<UNDEFINED>` instructions.
- Hard-coding the reset entry `0x9` only works while `.text` starts at `0x8` and `main` is first. Later: use the function's name so the linker fills in the address (with the Thumb bit).
- KB means 1024: `0x3E800` (256,000) is not 256 KB.
- A hex digit missing from `0x20040000` (`0x2004000`) silently put the SP in the wrong region. Check values with `--full-contents`.

## Verify

- `.vector_table` at `0x0`, `.text` at `0x8` in the linked file.
- First word `0x20040000`, second word odd.

## Sources

- [Cortex-M4 Generic User Guide (DUI0553)](https://support.arm.com/documentation/dui0553/b): 2.3.4 *Vector table*; 2.1.3 *Core registers* (Program Status Register, T bit); the `BX` instruction (section 3.1 *Instruction set summary* and the branch instruction pages)
- [nRF52840 Product Specification v1.11](https://docs.nordicsemi.com/r/bundle/ps_nrf52840/page/keyfeatures_html5.html): 4.2.3 *Memory map* for RAM and flash base addresses and sizes
- [GCC Common Attributes](https://gcc.gnu.org/onlinedocs/gcc/Common-Attributes.html): `section`, `used`
- [GNU ld manual](https://sourceware.org/binutils/docs/ld/): `SECTIONS`, location counter, comment syntax
- Intel HEX record format

## Next

Flash it with `nrfjprog` and verify memory contents on the target.
