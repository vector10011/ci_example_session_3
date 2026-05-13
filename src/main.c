#include "main.h"

#include "stm32f4xx.h"

#define                 PLL_CLK_SRC                     HSE
#define                 PLL_CLK_SRC_FREQ                16UL
#define                 PLL_M                           8UL
#define                 PLL_N                           162UL
#define                 PLL_P                           2UL

#define                 HPRE_DIV                        RCC_CFGR_HPRE_DIV2
#define                 PPRE1_DIV                       RCC_CFGR_PPRE1_DIV2
#define                 PPRE2_DIV                       RCC_CFGR_PPRE2_DIV2

#define                 TIMER_CLK_EN()                   (RCC->APB1ENR |= RCC_APB1ENR_TIM14EN)
#define                 TIMER                           TIM14
#define                 TIMER_UE_FREQ                   1000UL
#define                 TIMER_PSC                       1000UL
#define                 TIMER_ARR                       1000UL

#define                 DAC_CHANNEL                     1
#define                 DAC_CH_PORT                     GPIOA
#define                 DAC_CH_PIN                      4
#define                 DAC_RES                         0xFFFUL

#define                 DMA_CLK_EN()                      (RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN)
#define                 DMA                             DMA1
#define                 DMA_STR                         DMA1_Stream7
#define                 DMA_CH                          7
#define                 DMA_DIR                         0x0
#define                 DMA_MSIZE                       0x0
#define                 DMA_PSIZE                       0x2
#define                 DMA_MINC                        0
#define                 DMA_CIRC                        1
#define                 DMA_NDTR                        122

uint32_t periph_data;
uint32_t mem_data;

int main()
{
	/*====================================================================================================*/

    // RCC Config

    FLASH->ACR |= FLASH_ACR_LATENCY_3WS;                                                    // Set flash latency to 3 wait states

    #if (PLL_CLK_SRC == HSE)
        RCC->CR |= RCC_CR_HSEON;                                                                // Enable HSE
        while (!(RCC->CR & RCC_CR_HSERDY));                                                     // Wait until HSE
        RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC;                                                     // Set PLL source to HSE
    #endif

    RCC->CFGR |= (
        HPRE_DIV                                                                  // AHB prescaler = 2
        | PPRE1_DIV                                                               // APB1 prescaler = 2
        | PPRE2_DIV                                                               // APB2 prescaler = 2
    );

    RCC->PLLCFGR &= ~(
        RCC_PLLCFGR_PLLM
        | RCC_PLLCFGR_PLLN
        | RCC_PLLCFGR_PLLP
    );                                                                                      // Clear PLL configuration
    RCC->PLLCFGR |= (
        (8 << RCC_PLLCFGR_PLLM_Pos)                                                         // Set PLLM to 8
        | (40 << RCC_PLLCFGR_PLLN_Pos)                                                      // Set PLLN to 336
        | (0 << RCC_PLLCFGR_PLLP_Pos)                                                       // Set PLLP to 2
    );

    RCC->CR |= RCC_CR_PLLON;                                                                // Enable PLL
    while (!(RCC->CR & RCC_CR_PLLRDY));                                                     // Wait until PLL
    RCC->CFGR |= RCC_CFGR_SW_PLL;                                                           // Select PLL as system clock source
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);                                 // Wait until PLL is used as system clock source

    /*====================================================================================================*/

    // TIM Config

    TIMER_CLK_EN();                                                                                 // Enable timer clock
    TIMER->PSC = TIMER_PSC - 1UL;                                                                      // Set prescaler to 39999 (1 kHz timer clock)
    TIMER->ARR = TIMER_ARR - 1UL;                                                                        // Set auto-reload to 999 (1 second interval)
    TIMER->DIER |= TIM_DIER_UIE;                                                             // Enable update interrupt
    TIMER->CR1 |= TIM_CR1_CEN;                                                               // Start the timer
    // NVIC_EnableIRQ(TIM14_IRQn);                                                    // Enable TIM14 interrupt in NVIC

    /*====================================================================================================*/

    // Configure DAC

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;                                                     // Enable GPIOA clock
    GPIOA->MODER |= (3 << (DAC_CH_PIN * 2));                                                 // Set PA4 to analog mode

    RCC->APB1ENR |= RCC_APB1ENR_DACEN;
    DAC->CR |= DAC_CR_EN1;                                                                  // Enable DAC channel 1
    DAC->DHR12R1 = DAC_RES;                                                                    // Set DAC output to mid-scale (2048 for 12-bit resolution)

    /*====================================================================================================*/

    // DMA Config

    DMA_CLK_EN();


    DMA_STR->PAR = (uint32_t)&periph_data;
    DMA_STR->M0AR = (uint32_t)&mem_data;
    DMA_STR->NDTR = DMA_NDTR;
    DMA_STR->CR = (DMA_CH << DMA_SxCR_CHSEL_Pos)
                        | (DMA_DIR << DMA_SxCR_DIR_Pos)
                      | (DMA_MSIZE << DMA_SxCR_MSIZE_Pos)
                      | (DMA_PSIZE << DMA_SxCR_PSIZE_Pos)
                      | (DMA_CIRC << DMA_SxCR_CIRC_Pos)
                      | (DMA_MINC << DMA_SxCR_MINC_Pos);
    DMA_STR->CR |= DMA_SxCR_EN;

	for(;;)
	{
		// infinite loop
	}
}

 void NMI_Handler(void)
{
	while(1){};
}

 void HardFault_Handler(void)
{
	while(1){};
}

 void MemManage_Handler(void)
{
	while(1){};
}

 void BusFault_Handler(void)
{
	while(1){};
}

 void UsageFault_Handler(void)
{
	while(1){};
}