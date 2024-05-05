#include "tftlcd.h"
#include "rtx_os.h"
#include "stm32f10x_gpio.h"
#include "key.h"
#include "wrong.h"
#include "right.h"


osThreadId_t key_flag,button_flag,focus_flag,show_flag,judge_flag;
uint32_t flagsX1,flagsX2,flagsX3,flagsX4,flagsX5;


#define PASSWORD_LENGTH 5
uint8_t password[PASSWORD_LENGTH];
uint8_t presetPassword[PASSWORD_LENGTH] = {1, 1, 1, 1, 1};
static u8 fac_us=0;
static u16 fac_ms=0;
static u16 state=1;
static u16 old_state=1; 
static u16 input_num = 0;
typedef struct {
    uint8_t value;  
    uint16_t x1, y1, x2, y2;   
    const char* displayChar;   
} NumberInfo;

NumberInfo numbers[] = {
    {0x0001, 15, 75, 104, 164, "1"},
    {0x0002, 115, 75, 204, 164, "2"},
    {0x0003, 215, 75, 304, 164, "3"},
    {0x0004, 15, 175, 104, 264, "4"},
    {0x0005, 115, 175, 204, 264, "5"},
    {0x0006, 215, 175, 304, 264, "6"},
    {0x0007, 15, 275, 104, 364, "7"},
    {0x0008, 115, 275, 204, 364, "8"},
    {0x0009, 215, 275, 304, 364, "9"},
    {0x000A, 15, 375, 104, 464, "C"},
    {0x000B, 115, 375, 204, 464, "0"},
    {0x000C, 215, 375, 304, 464, "E"}
};


void SysTick_Init(u8 SYSCLK){
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
	fac_us=SYSCLK/8;
	fac_ms=(u16)fac_us*1000;
}

void delay_us(u32 nus){
	u32 temp;
	SysTick->LOAD=nus*fac_us;
	SysTick->VAL=0x00;
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;
	do{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
	SysTick->VAL =0X00;
}

void delay_ms(u16 nms){
	u32 temp;
	SysTick->LOAD=(u32)nms*fac_ms;
	SysTick->VAL =0x00;
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;
	do{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
	SysTick->VAL =0X00;
}

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
        //delay_ms(10);
        //if(KEY0 == 0 || KEY1 == 0 || KEY2 == 0 || WK_UP == 1)
        //{
            key_up = 0;
            if(KEY0 == 0) return KEY0_PRESS;
            if(KEY1 == 0) return KEY1_PRESS;
            if(KEY2 == 0) return KEY2_PRESS;
            if(WK_UP == 1) return WK_UP_PRESS;
        //}
    }
    return 0;
}

	
void find_key(void *argument){
	uint8_t key_sta = 0;
	while(1){
		key_sta = KEY_Scan(0);
		if(key_sta == KEY0_PRESS){
			osThreadFlagsSet(button_flag, 0x0001U);
			osDelay(50);
		}
		if(key_sta == KEY1_PRESS){
			osThreadFlagsSet(button_flag, 0x0002U);
			osDelay(50);
		}
		if(key_sta == KEY2_PRESS){
			osThreadFlagsSet(button_flag, 0x0004U);
			osDelay(50);
		}
		if(key_sta == WK_UP_PRESS){
			osThreadFlagsSet(button_flag, 0x0008U);
			osDelay(50);
		}
	}
}
void find_button(void *argument){// 320*480
	while(1){
		flagsX2=osThreadFlagsWait(0x000FU, osFlagsWaitAny, osWaitForever);
		old_state = state;
		uint32_t hexValue = 0;
		if((flagsX2 & 0x0001)==0x0001){
			if(state >= 12)
				state = 1;
			else
				state = state+1;
		}
		if((flagsX2 & 0x0002)==0x0002){
			hexValue = (uint32_t)state; 
        osThreadFlagsSet(show_flag, hexValue);	
		}
		if((flagsX2 & 0x0004)==0x0004){
			if(state <= 1)
				state = 12;
			else
				state = state-1;
		}
		if((flagsX2 & 0x0008)==0x0008){
			osThreadFlagsSet(judge_flag, 0x0001U);
		}
	    hexValue = (uint32_t)state; 
    	osThreadFlagsSet(focus_flag, hexValue);
		
	}
}
void ui_setup(){
    LCD_Clear(BLACK);
	LCD_Fill(15,10,304,59,WHITE);//input
	LCD_Fill(15,75,304,464,WHITE);//body

	LCD_DrawLine(15+96,75,15+96,464);
	LCD_DrawLine(15+96*2,75,15+96*2,464);
	for(int i=1;i<4;i++)//horizental
	{
		LCD_DrawLine(15,75+98*i,304,75+98*i);
	}
	
	LCD_ShowString(15+37,275+29,tftlcd_data.width,tftlcd_data.height,32,(u8*)"7");
	LCD_ShowString(115+37,275+29,tftlcd_data.width,tftlcd_data.height,32,(u8*)"8");	
	LCD_ShowString(215+37,275+29,tftlcd_data.width,tftlcd_data.height,32,(u8*)"9");
	
	LCD_ShowString(15+37,75+29,tftlcd_data.width,tftlcd_data.height,32,(u8*)"1");
	LCD_ShowString(115+37,75+29,tftlcd_data.width,tftlcd_data.height,32,(u8*)"2");
	LCD_ShowString(215+37,75+29,tftlcd_data.width,tftlcd_data.height,32,(u8*)"3");
	
    LCD_ShowString(15+37,175+29,tftlcd_data.width,tftlcd_data.height,32,(u8*)"4");
	LCD_ShowString(115+37,175+29,tftlcd_data.width,tftlcd_data.height,32,(u8*)"5");
    LCD_ShowString(215+37,175+29,tftlcd_data.width,tftlcd_data.height,32,(u8*)"6");
	
	LCD_ShowString(15+37,375+29,tftlcd_data.width,tftlcd_data.height,32,(u8*)"C");
	LCD_ShowString(115+37,375+29,tftlcd_data.width,tftlcd_data.height,32,(u8*)"0");
	LCD_ShowString(215+37,375+29,tftlcd_data.width,tftlcd_data.height,32,(u8*)"E");
}
	





void focus_button(void *argument){
	while(1){		
		flagsX3=osThreadFlagsWait(0x000FU, osFlagsWaitAny, osWaitForever);
		for (int i = 0; i < sizeof(numbers) / sizeof(numbers[0]); i++) {
			if ((flagsX3 & 0x000F) == numbers[i].value) {
				LCD_Fill(numbers[i].x1, numbers[i].y1, numbers[i].x2, numbers[i].y2, GREEN);
				LCD_ShowString(numbers[i].x1 + 37, numbers[i].y1 + 29, tftlcd_data.width, tftlcd_data.height, 32, (u8*)numbers[i].displayChar);
			} 
					else if ((old_state & 0x000F) == numbers[i].value) {
				LCD_Fill(numbers[i].x1, numbers[i].y1, numbers[i].x2, numbers[i].y2, WHITE);
				LCD_ShowString(numbers[i].x1 + 37, numbers[i].y1 + 29, tftlcd_data.width, tftlcd_data.height, 32, (u8*)numbers[i].displayChar);
			}
		osThreadFlagsSet(focus_flag, 0x0000U);
		}	
	}
}
 

u8 JudgePassword() {
    for (int i = 0; i < PASSWORD_LENGTH; i++) {
        if (password[i] != presetPassword[i]) {
            return 0;
        }
    }
    return 1;
}

void input_show(void *argument){
	while(1){
	flagsX4=osThreadFlagsWait(0x000FU, osFlagsWaitAny, osWaitForever);
	if(input_num>=5){
	  osThreadFlagsSet(judge_flag, 0x0001U);
	}
	else{
		if(flagsX4>=0x0001 && flagsX4<=0x0009){
			LCD_ShowChar(290-20*input_num,30,'*',24,0);
		  password[input_num++]=flagsX4;
		}
		else if(flagsX4 == 0x000B){
		   LCD_ShowChar(290-20*input_num,30,'*',24,0);
		   password[input_num++]=0;
		}
		else if(flagsX4 == 0x00A){
		   if(input_num ==0);
			 else{
			  input_num--;
				LCD_Fill(15,10,304,59,WHITE);//input
				for(int i =0; i<input_num; i++)
				{
					LCD_ShowChar(290-20*i,30,'*',24,0);
				}
			}
		}
		else if(flagsX4 == 0x000C){
		   osThreadFlagsSet(judge_flag, 0x0001U);	
		}
		else; 
	}
	osThreadFlagsSet(show_flag, 0x0000U);
	}
}

void password_success(void *argument){
	while(1){
	flagsX5=osThreadFlagsWait(0x000FU, osFlagsWaitAny, osWaitForever);
	if(JudgePassword()){
		LCD_Clear(WHITE);
		LCD_ShowPicture(15+45,75+45,240, 240, (u8 *)gImage_right);
		}
	else{
		LCD_Clear(WHITE);
		LCD_ShowPicture(15+45,75+45,240, 240, (u8 *)gImage_wrong);
		}
	delay_ms(5000);	
	state=1;
  old_state=1; 
  input_num = 0;
	ui_setup();		
	}
}


void app_main(void *argument){
	key_flag = osThreadNew(find_key, NULL, NULL);
	button_flag = osThreadNew(find_button, NULL, NULL);
	focus_flag = osThreadNew(focus_button, NULL, NULL);
	show_flag = osThreadNew(input_show, NULL, NULL);
	judge_flag = osThreadNew(password_success, NULL, NULL);
	while(1);
}

int main(void){
	KEY_GPIO_Init();
	SysTick_Init(72);
    TFTLCD_Init();
    FRONT_COLOR=BLACK;
	ui_setup();
	
	SystemCoreClockUpdate();
	osKernelInitialize();
	osThreadNew(app_main, NULL, NULL);
	osKernelStart(); 
	
	while(1);
		
}








