#include <stdint.h>

extern uint32_t _sdata; // start address of .data section in RAM
extern uint32_t _edata; // end address of .data section in RAM
extern uint32_t _sidata; // start address of .data section in FLASH
extern uint32_t _sbss; // start address of .bss section in RAM
extern uint32_t _ebss; // end address of .bss section in RAM

/* declaration main function */
int main(void);

void __libc_init_array(void);

void Reset_Handler(void) {
	// copy .data section from flash to ram
	/* Copy the data segment initializers from flash to SRAM */
	uint32_t size = (uint32_t)&_edata - (uint32_t)&_sdata;
	uint8_t *pRAM = (uint8_t*)&_sdata;
	uint8_t *pFlash = (uint8_t*)&_sidata;

	for(uint32_t i=0; i<size; i++) {
		pRAM[i] = pFlash[i];
	}

	// initialize .bss section
	/* Zero fill the bss segment. */
	size = (uint32_t)&_ebss - (uint32_t)&_sbss;
	pRAM = (uint8_t*)&_sbss;

	for(uint32_t i=0; i<size; i++) {
		pRAM[i] = 0;
	}

	// Call static constructors
	__libc_init_array();
	// Call the application's entry point.
	main();
}

