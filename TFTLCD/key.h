/* key0 define */
#define KEY0                            		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)
#define KEY0_GPIO_Pin                   		GPIO_Pin_4
#define KEY0_GPIO_Port                  		GPIOE
#define KEY0_RCC_APB2Periph_CLK     				RCC_APB2Periph_GPIOE

/* key1 define */
#define KEY1                            		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)
#define KEY1_GPIO_Pin                   		GPIO_Pin_3
#define KEY1_GPIO_Port                  		GPIOE
#define KEY1_RCC_APB2Periph_CLK    	 				RCC_APB2Periph_GPIOE

/* key2 define */
#define KEY2                           			GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)
#define KEY2_GPIO_Pin                   		GPIO_Pin_2
#define KEY2_GPIO_Port                  		GPIOE
#define KEY2_RCC_APB2Periph_CLK    	 				RCC_APB2Periph_GPIOE

/* key_up define */
#define WK_UP                           	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)
#define KEY_UP_GPIO_Pin                		GPIO_Pin_0
#define KEY_UP_GPIO_Port              		GPIOA
#define KEY_UP_RCC_APB2Periph_CLK  				RCC_APB2Periph_GPIOA

typedef enum _KEY_STA_t KEY_STA_t;
enum _KEY_STA_t {
  KEY0_PRESS = 1,
  KEY1_PRESS,
	KEY2_PRESS,
  WK_UP_PRESS
};
