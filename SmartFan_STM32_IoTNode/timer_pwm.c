#include"timer_pwm.h"

#include "RccConfig.h"


void TIM4_Config(void) {
        // 1. Enable Timer clock
        RCC -> APB1ENR |= (1 << 2); // Enable the timer4 clock

        // 2. Set the prescalar and the ARR
        TIM4 -> CR1 = (1 << 7) | (1 << 1);
        TIM4 -> PSC = 84 - 1; // 84MHz/84 = 1 MHz ~~ 1 uS delay
        TIM4 -> ARR = 10 - 1; // MAX ARR value 100KHz
        TIM4 -> CR1 &= ~(1 << 1);
        // 3. Enable the Timer, and wait for the update Flag to set
        TIM4 -> EGR = 1; // Enable the Counter
        TIM4 -> CR1 |= (1 << 0);

        //PWM
        TIM4 -> CCMR1 &= ~(7 << 4);
        TIM4 -> CCMR1 |= (6 << 4);
        TIM4 -> CCMR1 |= (1 << 3);
        TIM4 -> CCER |= (1 << 0);
        TIM4 -> CCR1 = 0;

        //pin PB6 fan
        RCC -> AHB1ENR |= (1 << 1); // Enable the GPIOB clock
        GPIOB -> MODER &= ~(3 << 12); // pin PB6(bits 13:12 = 0:0) as output (10)
        GPIOB -> MODER |= (1 << 13);
        GPIOB -> OSPEEDR |= (3 << 12);
        GPIOB -> AFR[0] &= ~(15 << 24);
        GPIOB -> AFR[0] |= (2 << 24);

}