# Chapter 04 – Flashing and First Boot

## Goal

Flash the image onto the nRF52840 and prove from the CPU registers that it booted the way the vector table says it should.

## Questions

1. What happens if `main` returns straight after reset?
2. Why did flashing a second image fail verification?
3. How can we confirm the core is running our code without an LED?

## Do it

```sh
nrfjprog --program firmware.bin --verify --family NRF52   # + an erase option, see Gotchas
nrfjprog --memrd 0x00000000 --w 32 --n 12                 # read flash back as words
nrfjprog --readregs                                       # CPU registers
nrfjprog --reset
```

A `.bin` carries no addresses, so `nrfjprog` programs it at `0x0`.

## Findings

### Returning from `main` at reset

`lr` resets to `0xFFFFFFFF`. `bx lr` keeps bit 0 (T=1) and jumps to `0xFFFFFFFE`, which cannot be executed and matches the exception-return range, so the core faults.

The HardFault entry (`0xC`) in our 8-byte-table image reads erased flash (`0xFFFFFFFF`), so the handler fetch faults too, and the core ends up in **lockup**. Observed afterwards: `PC=0xFFFFFFFE`, `SP=0xFFFFFFFC`.

Also note the table is longer than 2 entries: `0x8` is NMI, `0xC` is HardFault. Our code bytes at `0x8` were being treated as the NMI vector.

**A reset handler has no caller, so it must never return.** `while (1);` compiles to a single `b.n 0` (`e7fe`).

### Flash cannot be overwritten without erasing

Verify failed: *expected `0xFE` read `0x02` at `0x8`*. The old byte there was `0x03`, and `0x03 & 0xFE = 0x02`.

Flash writes can only clear bits (1 → 0). Setting bits back to 1 needs an erase (see the NVMC chapter of the Product Specification). Erase first (e.g. `--sectorerase`, `--chiperase`; see `nrfjprog --help`).

### Proof of boot (`--readregs` after reset)

| Register | Value | Why |
|---|---|---|
| `SP` / `MSP` | `0x20040000` | first vector table word, loaded by hardware |
| `PC` | `0x00000008` | reset entry `0x9`: bits 31:1 = address, bit 0 → T |
| `LR` | `0xFFFFFFFF` | architectural reset value |
| `xPSR` | `0x01000000` | bit 24 = **T bit**, set |

`xPSR` is the combined view of three parts of one register: **APSR** (flags), **IPSR** (exception number), **EPSR** (execution state, including T). The Generic User Guide documents them separately.

## Gotchas

- `nrfjprog --program` does not erase by default. Add an erase option on re-flash.
- Flash looks fine in `--memrd` as little-endian words: `47702003` is the bytes `03 20 70 47`.
- `--readregs` before a reset shows stale state from the previous run.

## Verify

- `--memrd` matches the `.bin` (`xxd -g 4 -c 4 firmware.bin`).
- After `--reset`: `SP=0x20040000`, `PC` inside your loop, `xPSR` bit 24 set.

## Sources

- [Cortex-M4 Generic User Guide (DUI0553)](https://support.arm.com/documentation/dui0553/b): 2.1.3 *Core registers* (reset values, Program Status Register and the T bit); 2.3.4 *Vector table* (NMI at `0x8`, HardFault at `0xC`)
- [nRF52840 Product Specification v1.11](https://docs.nordicsemi.com/r/bundle/ps_nrf52840/page/keyfeatures_html5.html): the NVMC chapter (flash write and erase)
- `nrfjprog --help`

## Next

Blink the LED: GPIO registers (`DIR`, `OUT`), delays, and the first real startup code.
