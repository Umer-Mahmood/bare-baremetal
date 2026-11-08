# Chapter 06 – Blink: A Delay Without a Timer

## Goal

Toggle the LED forever, with a software delay and no timer peripheral.

## Questions

1. How long does the core take to do "nothing", and at what clock?
2. What happens to an empty delay loop when the compiler optimises?
3. How do we flip one pin without knowing its current state?

## Findings

### Timing

After reset the core runs at 64 MHz (nRF52840 PS, clock chapter), so one second is about 64 million cycles. One loop iteration costs several cycles (increment, compare, branch, plus whatever the body does), so the iteration count for a given delay is a **fraction** of the cycle count. Tune it by eye, or measure with a scope. This is a rough delay, not a time base. Flash wait states, caching and optimisation level all change it.

### The compiler removes loops that do nothing

The compiler only has to preserve **observable behaviour**. A counting loop with an empty body has none, so at `-O1` and above it can be deleted. Make the body observable: reading or writing a `volatile` object counts as observable, and so does a memory-mapped register read.

In this repo the loop body reads the `volatile` GPIO `OUT` register each iteration, so the loop survives optimisation.

### Toggling

`*out_reg ^= (1 << LED_PIN)` flips bit 11 and leaves the other pins alone, whatever the current state. Like `|=` and `&= ~`, it's a read-modify-write on the register.

### Structure (`src/main.c`)

```
configure DIR (pin as output)
loop forever:
    toggle OUT bit 11
    spin ~N iterations
```

## Do it

Rebuild every stage, erase, flash, reset (see chapter 04). The LED blinks.

## Gotchas

- An optimised build can silently delete an empty delay loop. Check with `objdump --disassemble` if a delay is suspiciously short.
- Delay length depends on `-O` level. Changing optimisation changes your blink rate.
- Re-run all pipeline stages after an edit (see chapter 05).

## Verify

- The LED visibly blinks.
- `objdump --disassemble main.o` shows the loop with a load from the GPIO register inside it, not removed.

## Sources

- nRF52840 PS v1.11 (clock chapter, GPIO); GCC `volatile` semantics.

## Next

Automate the build with a Makefile, then replace the hard-coded `0x9` reset entry with the function's symbol, and write real startup code (`.data` / `.bss` initialisation).
