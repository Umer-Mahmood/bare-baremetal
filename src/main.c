typedef struct{
    int initial_sp;
    int reset_handler;
}vector_table;

/* Known limitations of this table:
 * - Only 2 of the core's vector entries exist. The word at 0x8 (NMI) and 0xC
 *   (HardFault) are not defined here, so a fault would fetch garbage.
 * - reset_handler is hard-coded as 0x9 (0x8 + Thumb bit). It only works while
 *   main() is the first code at 0x8; it should reference the function by name.
 * - Fields are plain ints, not uint32_t / function pointers.
 */
__attribute__((
    section (".vector_table"),
    used))
vector_table table  ={
    .initial_sp = (0x20000000 + 256*1024), //0x20040000, end address of RAM
    .reset_handler = 0x00000009 // main() at 0x8, bit 0 = 1 for Thumb
};

#define GPIO_BASE_ADDRESS_P0 0x50000000
#define GPIO_OUT_REG_OFFSET 0x504
#define P0_OUT_REG (GPIO_BASE_ADDRESS_P0 + GPIO_OUT_REG_OFFSET)

#define GPIO_DIR_REG_OFFSET 0x514
#define P0_DIR_REG (GPIO_BASE_ADDRESS_P0 + GPIO_DIR_REG_OFFSET)

#define LED_PIN 11

int main(){
    // led is connected on p0.11 : port 0, pin 11 and works active low
    volatile unsigned int *dir_reg = (volatile unsigned int*)P0_DIR_REG;
    *dir_reg |= (1 << LED_PIN); // set pin as output

    volatile unsigned int *out_reg = (volatile unsigned int *)P0_OUT_REG;

    while(1){
        //*out_reg &= ~(1 << LED_PIN); // set it to low
        *out_reg ^= (1 << LED_PIN);
        // Busy-wait delay: no timer is set up, so the CPU just burns cycles.
        // The core runs at 64 MHz after reset and each iteration takes several
        // cycles, so 1M iterations is a rough, uncalibrated delay (tune by eye).
        for (int i = 0; i < 1000000; i++)
        {
            // Read of a volatile register: the compiler must keep this access,
            // so the otherwise empty loop is not optimised away at -O1.
            volatile int val = *out_reg;
        }
    }
}