#include "main.h"


extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim15;
extern TIM_HandleTypeDef htim17;


void PWM_INPUT_TIM2_Init(void);
void PWM_OUTPUT_TIM3_Init(void);
void BASIC_TIM6_Init(void);
void SQU_OUTPUT_TIM15_Init(void);
void PWM_OUTPUT_TIM17_Init(void);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);