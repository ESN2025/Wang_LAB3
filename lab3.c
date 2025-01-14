#include <stdint.h>
#include "system.h"                  
#include "sys/alt_stdio.h"           // alt_printf, alt_putstr 
#include "sys/alt_irq.h"             // alt_irq_register / alt_ic_isr_register
#include <io.h>                      // IOWR_32DIRECT, IORD_32DIRECT
#include "altera_avalon_timer_regs.h"// IOWR_ALTERA_AVALON_TIMER_*
#include "unistd.h"                 

static volatile uint32_t g_sec_count = 0;

static void timer_isr(void *context)
{
    IOWR_ALTERA_AVALON_TIMER_STATUS(TIMER_0_BASE, 0);

    static uint16_t ms_count = 0;
    ms_count++;
    if (ms_count >= 1000) {
        ms_count = 0;
        g_sec_count++;  
    }
}

int main(void)
{
    alt_printf("===== Lab3: Timer Demo Start =====\n");

    
#ifdef ALT_ENHANCED_INTERRUPT_API_PRESENT
    alt_ic_isr_register(TIMER_0_IRQ_INTERRUPT_CONTROLLER_ID,
                        TIMER_0_IRQ,
                        timer_isr,
                        NULL, 
                        0);
#else
    alt_irq_register(TIMER_0_IRQ, NULL, timer_isr);
#endif


    IOWR_ALTERA_AVALON_TIMER_PERIODL(TIMER_0_BASE, 0xC350); 
    IOWR_ALTERA_AVALON_TIMER_PERIODH(TIMER_0_BASE, 0x0000);


    IOWR_ALTERA_AVALON_TIMER_CONTROL(
        TIMER_0_BASE,
        ALTERA_AVALON_TIMER_CONTROL_CONT_MSK |  
        ALTERA_AVALON_TIMER_CONTROL_ITO_MSK  |  
        ALTERA_AVALON_TIMER_CONTROL_START_MSK   
    );

    unsigned int count = 0;
    unsigned int last_sec_val = 0;

    while (1) 
    {

        if (g_sec_count != last_sec_val) {
            last_sec_val = g_sec_count; 

            unsigned int value = count;
            unsigned int units = value % 10;
            value /= 10;
            unsigned int tens  = value % 10;
            value /= 10;
            unsigned int hundr = value % 10;

            IOWR_32DIRECT(UNITS_PIO_BASE, 0, units);
            IOWR_32DIRECT(TENS_PIO_BASE,  0, tens);
            IOWR_32DIRECT(HUND_PIO_BASE,  0, hundr);

            count = (count + 1) % 1000;

            alt_printf("Count = %x (hex)\n", count);
        }

    }

    return 0;
}
