/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DJCOM 4   //舵机补偿
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void MaxSpeedForward();
void setleftpwm(int pwmval);
void setrightpwm(int pwmval);
void pwmenable();
void setbothpwm(int pwmval);
void beein(int time);
void tim2enable();
void stop();
void addleftpwm(int pwmval);
void addrightpwm(int pwmval);
void addbothpwm(int);
void moveforward(int);
void turnleft(int);
void turnright(int);
void singlelinemode1();
void singlelinemode2();
void setdjpwm(int pwmval);
void setdjw(int w);
void delay_us(uint32_t us);
void starttiming();
int gettiming();

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
_Bool enablehwcjit=1;  //响应红外测距中断
int tim6reloadcnt=0;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  MX_TIM5_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
	pwmenable();
	tim2enable();
	
	uint8_t t;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  starttiming();
	  HAL_Delay(0);
	  t=gettiming()/1000;
	  HAL_UART_Transmit(&huart1,&t,1,10);
	  
	   starttiming();
	  HAL_Delay(10);
	  t=gettiming()/1000;
	  HAL_UART_Transmit(&huart1,&t,1,10);
	  
	   starttiming();
	  HAL_Delay(20);
	  t=gettiming()/1000;
	  HAL_UART_Transmit(&huart1,&t,1,10);
	  HAL_Delay(1000);

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void delay_us(uint32_t us)
{
    uint32_t delay = (HAL_RCC_GetHCLKFreq() / 4000000 * us);
    while (delay--)
	{
		;
	}
}
void pwmenable()
{
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim5,TIM_CHANNEL_1);
}
void setleftpwm(int pwmval)//-500<val<500 绝对值越大越快
{
	if(pwmval>=0)
	{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,0);
		__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3,pwmval);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,1);
		__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3,500+pwmval);
	}
}
void setrightpwm(int pwmval)//-500<val<500 绝对值越大越快
{
	if(pwmval>=0)
	{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,0);
		__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_4,pwmval);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,1);
		__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_4,500+pwmval);
	}
}
void stop()
{
	setbothpwm(0);
}
void setbothpwm(int pwmval)
{
	setleftpwm(pwmval);
	setrightpwm(pwmval);
}
void addleftpwm(int pwmval)
{
	int t=__HAL_TIM_GET_COMPARE(&htim4,TIM_CHANNEL_3)+pwmval;
	if(t>500)t=500;
	else if(t<-500)t=-500;
	__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3,t);
}
void addrightpwm(int pwmval)
{
	int t=__HAL_TIM_GET_COMPARE(&htim4,TIM_CHANNEL_4)+pwmval;
	if(t>500)t=500;
	else if(t<-500)t=-500;
	__HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_4,t);
}
void addbothpwm(int pwmval)
{
	addleftpwm(pwmval);
	addrightpwm(pwmval);
}
void setdjpwm(int pwmval)
{
	__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_1,pwmval);
}
void setdjw(int w)//-90<=w<=90 负右 正左
{
	w+=90+(DJCOM);
	//setdjpwm(249.975+w*5.555);
	setdjpwm(227.755+w*5.6785);
}

void beenoin(int time)
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,1);
	HAL_Delay(time);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,0);
}
void beein(int time)
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,1);
	__HAL_TIM_SET_COUNTER(&htim2,time-1);
}
void moveforward(int val)
{
	setbothpwm(val);
}
void turnleft(int val)
{
	setleftpwm(-135);
	setrightpwm(val);
}
void turnright(int val)
{
	setleftpwm(val);
	setrightpwm(-135);
}
void tr()
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,1);
	delay_us(15);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,0);
}
void starttiming()
{
	tim6reloadcnt=0;
	__HAL_TIM_SET_COUNTER(&htim6,0);
	HAL_TIM_Base_Start(&htim6);
}
int gettiming()
{
	HAL_TIM_Base_Stop(&htim6);
	return __HAL_TIM_GET_COUNTER(&htim6)+tim6reloadcnt*65535;
}

void singlelinemode1()
{
	 //1有黑线 0无黑线 pb0左 pa7右
	while(1)
	{
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0))
		{
			if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7))
			{
				stop();
			}
			else
			{
				turnleft(250);
			}
		}
		else
		{
			if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7))
			{
				turnright(250);
			}
			else
			{
				moveforward(150);
			}
		}
	}
}
void singlelinemode2()
{
	int state=0;
	while(1)
	{
		switch(state)
	  {
		  case 0:
			  if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0))
			  {
				  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7))
				  {
					  state=5;
				  }
				  else
				  {
					state=1;
					moveforward(200);
					break;
				  }
			  }
			  else if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7))
			  {
				  state=2;
				  moveforward(200);
				  break;
			  }
			  break;
		  case 1:
			  if(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0))
			  {
				  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7))
				  {
					 state=2; 	
					 break;					  
				  }
				  else
				  {
					  state=3;
					  turnleft(200);
					  break;
				  }	  
			  }
			  else if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7))
			  {
				  /*
				  state=5;
				  stop();
				  break;
				  */
				  
			  }
			  break;
		  case 2:
			  if(!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7))
			  {
				  if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0))
				  {
					  state=1;
					  break;
				  }
				  else
				  {
					  state=4;
					  turnright(200);
					  break;
				  }
			  }
			  else if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0))
			  {
				  /*
				  state=5;
				  stop();
				  break;
				  */
				  state=1;
			  }
			  break;
		  case 3:
		  case 4:
			  if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0))
			  {
				  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7))
				  {
					  state=5;
					  stop();
					  break;
				  }
				  else
				  {
					  state=1;
					  moveforward(200);
					  break;
				  }					  
			  }
			  else
			  {
				  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7))
				  {
					  state=2;
					  moveforward(200);
					  break;
				  }
				  break;
			  }
			  break;
		  case 5:
			  if(!HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0))
			  {
				  if(!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7))
				  {
					  state=0;
					  break;
				  }
				  else
				  {
					  state=2;
					  moveforward(200);
					  break;
				  }
			  }
			  else
			  {
				  if(!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_7))
				  {
					  state=1;
					  moveforward(200);
					  break;
				  }
			  }
			  break;
		  default:
				  break;
			  
			  
	  }		
	}
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim==&htim2)//stop bee
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,0);
	}
	else if(htim==&htim6)//计时器溢出
	{
		tim6reloadcnt+=1;
	}
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin==GPIO_PIN_1&&enablehwcjit)//红外测距
	{
		 HAL_Delay(20); // 延时一小段时间，消除抖动 
		if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==0)
		{
			
        }
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_1);
	}
	if(GPIO_Pin==GPIO_PIN_2)//按键
	{
		HAL_Delay(20);
		if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_2)==1)
		{
			
			
		}
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);
	}
	
}

void tim2enable()
{
	HAL_TIM_Base_Start_IT(&htim2);
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
