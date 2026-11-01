#include "main.h"

void SystemInit(void) {
    // 1. Enable HSE
    RCC->CR |= RCC_CR_HSEON;

    // 2. Wait until HSE is ready
    while (!(RCC->CR & RCC_CR_HSERDY));

    // 3. Set HSE as system clock source
    RCC->CFGR &= ~RCC_CFGR_SW;          // Clear SW bits
    RCC->CFGR |=  RCC_CFGR_SW_HSE;      // Select HSE as SYSCLK

    // 4. Wait until HSE is used as system clock
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE);

    // 5. Set AHB, APB prescalers to 1 (no division)
    RCC->CFGR &= ~RCC_CFGR_HPRE;        // HCLK  = SYSCLK = 8MHz
    RCC->CFGR &= ~RCC_CFGR_PPRE;        // PCLK1 = HCLK   = 8MHz
}

void GPIO_Init(void) {
    // Enable clocks for GPIOA, GPIOB, GPIOD
    // (GPIOF is for NRST - no clock needed)
    RCC->IOPENR |= RCC_IOPENR_GPIOAEN   // PA pins
               |   RCC_IOPENR_GPIOBEN   // PB pins
               |   RCC_IOPENR_GPIODEN;  // PD pins

// =========================================================
    // GPIOA CONFIGURATION
    // =========================================================

    // --- MODER: Set pin modes ---
    // First clear all pins we're configuring, then set their mode.
    // PA0  = Alternate Function (10) → Timer input capture
    // PA1  = Output (01)             → JSN_TRIG
    // PA2  = Analog (11)             → Batt_Sense ADC
    // PA3  = Output (01)             → Batt_EN
    // PA4  = Analog (11)             → USB_5V_Sense ADC
    // PA6  = Output (01)             → BOOST_EN
    // PA8  = Analog (11)             → CC1 ADC
    // PA9  = Alternate Function (10) → USART1_TX
    // PA10 = Alternate Function (10) → USART1_RX
    // PA13 = Alternate Function (10) → SWDIO  (default, just leave it)
    // PA14 = Alternate Function (10) → SWCLK  (default, just leave it)


    GPIOA->ODR &= ~( (1U << 1)   // PA1 JSN_TRIG  → LOW
               | (1U << 3)   // PA3 BATT_EN   → LOW
               | (1U << 6)); // PA6 BOOST_EN  → LOW

    // Clear the bits for PA0-PA10 (PA13/PA14 left as default AF)
    GPIOA->MODER &= ~( (3U << (0  * 2))    // PA0
                     | (3U << (1  * 2))    // PA1
                     | (3U << (2  * 2))    // PA2
                     | (3U << (3  * 2))    // PA3
                     | (3U << (4  * 2))    // PA4
                     | (3U << (6  * 2))    // PA6
                     | (3U << (9  * 2))    // PA9
                     | (3U << (10 * 2)) ); // PA10

    // Now set the modes
    GPIOA->MODER |=  ( (2U << (0  * 2))    // PA0  = AF
                     | (1U << (1  * 2))    // PA1  = Output
                     | (3U << (2  * 2))    // PA2  = Analog
                     | (1U << (3  * 2))    // PA3  = Output
                     | (3U << (4  * 2))    // PA4  = Analog
                     | (1U << (6  * 2))    // PA6  = Output
                     | (2U << (9  * 2))    // PA9  = AF (USART1_TX)
                     | (2U << (10 * 2)) ); // PA10 = AF (USART1_RX)

    // --- OTYPER: All outputs Push-Pull (default 0, nothing to set) ---

    // --- PUPDR: No pull resistors needed on any PA pin ---
    GPIOA->PUPDR &= ~( (3U << (0  * 2))
                     | (3U << (1  * 2))
                     | (3U << (2  * 2))
                     | (3U << (3  * 2))
                     | (3U << (4  * 2))
                     | (3U << (6  * 2))
                     | (3U << (9  * 2))
                     | (3U << (10 * 2)) );

    // --- OSPEEDR: Low speed for all (default 0, nothing to set) ---

    // --- Alternate Function: PA0 → TIM2_CH1, PA9/PA10 → USART1 ---
    // AFR[0] covers PA0-PA7, AFR[1] covers PA8-PA15
    // PA0  → AF2 (TIM2_CH1)
    // PA9  → AF1 (USART1_TX)
    // PA10 → AF1 (USART1_RX)
    GPIOA->AFR[0] &= ~(0xFU << (0 * 4));    // Clear PA0 AF bits            
    GPIOA->AFR[0] |=  ( (2U << (0 * 4)) );   // PA0 = AF2 (TIM2_CH1)

    GPIOA->AFR[1] &= ~( (0xFU << ((9-8)  * 4))   // Clear PA9  AF bits
                      | (0xFU << ((10-8) * 4)) ); // Clear PA10 AF bits

    GPIOA->AFR[1] |=  ( (1U << ((9-8)  * 4))     // PA9  = AF1 (USART1_TX)
                      | (1U << ((10-8) * 4)) );   // PA10 = AF1 (USART1_RX)


    // =========================================================
    // GPIOB CONFIGURATION
    // =========================================================

    // PB0  = Input                                          → CHG_STAT
    // PB1  = Alternate Function (10)                        → PWM Buzz
    // PB8  = Alternate Function (10)                        → I2C1_SCL
    // PB9  = Alternate Function (10)                        → I2C1_SDA
    // PB15 = Analog (11)                                    → CC2 ADC

    GPIOB->MODER &= ~( (3U << (0  * 2))    // PB0
                     | (3U << (1  * 2))    // PB1
                     | (3U << (8  * 2))    // PB8
                     | (3U << (9  * 2)) );  // PB9


    GPIOB->MODER |=  ( (0U << (0  * 2))    // PB0  = Input
                     | (2U << (1  * 2))    // PB1  = AF (PWM)
                     | (2U << (8  * 2))    // PB8  = AF (I2C1_SCL)
                     | (2U << (9  * 2)) ); // PB9  = AF (I2C1_SDA) 

    // OTYPER: PB8 and PB9 open-drain (I2C requires it!)
    GPIOB->OTYPER  |= ((1U << 8)             // PB8  = Open-Drain (I2C)
                   | (1U << 9));            // PB9  = Open-Drain (I2C)

    // PUPDR: PB8 and PB9 pull-up (I2C requires it!)
    GPIOB->PUPDR &= ~( (3U << (0  * 2)) 
                     | (3U << (8 * 2))
                     | (3U << (9 * 2)) );
    GPIOB->PUPDR |=  ( (1U << (0  * 2))    // PB0 = Pull-Up
                     | (1U << (8 * 2))     // PB8 = Pull-Up
                     | (1U << (9 * 2)) );  // PB9 = Pull-Up

    // Alternate Functions:
    // PB1  → AF0 (TIM14_CH1)
    // PB8  → AF6 (I2C1_SCL)
    // PB9  → AF6 (I2C1_SDA)
    GPIOB->AFR[0] &= ~(0xFU << (1 * 4));  // Clear PB1 AF bits and sets to AF0 (TIM14_CH1)

    GPIOB->AFR[1] &= ~( (0xFU << ((8-8) * 4))
                      | (0xFU << ((9-8) * 4)) );
    GPIOB->AFR[1] |=  ( (6U << ((8-8) * 4))   // PB8 = AF6 (I2C1_SCL)
                      | (6U << ((9-8) * 4)) ); // PB9 = AF6 (I2C1_SDA)


    // =========================================================
    // GPIOD CONFIGURATION
    // =========================================================

    GPIOD->ODR &= ~( (1U << 2)   // PD2 LED1 → OFF
               | (1U << 3)); // PD3 LED2 → OFF

    // PD2 = Output (01) → LED1
    // PD3 = Output (01) → LED2

    GPIOD->MODER &= ~( (3U << (2 * 2))
                     | (3U << (3 * 2)) );

    GPIOD->MODER |=  ( (1U << (2 * 2))    // PD2 = Output
                     | (1U << (3 * 2)) );  // PD3 = Output

    // OTYPER: LEDS are Open-Drain by design 
    GPIOD->OTYPER  |= ( (1U << 2)             // PD2  = Open-Drain (LED)
                    | (1U << 3));            // PD3  = Open-Drain (LED)
    // PUPDR: No pull resistors (default, nothing to set)
    // OSPEEDR: Low speed (default, nothing to set)
}

void USART1_Init(void) {

    // 1. Enable USART1 clock
    RCC->APBENR2 |= RCC_APBENR2_USART1EN;

    // 2. Set baud rate
    // BRR = PCLK / baud = 8000000 / 115200 = 69
    USART1->BRR = 69;

    // 3. Configure CR1:
    // - Word length = 8 bits (M0=0, M1=0)
    // - Parity = none (PCE=0)
    // - TX enable (TE=1)
    // - RX enable (RE=1)
    USART1->CR1 &= ~( USART_CR1_M0          // 8-bit word length
                    | USART_CR1_M1
                    | USART_CR1_PCE );       // No parity

    USART1->CR1 |=  ( USART_CR1_TE          // Enable TX
                    | USART_CR1_RE );        // Enable RX

    // 4. Configure CR2:
    // - Stop bits = 1 (STOP=00)
    USART1->CR2 &= ~USART_CR2_STOP;         // 1 stop bit

    // 5. Enable USART1
    USART1->CR1 |= USART_CR1_UE;            // USART enable
}

// Send a single character
void USART1_SendChar(char c) {
    // Wait until TX register is empty
    while (!(USART1->ISR & USART_ISR_TXE_TXFNF));
    // Write character to transmit register
    USART1->TDR = c;
}

// Send a string
void USART1_SendString(const char *str) {
    while (*str) {
        USART1_SendChar(*str++);
    }
}

// Send a number (integer)
void USART1_SendNumber(uint32_t num) {
    char buf[12];
    int i = 0;

    // Handle zero
    if (num == 0) {
        USART1_SendChar('0');
        return;
    }

    // Convert number to string in reverse
    while (num > 0) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }

    // Send in correct order
    for (int j = i - 1; j >= 0; j--) {
        USART1_SendChar(buf[j]);
    }
}

void TIM16_Init(void) {

    // 1. Enable TIM16 clock
    RCC->APBENR2 |= RCC_APBENR2_TIM16EN;

    // 2. Set prescaler for 1MHz tick (1µs resolution)
    // Tick = PCLK / (PSC + 1) = 8MHz / (7+1) = 1MHz
    TIM16->PSC = 7;

    // 3. Set auto-reload to max (16-bit timer)
    TIM16->ARR = 0xFFFF;     // max 65535µs = ~65ms per delay

    // 4. Enable one pulse mode
    // Timer stops automatically after ARR is reached
    TIM16->CR1 |= TIM_CR1_OPM;

    // 5. Start the timer
    TIM16->CR1 |= TIM_CR1_CEN;
}

void delay_us(uint32_t us) {
    // Reset counter
    TIM16->CNT = 0;

    // Wait until counter reaches target
    while (TIM16->CNT < us);
}

void delay_ms(uint32_t ms) {
    // Call delay_us 1000 times for each millisecond
    for (uint32_t i = 0; i < ms; i++) {
        delay_us(1000);
    }
}

void TIM2_Init(void) {

    // 1. Enable TIM2 clock
    RCC->APBENR1 |= RCC_APBENR1_TIM2EN;

    // 2. Set prescaler for 1MHz tick (1µs resolution)
    // Tick = PCLK / (PSC + 1) = 8MHz / (7+1) = 1MHz
    TIM2->PSC = 7;

    // 3. Set auto-reload to max (32-bit timer)
    // We want the timer to count as long as possible
    // so it doesn't overflow during a long echo pulse
    TIM2->ARR = 0xFFFFFFFF;

    // 4. Configure CH1 as Input Capture
    // CCMR1 controls channels 1 and 2
    // CC1S = 01 → CH1 is input, mapped to TI1 (PA0)
    TIM2->CCMR1 &= ~TIM_CCMR1_CC1S;        // Clear CC1S bits
    TIM2->CCMR1 |=  (1U << TIM_CCMR1_CC1S_Pos); // CC1S = 01 (input on TI1)

    // 5. No input filter, no prescaler on capture
    TIM2->CCMR1 &= ~TIM_CCMR1_IC1F;        // No filter
    TIM2->CCMR1 &= ~TIM_CCMR1_IC1PSC;      // Capture every edge

    // 6. Set capture on RISING edge first (start of pulse)
    // CCER: CC1P = 0 (rising), CC1NP = 0
    TIM2->CCER &= ~TIM_CCER_CC1P;           // Rising edge
    TIM2->CCER &= ~TIM_CCER_CC1NP;          // Not both edges

    // 7. Enable capture on CH1
    TIM2->CCER |= TIM_CCER_CC1E;            // Enable CH1 capture

    // 8. Start the timer
    TIM2->CR1 |= TIM_CR1_CEN;               // Counter enable
}

uint32_t JSN_Read(void) {
    uint32_t start, end, pulse_width;

    // Step 1 — Send 10µs trigger pulse
    GPIOA->ODR |=  (1U << 1);   // PA1 HIGH
    delay_us(10);                // Wait 10µs
    GPIOA->ODR &= ~(1U << 1);   // PA1 LOW

 uint32_t timeout = 100000;
    while (!(TIM2->SR & TIM_SR_CC1IF)) {
        if (--timeout == 0) {
            USART1_SendString("JSN timeout - no echo!\r\n");
            return 0;   // sensor not responding
        }
    }
    start = TIM2->CCR1;              // save rising timestamp

    // ── Step 3: Wait for falling edge ──
    // Switch to falling edge capture
    TIM2->CCER |=  TIM_CCER_CC1P;   // falling edge
    TIM2->SR   &= ~TIM_SR_CC1IF;    // clear capture flag

    timeout = 100000;
    while (!(TIM2->SR & TIM_SR_CC1IF)) {
        if (--timeout == 0) {
            USART1_SendString("JSN timeout - no falling edge!\r\n");
            return 0;
        }
    }
    end = TIM2->CCR1;                // save falling timestamp


    // Step 4 — Calculate pulse width in µs
    pulse_width = end - start;       // Works even if counter wrapped!

    // Step 5 — Convert to distance in cm
    // Speed of sound = 343m/s
    // Distance = pulse_width(µs) / 58
    return pulse_width / 58;
}

void ADC_Init(void) {

    // 1. Enable ADC clock
    RCC->APBENR2 |= RCC_APBENR2_ADCEN;

    // 2. Set ADC clock source to PCLK/2 = 4MHz
    // CKMODE = 01 → synchronous PCLK/2
    ADC1->CFGR2 &= ~ADC_CFGR2_CKMODE;
    ADC1->CFGR2 |=  (1U << ADC_CFGR2_CKMODE_Pos);

    // 3. Calibrate ADC
    // Must be done before enabling ADC
    // ADCAL = 1 → start calibration
    ADC1->CR |= ADC_CR_ADCAL;
    while (ADC1->CR & ADC_CR_ADCAL);   // wait for calibration to finish

    // 4. Configure resolution and alignment
    // RES = 00 → 12-bit resolution
    // ALIGN = 0 → right aligned
    ADC1->CFGR1 &= ~ADC_CFGR1_RES;
    ADC1->CFGR1 &= ~ADC_CFGR1_ALIGN;

    // 5. Single conversion mode (default)
    ADC1->CFGR1 &= ~ADC_CFGR1_CONT;   // not continuous

    // 6. Set sampling time to maximum for accuracy
    // SMP = 111 → 160.5 clock cycles
    ADC1->SMPR |= ADC_SMPR_SMP1;

    // 7. Enable ADC
    ADC1->CR |= ADC_CR_ADEN;

    // 8. Wait for ADC ready
    while (!(ADC1->ISR & ADC_ISR_ADRDY));
}
uint16_t ADC_Read(uint32_t channel) {

    // 1. Select channel
    ADC1->CHSELR = (1U << channel);

    // 2. Clear end of conversion flag
    ADC1->ISR |= ADC_ISR_EOC;

    // 3. Start conversion
    ADC1->CR |= ADC_CR_ADSTART;

    // 4. Wait for conversion to complete
    while (!(ADC1->ISR & ADC_ISR_EOC));

    // 5. Read and return result
    return (uint16_t)ADC1->DR;
}
// Convert ADC reading to millivolts
// Vref = 3300mV, 12-bit = 4095 steps
uint32_t ADC_ToMillivolts(uint16_t adc_val) {
    return (adc_val * 3300UL) / 4095;
}

// Read battery voltage in millivolts
// Adjust divider_ratio to match your schematic
uint32_t Batt_ReadmV(void) {
    uint16_t raw = ADC_Read(2);          // PA2 = channel 2
    uint32_t mv  = ADC_ToMillivolts(raw);

    // Correct for voltage divider
    // If R1 = R2 → ratio = 2
    return mv * 2;
}

// Read USB 5V sense in millivolts
uint32_t USB_ReadmV(void) {
    uint16_t raw = ADC_Read(4);          // PA4 = channel 4
    uint32_t mv  = ADC_ToMillivolts(raw);

    // Correct for voltage divider
    // R1 = R2 = 10k → ratio = 2
    return mv * 2;
}

int main(void) {
    SystemInit();
    GPIO_Init();
    USART1_Init();

    USART1_SendString("System initialized!\r\n");

    uint32_t count = 0;

    while(1) {
        USART1_SendString("Count: ");
        USART1_SendNumber(count++);
        USART1_SendString("\r\n");
        delay_ms(500);  
        uint32_t distance = JSN_Read(); 
        USART1_SendString("Distance: ");
        USART1_SendNumber(distance);
        USART1_SendString(" cm\r\n");
        delay_ms(100);    // 100ms between readings
         // Read battery
        uint32_t batt_mv = Batt_ReadmV();
        USART1_SendString("Battery: ");
        USART1_SendNumber(batt_mv);
        USART1_SendString(" mV\r\n");

        // Read USB
        uint32_t usb_mv = USB_ReadmV();
        USART1_SendString("USB 5V: ");
        USART1_SendNumber(usb_mv);
        USART1_SendString(" mV\r\n");

        USART1_SendString("---\r\n");
        delay_ms(500);
    }

    uint32_t distance_cm;

    while(1) {
        
    }
}