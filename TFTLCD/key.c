#include "key.h"
#include "stm32f10x_gpio.h"
 
void KEY_GPIO_Init(void){
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_APB2PeriphClockCmd(KEY0_RCC_APB2Periph_CLK | KEY_UP_RCC_APB2Periph_CLK, ENABLE);

    GPIO_InitStruct.GPIO_Pin = KEY0_GPIO_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(KEY0_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = KEY1_GPIO_Pin;
    GPIO_Init(KEY1_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = KEY2_GPIO_Pin;
    GPIO_Init(KEY2_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = KEY_UP_GPIO_Pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(KEY_UP_GPIO_Port, &GPIO_InitStruct);
}


u8 KEY_Scan(u8 mode){
    static u8 key_up = 1;
    if(mode) key_up = 1;
    if(KEY0 == 1 && KEY1 == 1 && KEY2 == 1 && WK_UP == 0)
    {
        key_up = 1;
    }
    if(key_up && (KEY0 == 0  || KEY1 == 0 || KEY2 == 0 || WK_UP == 1))
    {
        delay_ms(10);
        if(KEY0 == 0 || KEY1 == 0 || KEY2 == 0 || WK_UP == 1)
        {
            key_up = 0;
            if(KEY0 == 0) return KEY0_PRESS;
            if(KEY1 == 0) return KEY1_PRESS;
            if(KEY2 == 0) return KEY2_PRESS;
            if(WK_UP == 1) return WK_UP_PRESS;
        }
    }
    return 0;
}
