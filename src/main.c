typedef struct{
    int initial_sp;
    int reset_handler;
}vector_table;

__attribute__((
    section (".vector_table"),
    used))
vector_table table  ={
    .initial_sp = (0x20000000 + 256*1024), //0x20040000, end address of RAM
    .reset_handler = 0x00000009
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