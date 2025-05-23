/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "spi.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "linear_actuator_12vdc.h"
#include "nrf24l01p.h"
#include <string.h>
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
typedef enum
{
    Open,
    Closed,
	Not_Init
} DoorStatus_t;

DoorStatus_t door_status, pending_door_status;
uint8_t rx_data[NRF24L01P_PAYLOAD_LENGTH];
uint8_t motor_moving = 0;
uint8_t command_pending = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Receiver_Interrupt()
{
	nrf24l01p_rx_receive(rx_data);
	rx_data[NRF24L01P_PAYLOAD_LENGTH - 1] = '\0';
	char* message = (char*)rx_data;

	if(!motor_moving)
	{
	// if message contains "open", pending_door_status = open, message contains "closed", pending_door_status=closed
		if (strstr(message, "open") != NULL && door_status != Open) {
			pending_door_status = Open;
			command_pending = 1;
		} else if (strstr(message, "closed") != NULL && door_status != Closed) {
			pending_door_status = Closed;
			command_pending = 1;
		}
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == RF_IRQ_Pin)
    {
        // Your code when RF_IRQ (PB1) interrupt happens
    	Receiver_Interrupt();
    }
}

void Open_Coop_Door()
{
	 // if we get an interrupt during this period, we want to ignore it
	Move_Backward();
	HAL_Delay(21000);
	Stop_Motor();
	HAL_Delay(10);
}

void Close_Coop_Door()
{
	Move_Forward();
	HAL_Delay(21000); // TODO stop here in the middle to avoid decapitations
	Stop_Motor();
	HAL_Delay(10);
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


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
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

	door_status = Not_Init;
	pending_door_status = Open;

	nrf24l01p_rx_init(2576, _1Mbps);
	HAL_Delay(1000);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

	uint8_t status = nrf24l01p_get_status();
	while (1)
	{
		/*if(door_status != pending_door_status)
		{
		  if(pending_door_status == Open && !motor_moving)
		  {
			  Open_Coop_Door();
			  door_status = Open;
		  }
		  else if (!motor_moving)
		  {
			  Close_Coop_Door();
			  door_status = Closed;
		  }
		}*/
		if(command_pending && !motor_moving)
		{
			if(pending_door_status == Open)
			{
				motor_moving = 1;
				command_pending = 0;
				Open_Coop_Door();
				door_status = Open;
				motor_moving = 0;
			}
			else if(pending_door_status == Closed)
			{
				motor_moving = 1;
				command_pending =  0;
				Close_Coop_Door();
				door_status = Closed;
				motor_moving = 0;
			}
		}

		__WFI();

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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI48;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV4;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
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
