#include "main.h"
#include "RCC\bsp_rcc.h"
#include "KEY_LED\bsp_key_led.h"
#include "LCD\bsp_lcd.h"
//#include "UART\bsp_uart.h"
//#include "I2C\bsp_i2c.h"
#include "ADC\bsp_adc.h"
#include "TIM\bsp_tim.h"
//#include "RTC\bsp_rtc.h"

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
//RTC_TimeTypeDef H_M_S_Time;
//RTC_DateTypeDef Y_M_D_Date;


//全局变量
_Bool Lcd_Show_State;//0- 数据界面，1- 参数界面
uint8_t PWM_Running_State = 2;
float R37_Voltage;
uint8_t PA6_Duty_Int = 10;
uint8_t PA7_Duty_Int = 10;





//***子函数声明区
void Key_Proc(void);
void Led_Proc(void);
void Lcd_Proc(void);
//void Usart_Proc(void);


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
	LCD_Clear(Black);
  LCD_SetBackColor(Black);
  LCD_SetTextColor(White);	
	
//	UART1_Init();
//	I2CInit();
//  ADC1_Init();
  ADC2_Init();
//	
//	BASIC_TIM6_Init();
//	PWM_INPUT_TIM2_Init();
//	SQU_OUTPUT_TIM15_Init();
	PWM_OUTPUT_TIM3_Init();
	PWM_OUTPUT_TIM17_Init();
//	RTC_Init();



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
	
	//PA6绿色
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 1000);
		__HAL_TIM_SET_AUTORELOAD(&htim3, 9999);	
	//PA7黄色
		__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, 500);
		__HAL_TIM_SET_AUTORELOAD(&htim17, 4999);		
	
	//*启动定时器3和定时器17通道输出
  HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);	//PA6
  HAL_TIM_PWM_Start(&htim17,TIM_CHANNEL_1);		//PA7
		
//		__HAL_TIM_SET_COMPARE(__HANDLE__, __CHANNEL__, __COMPARE__)
//		__HAL_TIM_SET_AUTORELOAD(__HANDLE__, __AUTORELOAD__)

		
		
		
		
  while (1)
  {
		Key_Proc();
		Led_Proc();
		Lcd_Proc();
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

	switch(unKey_Down)
	{
		case 1://B1
			Lcd_Show_State ^= 1;
			LCD_Clear(Black);
		break;
		
		case 2://B2
			if(Lcd_Show_State == 1)//当处于参数界面
			{
				PA6_Duty_Int += 10;
				if(PA6_Duty_Int == 100) PA6_Duty_Int = 10;			
			}
		break;		
	
		case 3://B3   //0- 数据界面，1- 参数界面
			if(Lcd_Show_State == 0)//当处于数据界面
			{
				if(PWM_Running_State == 0) PWM_Running_State = 1;
				else if(PWM_Running_State == 1) PWM_Running_State = 0;
				else if(PWM_Running_State == 2) PWM_Running_State = 3;
				else if(PWM_Running_State == 3) PWM_Running_State = 2;	
			}
			
			if(Lcd_Show_State == 1)//当处于参数界面
			{
				PA7_Duty_Int += 10;
				if(PA7_Duty_Int == 100) PA7_Duty_Int = 10;			
			}
		break;	
	
		case 4://B4
			if(PWM_Running_State == 0) PWM_Running_State = 2;
			else if(PWM_Running_State == 2) PWM_Running_State = 0;
			else if(PWM_Running_State == 1) PWM_Running_State = 3;
			else if(PWM_Running_State == 3) PWM_Running_State = 1;		
		break;
	}
	
	
	
	
	
	
}


//***LED扫描子函数
void Led_Proc(void)
{
	if((uwTick -  uwTick_Led_Set_Point)<200)	return;//减速函数
		uwTick_Led_Set_Point = uwTick;

	if(PWM_Running_State >= 2)//自动模式
	{	
		ucLed |= 0x01;//LD1点亮
	}
	else //手动模式
	{
		ucLed &= (~0x01);//LD1熄灭	
	}
	
	if(Lcd_Show_State)//参数界面
	{
		ucLed &= (~0x02);//LD2熄灭		
	}
	else//数据界面
	{
		ucLed |= 0x02;//LD2点亮	
	}
	
	
	
	LED_Disp(ucLed);
}


void Lcd_Proc(void)
{
	if((uwTick -  uwTick_Lcd_Set_Point)<100)	return;//减速函数
		uwTick_Lcd_Set_Point = uwTick;
	//数据采集区
	R37_Voltage = ((((float)getADC2())/4096)*3.3);
	
	
	//我的显示
	if(Lcd_Show_State == 0)//数据界面
	{
		sprintf((char *)Lcd_Disp_String, "      Data");
		LCD_DisplayStringLine(Line0, Lcd_Disp_String);	
		
		sprintf((char *)Lcd_Disp_String, "    V:%4.2fV",R37_Voltage);
		LCD_DisplayStringLine(Line2, Lcd_Disp_String);		
	
		if(PWM_Running_State <= 1)
		{
			sprintf((char *)Lcd_Disp_String, "    Mode:MANU");
			LCD_DisplayStringLine(Line4, Lcd_Disp_String);				
		}
		else
		{
			sprintf((char *)Lcd_Disp_String, "    Mode:AUTO");
			LCD_DisplayStringLine(Line4, Lcd_Disp_String);				
		}			
	}
	else //参数界面
	{
		sprintf((char *)Lcd_Disp_String, "      Para");
		LCD_DisplayStringLine(Line0, Lcd_Disp_String);		
	
		sprintf((char *)Lcd_Disp_String, "    PA6:%02d%%",(unsigned int)PA6_Duty_Int);
		LCD_DisplayStringLine(Line2, Lcd_Disp_String);	

		sprintf((char *)Lcd_Disp_String, "    PA7:%02d%%",(unsigned int)PA7_Duty_Int);
		LCD_DisplayStringLine(Line4, Lcd_Disp_String);		
	}
		
	
	
	//数据输出区
	
		switch(PWM_Running_State)
		{
			case 0:
					__HAL_TIM_SET_AUTORELOAD(&htim3, 9999);	//PA6（绿色）的频率为100hz
					__HAL_TIM_SET_AUTORELOAD(&htim17, 4999);//PA7（黄色）的频率为200hz
			
					__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (100*PA6_Duty_Int));//PA6（绿色）的占空比跟随设置
					__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, (50*PA7_Duty_Int));//PA7（黄色）的占空比跟随设置			
			break;
		
			case 1:
					__HAL_TIM_SET_AUTORELOAD(&htim3, 4999);	//PA6（绿色）的频率为200hz
					__HAL_TIM_SET_AUTORELOAD(&htim17, 9999);//PA7（黄色）的频率为100hz
			
					__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (50*PA6_Duty_Int));//PA6（绿色）的占空比跟随设置
					__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, (100*PA7_Duty_Int));//PA7（黄色）的占空比跟随设置			
			break;		
		
			case 2:
					__HAL_TIM_SET_AUTORELOAD(&htim3, 9999);	//PA6（绿色）的频率为100hz
					__HAL_TIM_SET_AUTORELOAD(&htim17, 4999);//PA7（黄色）的频率为200hz
			
					__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (unsigned int)(10000*(R37_Voltage/3.3)));//PA6（绿色）的占空比跟随设置
					__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, (unsigned int)(5000*(R37_Voltage/3.3)));//PA7（黄色）的占空比跟随设置			
			break;			
		
			case 3:
					__HAL_TIM_SET_AUTORELOAD(&htim3, 4999);	//PA6（绿色）的频率为200hz
					__HAL_TIM_SET_AUTORELOAD(&htim17, 9999);//PA7（黄色）的频率为100hz
			
					__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (unsigned int)(5000*(R37_Voltage/3.3)));//PA6（绿色）的占空比跟随设置
					__HAL_TIM_SET_COMPARE(&htim17, TIM_CHANNEL_1, (unsigned int)(10000*(R37_Voltage/3.3)));//PA7（黄色）的占空比跟随设置			
			break;				
			
		}
	
	
	
	
	//开机屏幕测试代码
//	sprintf((char *)Lcd_Disp_String, "PWM-%1d LCD-%1d",(unsigned int)PWM_Running_State, (unsigned int)Lcd_Show_State);
//	LCD_DisplayStringLine(Line9, Lcd_Disp_String);	
	
	//*EEPROM测试
//	sprintf((char *)Lcd_Disp_String, "EE:%x%x%x%x%x R:%x",EEPROM_String_2[0],EEPROM_String_2[1],EEPROM_String_2[2],EEPROM_String_2[3],EEPROM_String_2[4], RES_4017);
//	LCD_DisplayStringLine(Line0, Lcd_Disp_String);

	//*ADC测试
//	sprintf((char *)Lcd_Disp_String, "R38_Vol:%6.3fV",((((float)getADC1())/4096)*3.3));
//	LCD_DisplayStringLine(Line1, Lcd_Disp_String);	
	
//	sprintf((char *)Lcd_Disp_String, "R37_Vol:%6.3fV",((((float)getADC2())/4096)*3.3));
//	LCD_DisplayStringLine(Line2, Lcd_Disp_String);	
	
	//*PWM输入捕获测试，测量占空比和频率
//	sprintf((char *)Lcd_Disp_String, "R40P:%05dHz,%4.1f%%",(unsigned int)(1000000/PWM_T_Count),PWM_Duty*100);
//	LCD_DisplayStringLine(Line3, Lcd_Disp_String);		
	
	//*RTC内容显示
//	HAL_RTC_GetTime(&hrtc, &H_M_S_Time, RTC_FORMAT_BIN);//读取日期和时间必须同时使用
//	HAL_RTC_GetDate(&hrtc, &Y_M_D_Date, RTC_FORMAT_BIN);
//	sprintf((char *)Lcd_Disp_String, "Time:%02d-%02d-%02d",(unsigned int)H_M_S_Time.Hours,(unsigned int)H_M_S_Time.Minutes,(unsigned int)H_M_S_Time.Seconds);
//	LCD_DisplayStringLine(Line4, Lcd_Disp_String);		

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
