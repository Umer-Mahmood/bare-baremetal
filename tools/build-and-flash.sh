#! /bin/bash
set -e
cd src
arm-none-eabi-gcc -c main.c -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -O1
arm-none-eabi-ld main.o --script linker.ld  --output linked_output.out
arm-none-eabi-objcopy --output-target binary linked_output.out firmware.bin
nrfjprog --eraseall
nrfjprog --program firmware.bin  --verify --family NRF52
nrfjprog --reset