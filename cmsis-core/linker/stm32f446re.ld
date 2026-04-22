/* ============================================================
 *  Safe generic STM32 linker script (FLASH + RAM)
 *  Compatible with STM32CubeMX / CMSIS startup code
 * ============================================================ */

ENTRY(Reset_Handler)

/* ============================================================
 *  Memory layout (filled by CMake)
 * ============================================================ */
MEMORY
{
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 512K
  RAM   (xrw) : ORIGIN = 0x20000000,   LENGTH = 128K
}

/* ============================================================
 *  Stack & heap configuration
 * ============================================================ */
_Min_Heap_Size  = 0x200;
_Min_Stack_Size = 0x400;

/* End of RAM, used by startup */
_estack = ORIGIN(RAM) + LENGTH(RAM);

/* ============================================================
 *  Sections
 * ============================================================ */
SECTIONS
{
  /* ----------------------------------------------------------
   *  Interrupt vector table
   * ---------------------------------------------------------- */
  .isr_vector :
  {
    . = ALIGN(4);
    KEEP(*(.isr_vector))
    . = ALIGN(4);
  } > FLASH

  /* ----------------------------------------------------------
   *  Program code
   * ---------------------------------------------------------- */
  .text :
  {
    . = ALIGN(4);

    *(.text)
    *(.text*)
    *(.glue_7)
    *(.glue_7t)
    *(.eh_frame)

    KEEP(*(.init))
    KEEP(*(.fini))

    . = ALIGN(4);
  } > FLASH

  /* ----------------------------------------------------------
   *  Read-only data
   * ---------------------------------------------------------- */
  .rodata :
  {
    . = ALIGN(4);
    *(.rodata)
    *(.rodata*)
    . = ALIGN(4);
  } > FLASH

  /* ----------------------------------------------------------
   *  ARM exception tables (C++)
   * ---------------------------------------------------------- */
  .ARM.extab :
  {
    . = ALIGN(4);
    *(.ARM.extab* .gnu.linkonce.armextab.*)
    . = ALIGN(4);
  } > FLASH

  .ARM.exidx :
  {
    . = ALIGN(4);
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
    . = ALIGN(4);
  } > FLASH

  /* ----------------------------------------------------------
   *  C++ constructors / destructors
   * ---------------------------------------------------------- */
  .preinit_array :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN(__preinit_array_start = .);
    KEEP(*(.preinit_array*))
    PROVIDE_HIDDEN(__preinit_array_end = .);
    . = ALIGN(4);
  } > FLASH

  .init_array :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN(__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array*))
    PROVIDE_HIDDEN(__init_array_end = .);
    . = ALIGN(4);
  } > FLASH

  .fini_array :
  {
    . = ALIGN(4);
    PROVIDE_HIDDEN(__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.fini_array*))
    PROVIDE_HIDDEN(__fini_array_end = .);
    . = ALIGN(4);
  } > FLASH

  /* End of text/rodata/ctors area */
  . = ALIGN(4);
  _etext = .;

  /* ----------------------------------------------------------
   *  Initialized data (copied from FLASH to RAM)
   * ---------------------------------------------------------- */
  .data :
  {
    . = ALIGN(4);
    _sdata = .;
    __data_start__ = _sdata;

    *(.data)
    *(.data*)
    *(.RamFunc)
    *(.RamFunc*)

    . = ALIGN(4);
    _edata = .;
    __data_end__ = _edata;
  } > RAM AT > FLASH

  /* Source address for data init (used by startup) */
  _sidata = LOADADDR(.data);

  /* ----------------------------------------------------------
   *  Uninitialized data
   * ---------------------------------------------------------- */
  .bss :
  {
    . = ALIGN(4);
    _sbss = .;
    __bss_start__ = _sbss;

    *(.bss)
    *(.bss*)
    *(COMMON)

    . = ALIGN(4);
    _ebss = .;
    __bss_end__ = _ebss;
  } > RAM

  /* ----------------------------------------------------------
   *  Heap and stack
   * ---------------------------------------------------------- */
  ._user_heap_stack :
  {
    . = ALIGN(8);
    PROVIDE(end = .);
    PROVIDE(_end = .);

    . = . + _Min_Heap_Size;
    . = . + _Min_Stack_Size;

    . = ALIGN(8);
  } > RAM

  /* ----------------------------------------------------------
   *  Discard unused standard libraries
   * ---------------------------------------------------------- */
  /DISCARD/ :
  {
    libc.a ( * )
    libm.a ( * )
    libgcc.a ( * )
  }

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
