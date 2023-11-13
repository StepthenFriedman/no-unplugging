/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int rtime = 30;   //��Ƴ�ʼʱ��
int gtime = 30;   //�̵Ƴ�ʼʱ��
int flag = 0;     //�ж�״̬����0��1
int distance;
//ʹ��TIM1����us����ʱ����,�˺���Ϊ1us
void TIM1_Delay_us(uint16_t n_us)
{
	__HAL_TIM_SetCounter(&htim1, 0);//htim1

	/* ������ʱ��1���� */
	__HAL_TIM_ENABLE(&htim1);

	//��ȡ��ʱ���ļ���ֵ���������жϼ���ֵ�����Ǽ���ֵ���ô��ڶ�ʱ�����õ���װ��ֵ��\
	���磺��ʱ���趨����װ��ֵΪ65536-1�� �Ƚ�ֵ��С����װ��ֵ����(����1�� * n_us)-1) С�ڣ�65536-1��\
																	  ���������Ĳ���n_us��ҪС��65536-1��	
	while(__HAL_TIM_GetCounter(&htim1) < ((1 * n_us)-1) );
	/* Disable the Peripheral */
	__HAL_TIM_DISABLE(&htim1);
}

//��ȡ������ģ��ļ�����
uint32_t car_csb_get_distance(void)  //����������
{
    uint32_t CSB_value = 0 ;
	//����������һ���ߵ�ƽ
	HAL_GPIO_WritePin(CTRIG_GPIO_Port, CTRIG_Pin, GPIO_PIN_SET);
	//��ʱ10us����
	TIM1_Delay_us(20);
	//����������һ���͵�ƽ
	HAL_GPIO_WritePin(CTRIG_GPIO_Port,CTRIG_Pin, GPIO_PIN_RESET);
	//�ȴ��������ű�ɸߵ�ƽ
	while( HAL_GPIO_ReadPin(CECHO_GPIO_Port,CECHO_Pin) == 0);
	//���ö�ʱ����ʼֵΪ0
	__HAL_TIM_SetCounter(&htim1, 0);
	//��ʼ��ʱ
	__HAL_TIM_ENABLE(&htim1);
	//������ȫ����Ϊ�ߵ�ƽ�������������ű�ɵ͵�ƽ��ֹͣ��ʱ����ȡ����ʱ��
    while( HAL_GPIO_ReadPin(CECHO_GPIO_Port,CECHO_Pin) == 1);  
	//��ȡ��ʱ���ļ���ֵ,��ֵ����  a = b;
	CSB_value = __HAL_TIM_GetCounter(&htim1);
	//ֹͣ��ʱ
	__HAL_TIM_DISABLE(&htim1);
	//��֪�ߵ�ƽ��ʱ�䣬�������ù�ʽ�� ���Ծ���= (�ߵ�ƽʱ��*����(340M/S))/2 �������㳬����ģ������ϰ���ĵ��̾��룻
	//�����Ҫ���� ���׼������,��ʽΪ�� ���Ծ���= (�ߵ�ƽʱ��*����(340M/1000mS))/2 ��
	int car_value;
	car_value  = CSB_value*340/10000/2;
	if(car_value > 50) return 50;
	else return car_value;
}
uint32_t man_csb_get_distance(void)  //���������
{
    uint32_t CSB_value = 0 ;
	//����������һ���ߵ�ƽ
	HAL_GPIO_WritePin(MTRIG_GPIO_Port, MTRIG_Pin, GPIO_PIN_SET);
	//��ʱ10us����
	TIM1_Delay_us(20);
	//����������һ���͵�ƽ
	HAL_GPIO_WritePin(MTRIG_GPIO_Port,MTRIG_Pin, GPIO_PIN_RESET);
	//�ȴ��������ű�ɸߵ�ƽ
	while( HAL_GPIO_ReadPin(MECHO_GPIO_Port,MECHO_Pin) == 0);
	//���ö�ʱ����ʼֵΪ0
	__HAL_TIM_SetCounter(&htim1, 0);
	//��ʼ��ʱ
	__HAL_TIM_ENABLE(&htim1);
	//������ȫ����Ϊ�ߵ�ƽ�������������ű�ɵ͵�ƽ��ֹͣ��ʱ����ȡ����ʱ��
    while( HAL_GPIO_ReadPin(MECHO_GPIO_Port,MECHO_Pin) == 1);  
	//��ȡ��ʱ���ļ���ֵ,��ֵ����  a = b;
	CSB_value = __HAL_TIM_GetCounter(&htim1);
	//ֹͣ��ʱ
	__HAL_TIM_DISABLE(&htim1);
	//��֪�ߵ�ƽ��ʱ�䣬�������ù�ʽ�� ���Ծ���= (�ߵ�ƽʱ��*����(340M/S))/2 �������㳬����ģ������ϰ���ĵ��̾��룻
	//�����Ҫ���� ���׼������,��ʽΪ�� ���Ծ���= (�ߵ�ƽʱ��*����(340M/1000mS))/2 ��
	int man_value;
	man_value = CSB_value*340/10000/2;
	if(man_value > 50) return 50;
	else return man_value;

}

int calc(void)  //ʱ��͵�������ģ��
{
	int m,c;
	m = 50 - man_csb_get_distance();
	HAL_Delay(100);
	c = 50 - car_csb_get_distance();
	HAL_Delay(100);
	if ((m > 0 && m <= 20) && (c > 30 && c <= 50))
	{
		rtime = 45;
		gtime = 15;
		HAL_UART_Transmit(&huart1, (uint8_t *)"��ǰ���ʱ��45�룬�̵�ʱ��15��\n", 30, 0xffff);
	}
	else if((m > 20 && m <= 50) && (c > 0 && c <= 30))
	{
		gtime = 45;
		rtime = 15;
		HAL_UART_Transmit(&huart1, (uint8_t *)"��ǰ���ʱ��15�룬�̵�ʱ��45��\n", 30, 0xffff);
	}
	else
	{
		rtime = 30;
		gtime = 30;
		HAL_UART_Transmit(&huart1, (uint8_t *)"��ǰ���ʱ��30�룬�̵�ʱ��30��\n", 30, 0xffff);
	}
	
}
	


int green(void) //�̵�
{
	flag = 0;
	HAL_GPIO_WritePin(RED_GPIO_Port,RED_Pin,0);
	HAL_GPIO_WritePin(GREEN_GPIO_Port,GREEN_Pin,1);
}
int red(void)  //���
{
	flag = 1;
	HAL_GPIO_WritePin(GREEN_GPIO_Port,GREEN_Pin,0);
	HAL_GPIO_WritePin(RED_GPIO_Port,RED_Pin,1);
}
int car_move(void)  //��⳵�������
{
	int d1,d2,re;
	d1 = car_csb_get_distance();
	HAL_Delay(100);
	d2 = car_csb_get_distance();
	HAL_Delay(100);
	if(d2 > d1) 
	{
		HAL_UART_Transmit(&huart1, (uint8_t *)"�г������", 10, 0xffff);
		re = 1;
	}
	else re = 0;
	return re;
}
int man_move(void) //������ള���
{
	int d3,d4,ret;
	d3 = man_csb_get_distance();
	HAL_Delay(100);
	d4 = man_csb_get_distance();
	HAL_Delay(100);
	if(d4 > d3)
	{
		HAL_UART_Transmit(&huart1, (uint8_t *)"���˴����", 10, 0xffff);
		ret = 1;
	}
	else ret = 0;
	return ret;
}


int blink(void)  //���3�����
{
	if(flag == 0)
	{
		int gtemp2 = 3;
		while(gtemp2 > 0 && gtemp2 <= 3)
		{
			HAL_GPIO_TogglePin(GREEN_GPIO_Port,GREEN_Pin);
			HAL_Delay(500);
			HAL_GPIO_TogglePin(GREEN_GPIO_Port,GREEN_Pin);
			HAL_Delay(500);
			gtemp2 -= 1;
		}
		red();
	}
	else
	{
		int rtemp2 = 3;
		while(rtemp2 > 0 && rtemp2 <= 3)
		{
			HAL_GPIO_TogglePin(RED_GPIO_Port,RED_Pin);
			HAL_Delay(500);
			HAL_GPIO_TogglePin(RED_GPIO_Port,RED_Pin);
			HAL_Delay(500);
			rtemp2 -= 1;
		}
		green();
	}
}

int bef_blink(void)  //ǰ27��
{
	if(flag == 0)
	{
		green();
		int gtemp1 = gtime;  //������ʱ�������Ͳ��ð�ʱ���������ȥ�����ú��޸ķ���
		while(gtemp1 > 3)
		{	
			HAL_Delay(800);
			gtemp1 -= 1;
			int carmove;
			carmove = car_move();
			while(carmove == 1 && gtemp1 > 3)
			{
				HAL_GPIO_TogglePin(GREEN_GPIO_Port,GREEN_Pin);
				HAL_Delay(250);
				HAL_GPIO_TogglePin(GREEN_GPIO_Port,GREEN_Pin);
				carmove = car_move();
				HAL_Delay(50);
				HAL_GPIO_TogglePin(GREEN_GPIO_Port,GREEN_Pin);
				HAL_Delay(250);
				HAL_GPIO_TogglePin(GREEN_GPIO_Port,GREEN_Pin);
				carmove = car_move();
				HAL_Delay(50);
				gtemp1 -= 1;
				
			}
			
			
		}
	}
	else
	{
		red();
		int rtemp1 = rtime;  //������ʱ�������Ͳ��ð�ʱ���������ȥ�����ú��޸ķ���
		while(rtemp1 > 3)
		{
			HAL_Delay(800);
			rtemp1 -= 1;
			int manmove;
			manmove = man_move();
			while(manmove == 1 && rtemp1 > 3)
			{
				HAL_GPIO_TogglePin(RED_GPIO_Port,RED_Pin);
				HAL_Delay(250);
				HAL_GPIO_TogglePin(RED_GPIO_Port,RED_Pin);
				manmove = man_move();
				HAL_Delay(50);
				HAL_GPIO_TogglePin(RED_GPIO_Port,RED_Pin);
				HAL_Delay(250);
				HAL_GPIO_TogglePin(RED_GPIO_Port,RED_Pin);
				manmove = man_move();
				HAL_Delay(50);
				rtemp1 -= 1;
				
			}
		}
	}
	blink();
}


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
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,0);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		calc();  //������ʱ���������
		
		bef_blink();  //�����ʱ

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
