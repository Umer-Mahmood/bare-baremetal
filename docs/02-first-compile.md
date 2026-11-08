# Chapter 02 – First Compile & Inspecting the Object File

## Goal

Compile a trivial `main.c` to an object file and prove, with binutils, that the compiler did what the flags asked.

## Questions

1. Did the flags take effect? (`readelf`)
2. Why does the object say `7E-M` and not `cortex-m4`?
3. What does an empty-ish `main` look like in machine code? (`objdump`)
4. What does `-O0` vs `-O1` change?

## Do it

```sh
arm-none-eabi-gcc -c src/main.c -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard
arm-none-eabi-readelf --all main.o            # headers, symbols, attributes
arm-none-eabi-objdump --disassemble main.o
```

`-c` compiles only (no link). Test source: `main` returning `1 + 2`.

## Findings

### Core vs architecture

`readelf` shows `Tag_CPU_name: "7E-M"`, not Cortex-M4.

- **Architecture** (ARMv7E-M): the contract: instruction set, registers, exception model. Specified in the ARMv7-M Architecture Reference Manual (DDI0403). The "E" adds the DSP extensions (SIMD, saturating ops, e.g. `QADD16`, `QADD8`).
- **Core** (Cortex-M4): one implementation of that contract. The Generic User Guide is written about the core, so it rarely names the architecture.
- Toolchains record the architecture, because it decides which instructions are legal and which objects can be linked together.

Other attributes confirmed the flags: `Thumb-2`, `VFPv4-D16`, `Tag_ABI_VFP_args: VFP registers` (= `-mfloat-abi=hard`), `Tag_ABI_HardFP_use: SP only`.

### Machine code

`-O0`:

```
 0: b480       push {r7}
 2: b083       sub  sp, #12
 4: af00       add  r7, sp, #0
 6: 2303       movs r3, #3        ; 1+2 folded at compile time
 8: 607b       str  r3, [r7, #4]
 a: 687b       ldr  r3, [r7, #4]
 c: 4618       mov  r0, r3
 e: 370c       adds r7, #12
10: 46bd       mov  sp, r7
12: f85d 7b04  ldr.w r7, [sp], #4
16: 4770       bx   lr
```

`-O1`:

```
 0: 2003       movs r0, #3
 2: 4770       bx   lr
```

What this shows:

- **Constant folding** happens even at `-O0`: no add instruction exists.
- **AAPCS**: an `int` return value goes in `r0`; return is `bx lr` (`bl` put the return address in `lr`).
- **Thumb-2 is visible in the bytes**: most instructions are 2 bytes, `ldr.w` is 4 bytes (`.w` = wide), and addresses step by 2.
- **`-O0` overhead**: frame pointer (`r7`), and a store/reload through the stack. `-O1` removes all of it.

## Gotchas

- Without `-c`, gcc tries to link and fails or produces something unintended.
- `-mtune` is redundant with `-mcpu`.
- Don't worry that `readelf` doesn't say "cortex-m4" (see above).

## Verify

- `readelf -A main.o` shows `Thumb-2` and the FPU/ABI attributes you asked for.
- Every instruction in `objdump` output is 2 or 4 bytes; none are Arm-state 4-byte opcodes at 4-byte steps with different encodings.

## Sources

- [GCC ARM Options](https://gcc.gnu.org/onlinedocs/gcc-7.1.0/gcc/ARM-Options.html)
- [Cortex-M4 Generic User Guide (DUI0553)](https://support.arm.com/documentation/dui0553/b), section 3.1 *Instruction set summary* (e.g. the DSP instructions `QADD8`, `QADD16`)
- [ARMv7-M Architecture Reference Manual (DDI0403)](https://developer.arm.com/documentation/ddi0403)
- AAPCS (Arm Procedure Call Standard): where `int` return values go (`r0`) and how return works (`bx lr`)

## Open question (next chapter)

Why must bit 0 of the address in `lr` be 1 for `bx lr` on Cortex-M? (Thumb bit / EPSR.T.)
