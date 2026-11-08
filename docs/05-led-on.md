# Chapter 05 – LED On: Talking to GPIO Registers

## Goal

Drive the LED on P0.11 by writing GPIO registers directly, and debug it by reading the hardware state back.

## Questions

1. Where are the GPIO registers, and which do we need?
2. Is the LED active-high or active-low?
3. The LED doesn't light: is it the hardware, the code, or the build?

## Findings

### Registers (nRF52840 PS v1.11, GPIO chapter)

| Register | Address | Meaning |
|---|---|---|
| P0 base | `0x50000000` | GPIO port 0 |
| `OUT` | base + `0x504` | output level of each pin (bit n = P0.n) |
| `DIR` | base + `0x514` | direction: 1 = output |

Peripheral registers are memory-mapped, so C reaches them through a `volatile` pointer. `volatile` stops the compiler from caching or removing the accesses.

Setting or clearing **one** pin without disturbing the rest is a read-modify-write: `|= mask` sets a bit, `&= ~mask` clears it.

### Polarity (board schematic)

```
P0.11 ---[R]---|<|--- VBAT
```

Current flows from VBAT through the LED and resistor **into** the pin, so the pin must **sink** current: drive it **low** to light the LED. Driving it high puts both ends at about the same voltage, so nothing flows.

### Minimal code

```c
#define P0_OUT (0x50000000 + 0x504)
#define P0_DIR (0x50000000 + 0x514)
#define LED_PIN 11

int main(){
    *(volatile unsigned int *)P0_DIR |=  (1 << LED_PIN);  // output
    *(volatile unsigned int *)P0_OUT &= ~(1 << LED_PIN);  // low = LED on
    while (1);                                            // never return
}
```

(Shown tidied: the repo version uses named pointers.)

## Debugging story

No reaction. Instead of guessing, read the registers back from the running chip:

```sh
nrfjprog --memrd 0x50000504   # OUT -> 00000800  (bit 11 set)
nrfjprog --memrd 0x50000514   # DIR -> 00000000  (nothing is an output)
```

That pair is exactly what the **first version** of the code (set `OUT` only, never `DIR`) produces. The chip was running a stale image. The manual chain `gcc -c` → `ld` → `objcopy` → `nrfjprog` had a step that wasn't re-run. After rebuilding every stage and re-flashing, the LED lit.

## Gotchas

- Writing `OUT` has no visible effect on a pin that isn't configured as an output (`DIR`).
- Writing `OUT` high on an active-low LED turns it **off**. Check polarity in the schematic.
- A multi-step manual build makes stale artifacts easy. Compare timestamps (`ls -l --time-style=full-iso`) or, better, automate with `make`.
- Always erase before re-flashing (see chapter 04).

## Verify

- `--memrd 0x50000514` has bit 11 set; `--memrd 0x50000504` has bit 11 clear.
- The LED is lit.

## Sources

- nRF52840 PS v1.11 (GPIO chapter and register table); board schematic.

## Next

Blink it (a delay without a timer), then replace the hand-run commands with a Makefile and start moving the hard-coded `0x9` over to a symbol.
