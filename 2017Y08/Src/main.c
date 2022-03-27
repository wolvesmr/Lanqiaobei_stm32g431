#include "main.h"
#include "RCC\bsp_rcc.h"
#include "KEY_LED\bsp_key_led.h"
#include "LCD\bsp_lcd.h"
//#include "UART\bsp_uart.h"
//#include "I2C\bsp_i2c.h"
//#include "ADC\bsp_adc.h"
#include "TIM\bsp_tim.h"
#include "RTC\bsp_rtc.h"

//***全局变量声明区
//*减速变量
__IO uint32_t uwTick_Key_Set_Point = 0;//控制Key_Proc的执行速度
__IO uint32_t uwTick_Led_Set_Point = 0;//控制Led_Proc的执行速度
__IO uint32_t uwTick_Lcd_Set_Point = 0;//控制Lcd_Proc的执行速度
//__IO uint32_t uwTick_Usart_Set_Point = 0;//控制Usart_Proc的执行速度

//*按键扫描专用变量
uint8_t ucKey_Val, unKey_Down, ucKey_Up, ucKey_Old;

//*LED专用变量
uint8_t ucLed;

//*LCD显示专用变量
uint8_t Lcd_Disp_String[21];//最多显示20个字符

//*串口专用变量
//uint16_t counter = 0;
//uint8_t str[40];
//uint32_t rx_buffer;

//*EEPROM的相关变量
//uint8_t EEPROM_String_1[5] = {0x11,0x22,0x33,0x44,0x55};
//uint8_t EEPROM_String_2[5] = {0};

//*4017相关变量
//uint8_t RES_4017;

//*pwm相关变量
//uint16_t PWM_T_Count;
//uint16_t PWM_D_Count;
//float PWM_Duty;


//*rtc相关变量
RTC_TimeTypeDef H_M_S_Time;
RTC_DateTypeDef Y_M_D_Date;

//全局变量
uint8_t ucPlat = 1;//1234表示当前所处的平台
//_Bool PA4_Voltage = 0;
//_Bool PA5_Voltage = 0;
uint8_t ucSet;//用于记录几号按键按下去了。只用低4位,分别对应LD4~LD1，按键B4~B1， _ _ _ _  [ _ _ _ _ ].
uint8_t ucState;//状态机，0~8.
uint8_t Dir;//电梯运行方向变量 0 -没运行，1-上，2-下
uint8_t Flow = 0x10;//流水的变量
__IO uint32_t uwTick_Set_Point = 0;//计时专用





//***子函数声明区
void Key_Proc(void);
void Led_Proc(void);
void Lcd_Proc(void);
//void Usart_Proc(void);
void Elev_Proc(void);

//***系统主函数
int main(void)
{

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

	/*bsp资源的初始化*/
	KEY_LED_Init();
	
	LCD_Init();
	LCD_Clear(White);
  LCD_SetBackColor(White);
  LCD_SetTextColor(Blue);	
	
//	UART1_Init();
//	I2CInit();
//  ADC1_Init();
//  ADC2_Init();
//	
//	BASIC_TIM6_Init();
//	PWM_INPUT_TIM2_Init();
//	SQU_OUTPUT_TIM15_Init();
	PWM_OUTPUT_TIM3_Init();   // PA6     ------> TIM3_CH1
	PWM_OUTPUT_TIM17_Init();   //PA7     ------> TIM17_CH1
	RTC_Init();



	/*外设使用基本配置*/	
	//*EEPROM测试
//	iic_24c02_write(EEPROM_String_1, 0, 5);
//	HAL_Delay(1);	
//	iic_24c02_read(EEPROM_String_2, 0, 5);	
	
  //*MCP4017测试
//	write_resistor(0x77);
//	RES_4017 = read_resistor();	
	
	//*串口接收中断打开
//	HAL_UART_Receive_IT(&huart1, (uint8_t *)(&rx_buffer), 4);
	
	//*打开基本定时器
//	HAL_TIM_Base_Start_IT(&htim6);//每100ms触发一次中断
		
	//*输入捕获PWM启动
//	HAL_TIM_Base_Start(&htim2);  /* 启动定时器 */
//  HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_1);		  /* 启动定时器通道输入捕获并开启中断 */
//  HAL_TIM_IC_Start_IT(&htim2,TIM_CHANNEL_2);		
		
	//*输出方波PA2引脚	
//  HAL_TIM_OC_Start_IT(&htim15,TIM_CHANNEL_1);
	
	//*启动定时器3和定时器17通道输出
//	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 800);//修改占空比的基本操作 D=0.8
//  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);	//PA6   1khz  绿色波形
//	
//	__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, 250);//修改占空比的基本操作 D=0.5	
//  HAL_TIM_PWM_Start(&htim17,TIM_CHANNEL_1);		//PA7   2khz  黄色波形
	
	
  while (1)
  {
		Key_Proc();
		Led_Proc();
		Lcd_Proc();
		Elev_Proc();
//		Usart_Proc();
		
		
		
  }

}


//***按键扫描子函数
void Key_Proc(void)
{
	if((uwTick -  uwTick_Key_Set_Point)<50)	return;//减速函数
		uwTick_Key_Set_Point = uwTick;

	ucKey_Val = Key_Scan();
	unKey_Down = ucKey_Val & (ucKey_Old ^ ucKey_Val); 
	ucKey_Up = ~ucKey_Val & (ucKey_Old ^ ucKey_Val);	
	ucKey_Old = ucKey_Val;
	
//	if(unKey_Down == 4)
//	{
//		ucLed = 0x88;
//	}
//	if(unKey_Down == 3)
//	{
//		ucLed = 0x00;
//	}	
//	ucSet
//	ucPlat
	
	if(ucState == 0)
	{
		if(unKey_Down == 1)//B1按压
		{
			if(ucPlat != 1) ucSet |= 0x01; 
		}	
		else if(unKey_Down == 2)//B2按压
		{
			if(ucPlat != 2) ucSet |= 0x02; 
		}	
		else if(unKey_Down == 3)//B3按压
		{
			if(ucPlat != 3) ucSet |= 0x04; 	
		}		
		else if(unKey_Down == 4)//B4按压
		{
			if(ucPlat != 4) ucSet |= 0x08; 	
		}		
		
		ucLed &= 0xF0;
		ucLed |= ucSet;
		
		if(unKey_Down != 0)//当有按键按下去，启动计时
		{
			uwTick_Set_Point = uwTick;
		}
		
	}
}


//***LED扫描子函数
void Led_Proc(void)
{
	if((uwTick -  uwTick_Led_Set_Point)<200)	return;//减速函数
		uwTick_Led_Set_Point = uwTick;

	LED_Disp(ucLed);
}


void Lcd_Proc(void)
{
	if((uwTick -  uwTick_Lcd_Set_Point)<100)	return;//减速函数
		uwTick_Lcd_Set_Point = uwTick;
	
	//开机屏幕测试代码
	sprintf((char *)Lcd_Disp_String, " Current Platform",ucLed);
	LCD_DisplayStringLine(Line1, Lcd_Disp_String);	
	
	
	sprintf((char *)Lcd_Disp_String, "          %1d",(unsigned int)ucPlat);
	LCD_DisplayStringLine(Line3, Lcd_Disp_String);	

	//*EEPROM测试
//	sprintf((char *)Lcd_Disp_String, "EE:%x%x%x%x%x R:%x",EEPROM_String_2[0],EEPROM_String_2[1],EEPROM_String_2[2],EEPROM_String_2[3],EEPROM_String_2[4], RES_4017);
//	LCD_DisplayStringLine(Line0, Lcd_Disp_String);

	//*ADC测试
//	sprintf((char *)Lcd_Disp_String, "R38_Vol:%6.3fV",((((float)getADC1())/4096)*3.3));
//	LCD_DisplayStringLine(Line1, Lcd_Disp_String);	
//	
//	sprintf((char *)Lcd_Disp_String, "R37_Vol:%6.3fV",((((float)getADC2())/4096)*3.3));
//	LCD_DisplayStringLine(Line2, Lcd_Disp_String);	
	
	//*PWM输入捕获测试，测量占空比和频率
//	sprintf((char *)Lcd_Disp_String, "R40P:%05dHz,%4.1f%%",(unsigned int)(1000000/PWM_T_Count),PWM_Duty*100);
//	LCD_DisplayStringLine(Line3, Lcd_Disp_String);		
	
	//*RTC内容显示
	HAL_RTC_GetTime(&hrtc, &H_M_S_Time, RTC_FORMAT_BIN);//读取日期和时间必须同时使用
	HAL_RTC_GetDate(&hrtc, &Y_M_D_Date, RTC_FORMAT_BIN);
	sprintf((char *)Lcd_Disp_String, "       %02d-%02d-%02d",(unsigned int)H_M_S_Time.Hours,(unsigned int)H_M_S_Time.Minutes,(unsigned int)H_M_S_Time.Seconds);
	LCD_DisplayStringLine(Line6, Lcd_Disp_String);		


//	sprintf((char *)Lcd_Disp_String, " PA4:%1d  PA5:%1d",(unsigned int)PA4_Voltage,(unsigned int)PA5_Voltage);
//	LCD_DisplayStringLine(Line8, Lcd_Disp_String);	

}



void Elev_Proc(void)
{
	if(ucSet)//如果用户没有设置目标层，不执行此程序。只有按键设置了，才可以。
	{
		switch(ucState)
		{
			
			
			case 0://等待按键按下后1s时间的到来
				if((uwTick - uwTick_Set_Point) >= 1000)
					ucState = 1;
				else 
					break;
				
				
			case 1://控制电梯门关闭 PA7 -  PWM    2KHZ    D=50%     PA5= 0  
				
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);		//关门			
				__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, 250);//修改占空比的基本操作 D=0.5	
				HAL_TIM_PWM_Start(&htim17,TIM_CHANNEL_1);		//PA7   2khz  黄色波形		
			
				sprintf((char *)Lcd_Disp_String, " Door Closing          ");
				LCD_DisplayStringLine(Line8, Lcd_Disp_String);	
			
			  uwTick_Set_Point = uwTick;
				ucState = 2;
			
			
			case 2://等待电梯关门时间到达4s，到了之后，执行后边的操作，否则退出。
				if((uwTick - uwTick_Set_Point) >= 4000)
				{
					HAL_TIM_PWM_Stop(&htim17,TIM_CHANNEL_1);		//PA7   2khz  黄色波形	
					
					sprintf((char *)Lcd_Disp_String, " Door Closed          ");
					LCD_DisplayStringLine(Line8, Lcd_Disp_String);	
					
					ucState = 3;				
				}
				else 
					break;	
				
				
			case 3://判断当前层数变量和设定变量之间的关系，决定应该上行还是下行，并启动运行。
				//ucPlat=1~4=1(0001) 2(0010) 3(0100) 4(1000)      ucSet= 0010B
				if(ucSet > (1<<(ucPlat-1)))//上行 PA6  -  PWM    1KHZ   D=80%      PA4 = 1
				{
					Dir = 1;//上行
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);		//	电梯上行		
//					PA4_Voltage = 1;//为了屏幕测试上行方便
					__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 800);//修改占空比的基本操作 D=0.8
					HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);	//PA6   1khz  绿色波形
					
					sprintf((char *)Lcd_Disp_String, "Elev Upping          ");
					LCD_DisplayStringLine(Line8, Lcd_Disp_String);	
					
				}
				else if(ucSet < (1<<(ucPlat-1)))//下行  PA6  -  PWM    1KHZ    D=60%         PA4 = 0 
				{
					Dir = 2;//下行
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);		//	电梯下行		
//					PA4_Voltage = 0;//为了屏幕测试下行方便
					__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 600);//修改占空比的基本操作 D=0.6
					HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);	//PA6   1khz  绿色波形		

					sprintf((char *)Lcd_Disp_String, "Elev Downing           ");
					LCD_DisplayStringLine(Line8, Lcd_Disp_String);									
				}
				
			  uwTick_Set_Point = uwTick;
				ucState = 4;				
			
				
			case 4://判断电梯走6s时间是否到来，如果到来，当前所在层数变化1个单位。如果没有到6s，流水灯效果。
				if((uwTick - uwTick_Set_Point) >= 6000)
				{
					if(Dir == 1) ucPlat++;
					else if(Dir == 2) ucPlat--;
					
					sprintf((char *)Lcd_Disp_String, "          %1d",(unsigned int)ucPlat);
					LCD_DisplayStringLine(Line3, Lcd_Disp_String);					

					sprintf((char *)Lcd_Disp_String, "Elev Runned 1 Floor        ");
					LCD_DisplayStringLine(Line8, Lcd_Disp_String);						
					
					ucLed &=	0x0F;
					Flow = 0x10;
					ucState = 5;				
				}
				else 
				{
					if(Dir == 1)//上行，左到右
					{
						Flow = (Flow >> 1);//0001 0000  -  0000 1000
						if(Flow == 0x08)
							Flow = 0x80;
						ucLed &=	0x0F;
						ucLed |= Flow;
					}
					else if(Dir == 2)//下行，右到左
					{
						ucLed &=	0x0F;
						ucLed |= Flow;
						Flow = (Flow << 1);//1000 0000  -  0000 0000
						if(Flow == 0x00)
							Flow = 0x10;					
					}
					HAL_Delay(300);
					break;	
				}
				
				
				
				
			case 5://ucPlat当前层 3    ucSet 0000 1100
				if((1<<(ucPlat-1))&ucSet)//当不为零的时候，表示到了目标层
				{
					HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);	//PA6   1khz  绿色波形，停止波形的发生						

					HAL_Delay(300);					
					sprintf((char *)Lcd_Disp_String, "             ");
					LCD_DisplayStringLine(Line3, Lcd_Disp_String);	
					HAL_Delay(300);								
					sprintf((char *)Lcd_Disp_String, "          %1d",(unsigned int)ucPlat);
					LCD_DisplayStringLine(Line3, Lcd_Disp_String);						
					HAL_Delay(300);					
					sprintf((char *)Lcd_Disp_String, "             ");
					LCD_DisplayStringLine(Line3, Lcd_Disp_String);	
					HAL_Delay(300);								
					sprintf((char *)Lcd_Disp_String, "          %1d",(unsigned int)ucPlat);
					LCD_DisplayStringLine(Line3, Lcd_Disp_String);					
					
					//电梯开门，  PA7 -  PWM    2KHZ    D=60%          PA5= 1 。
					HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);		//开门			
//					PA5_Voltage = 1;//为了屏幕测试关门方便
					__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, 300);//修改占空比的基本操作 D=0.6
					HAL_TIM_PWM_Start(&htim17,TIM_CHANNEL_1);		//PA7   2khz  黄色波形		
					
					sprintf((char *)Lcd_Disp_String, "Comed , Door Opening            ");
					LCD_DisplayStringLine(Line8, Lcd_Disp_String);	
					
					uwTick_Set_Point = uwTick;
					ucState = 6;					
				}		
				else //没有到达目标层
				{
					uwTick_Set_Point = uwTick;
					ucState = 4;
					break;		
				}
				
			case 6://等待电梯开门4s时间是否到达
				if((uwTick - uwTick_Set_Point) >= 4000)
				{
					HAL_TIM_PWM_Stop(&htim17,TIM_CHANNEL_1);		//PA7   2khz  黄色波形	
					
					sprintf((char *)Lcd_Disp_String, "Door Opened           ");
					LCD_DisplayStringLine(Line8, Lcd_Disp_String);	
					
					ucSet &= (~(1<<(ucPlat-1)));//用户设置的目标层达成目标 1000
					ucLed &= 0xF0;
					ucLed |= ucSet;					
					LED_Disp(ucLed);					
					
					ucState = 7;				
				}
				else 
					break;	
				
				
			case 7://判别是否还有别的目标层
				if(ucSet)//如果还有别的目标平台，等待2秒
				{
					uwTick_Set_Point = uwTick;
					
					sprintf((char *)Lcd_Disp_String, "Waitting 2s           ");
					LCD_DisplayStringLine(Line8, Lcd_Disp_String);						
					
					ucState = 8;						
				}
				else //没有别的目标了
				{
					ucState = 0;
					
					sprintf((char *)Lcd_Disp_String, "                  ");
					LCD_DisplayStringLine(Line8, Lcd_Disp_String);	
					
					break;					
				}
			
				
			case 8://判断到达目标层之后，电梯开门后，是否等待了2s的时间已经到达。
				if((uwTick - uwTick_Set_Point) >= 2000)
				{		
					sprintf((char *)Lcd_Disp_String, "                  ");
					LCD_DisplayStringLine(Line8, Lcd_Disp_String);
					
					ucState = 1;										
				}	
		}
	
	}
}
















//void Usart_Proc(void)
//{
//	if((uwTick -  uwTick_Usart_Set_Point)<1000)	return;//减速函数
//	uwTick_Usart_Set_Point = uwTick;
//	
////	sprintf(str, "%04d:Hello,world.\r\n", counter);
////	HAL_UART_Transmit(&huart1,(unsigned char *)str, strlen(str), 50);
//	
////	if(++counter == 10000)
////		counter = 0;
//}


//串口接收中断回调函数
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
////	counter++;
////	sprintf(str, "%04d:Hello,world.\r\n", counter);
////	HAL_UART_Transmit(&huart1,(unsigned char *)str, strlen(str), 50);	
////	
////	HAL_UART_Receive_IT(&huart1, (uint8_t *)(&rx_buffer), 4);

//}



//基本定时器6更新回调函数
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//	if(htim->Instance==TIM6)
//  {
////		if(++counter == 10)
////		{
////			counter = 0;
////			sprintf(str, "Hello,world.\r\n");
////			HAL_UART_Transmit(&huart1,(unsigned char *)str, strlen(str), 50);	
////		}
//	}
//}


//输入捕获PWM中断回调
//void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
//{
//	  if(htim->Instance==TIM2)
//  {
//				if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
//			{
//				PWM_T_Count =  HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_1)+1;
//				PWM_Duty = (float)PWM_D_Count/PWM_T_Count;
//			}
//				else if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
//			{
//				PWM_D_Count =  HAL_TIM_ReadCapturedValue(htim,TIM_CHANNEL_2)+1;
//			}			
//	}	
//}




//方波输出回调函数
//void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
//{
//  if(htim->Instance==TIM15)
//  {
//				if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
//			{			
//			  __HAL_TIM_SET_COMPARE(htim,TIM_CHANNEL_1,(__HAL_TIM_GetCounter(htim)+500));//1Khz
//			}
//	}
//}







/******************************************************************************
/******************************************************************************
/******************************************************************************
/******************************************************************************
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
}
