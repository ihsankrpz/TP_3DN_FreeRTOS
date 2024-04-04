/* USER CODE BEGIN Header */
	/**
	******************************************************************************
	* @file           : main.c
	* @brief          : Main program body
	******************************************************************************
	* @attention
	*
	* Copyright (c) 2024 STMicroelectronics.
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
#include "cmsis_os.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include "shell.h"
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
int fonction(int argc, char ** argv);
int addition (int argc, char ** argv);
int led(int argc, char ** argv);
int spam(int argc, char ** argv);

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
////////////////////////////////////////////////////////////////////////////////////
// DECLARATIONS
////////////////////////////////////////////////////////////////////////////////////

//SemaphoreHandle_t xSemaphore;
QueueHandle_t QueueHandle;
UBaseType_t uxQueueLength = 10, uxItemSize = sizeof(int);
TaskHandle_t handle_blink_led, handle_echo_uart, handle_givetask,
			handle_taketask, handle_shell, handle_spam, handle_bidon;
static int delayLed = 0;
static int delaymsg = 0;
char * msg = "test";
//static int count_bidon = 0;

////////////////////////////////////////////////////////////////////////////////////
// CALLBACKS
////////////////////////////////////////////////////////////////////////////////////

int __io_putchar(int chr) // pour faire des printf parce que printf appelLE cette fonction
{
	HAL_UART_Transmit(&huart1, (uint8_t *)&chr, 1, HAL_MAX_DELAY);

	return chr;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1) // on s'assure que c'est l'uart1
	{
		BaseType_t xHigherPriorityTaskWoken; //
		vTaskNotifyGiveFromISR( handle_shell, &xHigherPriorityTaskWoken );

		portYIELD_FROM_ISR( xHigherPriorityTaskWoken ); //
	}

}

////////////////////////////////////////////////////////////////////////////////////
// TASKS
////////////////////////////////////////////////////////////////////////////////////

void task_blink_led(void * unused)
{
	for(;;)
	{
	  if(delayLed != 0 )
	  {
		  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		  vTaskDelay(portTICK_PERIOD_MS*delayLed);  }
	  else
	  {
		  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin,GPIO_PIN_RESET);
		  vTaskSuspend(handle_blink_led);
	  }
	}
}

void shell(void * unused)
{
	shell_init();
	shell_add('f', fonction, "Une fonction inutile");
	shell_add('a', addition, "Add 2 value");
	shell_add('l', led, "Change led delay (ms)");
	shell_add('s', spam, "spam message");

	shell_run();
}

void task_spam(void *unused)
{
	for(;;)
	{
		if(delaymsg !=0)
		{
			printf("%s \r\n", msg);
			vTaskDelay(portTICK_PERIOD_MS*delaymsg);
		}
		else
		{
			vTaskSuspend(handle_spam);
		}
	}
}

void task_bidon(void * unused)
{
	int tableau_enorme[1000];
	int i = 0;
	for(;;)
	{
		tableau_enorme[i] = i;
		i++;
		printf("tableau_enorme[%d] = %d \r\n",i, tableau_enorme[i]);
	}
}

//void task_uart_com_echo(void * unused)
//{
//	for(;;)
//	{
//		char pData;
//		HAL_UART_Receive_IT(&huart1, (uint8_t *)&pData, 1);
//
//		ulTaskNotifyTake( pdTRUE, portMAX_DELAY ); // on admet
//
//		HAL_UART_Transmit(&huart1, (uint8_t *)&pData, 1, HAL_MAX_DELAY);
//
//	}
//}

//void task_give(void * unused)
//{
////	BaseType_t xHigherPriorityTaskWoken; //
//	int q_value_send;
//
//	for(;;)
//	{
//		//semaphore part
////		printf("Task give before give\r\n");
////		xSemaphoreGive(xSemaphore);
////		printf("Task give after give\r\n");
////		vTaskDelay(portTICK_PERIOD_MS*100);
//
//		//notification part
////		for (int i = 0; i < 12; i++)
////		{
////			printf("Task give before give\r\n");
////			xTaskNotifyGive( handle_taketask );
////			printf("Task give after give\r\n");
////			vTaskDelay(portTICK_PERIOD_MS*100*i);
////		}
//
//
//		//queue
//
//		for (int i = 0; i < 20; i++)
//		{
//			q_value_send = i;
//			xQueueSend(QueueHandle, &q_value_send ,portMAX_DELAY);
//			vTaskDelay(portTICK_PERIOD_MS*100*i);
//		}
//
//
//	}
//}

//void task_take(void * unused)
//{
////	uint32_t ret;
//	int q_value_receive;
//	BaseType_t ret_q;
//
//	for(;;)
//	{
//		//semaphore part
////		printf("Task take before take\r\n");
////		xSemaphoreTake(xSemaphore, portMAX_DELAY);
////		printf("Task take after take\r\n");
//
//		//notifcation part
////		printf("Task take before take\r\n");
////		ret = ulTaskNotifyTake(pdTRUE, 1000);
////		if(ret != pdTRUE)
////		{
////			NVIC_SystemReset();
////			printf("System Reset\r\n");
////		}
////		printf("Task take after take\r\n");
//
//		//queue
//		ret_q = xQueueReceive(QueueHandle, &q_value_receive ,1000);
////		if(ret_q != pdTRUE)
////		{
////			NVIC_SystemReset();
////			printf("System Reset\r\n");
////		}
////		printf("Received %d\r\n", q_value_receive);
//
//	}
//}

////////////////////////////////////////////////////////////////////////////////////
// SHELL FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////

int fonction(int argc, char ** argv)
{
	printf("argc = %d\r\n", argc);

	for(int itr = 0; itr < argc; itr++)
	{
		printf("argv[%d] = %s\r\n", itr, argv[itr]);
	}

	return 0;
}

int addition (int argc, char ** argv)
{
	//récupérer deux paramètre ni plus ni moins
	//convertir les deux paramètres en nombres entier
	//les additioners et afficher le résultat

	if(argc != 3)
	{
		//faire planter proprement si on a plus de deux paramètre
		printf("Error, excpected 2 argument\r\n");
		return -1;
	}
	else
	{
		int a,b, result;
		// permet de récupérer les deux paramètres et de les convertir en entier
		a = atoi(argv[1]);
		b = atoi(argv[2]);

		result = a+b;
		printf("%d + %d = %d\r\n", a, b, result);
	}

	return 0;
}

int led(int argc, char ** argv)
{
	if(argc != 2)
	{
		//faire planter proprement si on a plus de deux paramètre
		printf("Error, excpected 1 argument\r\n");
		return -1;
	}
	else
	{
		delayLed = atoi(argv[1]);

		if(delayLed != 0)
		{
			vTaskResume(handle_blink_led);
		}
	}

	return 0;
}

int spam(int argc, char ** argv)
{
	if(argc != 3)
	{
		//faire planter proprement si on a plus de deux paramètre
		printf("Error, excpected 2 argument\r\n");
		return -1;
	}
	else
	{
	    delaymsg = atoi(argv[1]);
		msg = argv[2];

		if(delaymsg != 0)
		{
			vTaskResume(handle_spam);
		}
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////////
// OWERFLOW CHECK
////////////////////////////////////////////////////////////////////////////////////

void vApplicationStackOverflowHook( TaskHandle_t xTask, signed char *pcTaskName )
{
	for(;;)
	{
		HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		vTaskDelay(portTICK_PERIOD_MS*100);
	}
}



////////////////////////////////////////////////////////////////////////////////////
// MAIN
////////////////////////////////////////////////////////////////////////////////////

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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  BaseType_t ret;

  printf("\r\n================================================================\r\n");

//  xSemaphore = xSemaphoreCreateBinary();
  QueueHandle =  xQueueCreate ( uxQueueLength, uxItemSize);

  if(QueueHandle == NULL)
  {
	  printf("Error creating Queue \r\n");
	  Error_Handler();
  }

  ////////////////////////////////////////////////////////////////////////////////////
  // TASK CREATIONS
  ////////////////////////////////////////////////////////////////////////////////////

  //TASK LED
  ret = xTaskCreate(  task_blink_led,
  					"Blink LED",
  					256, // Taille de la pile (en mots de 32 bits)
  					NULL, //paramètre non utilisé ici
  					5, //Prio
  					&handle_blink_led
  				   );
    if(ret != pdPASS)
    {
  	  printf("Error creating Blink LED task\r\n");
  	  Error_Handler();
    }

//    //TASK UART
//    ret = xTaskCreate(  task_uart_com_echo,
//  					"Echo UART",
//  					256, // Taille de la pile (en mots de 32 bits)
//  					NULL, //paramètre non utilisé ici
//  					5, //Prio
//  					&handle_echo_uart
//  				   );
//
//    if(ret != pdPASS)
//    {
//  	  printf("Error creating Echo UART task\r\n");
//  	  Error_Handler();
//    }

//    //TASK GIVE
//    ret = xTaskCreate(  task_give,
//  					"Task Give",
//  					256, // Taille de la pile (en mots de 32 bits)
//  					NULL, //paramètre non utilisé ici
//  					6, //Prio
//  					&handle_givetask
//  				   );
//
//    if(ret != pdPASS)
//    {
//  	  printf("Error creating Task Give task\r\n");
//  	  Error_Handler();
//    }
//
//    //TASK Take
//    ret = xTaskCreate(  task_take,
//    				"Task Take",
//  					256, // Taille de la pile (en mots de 32 bits)
//  					NULL, //paramètre non utilisé ici
//  					7, //Prio
//  					&handle_taketask
//  				   );
//
//    if(ret != pdPASS)
//    {
//  	  printf("Error creating Task Take task\r\n");
//  	  Error_Handler();
//    }

    //TASK Shell
    ret = xTaskCreate(  shell,
    				"SHELL",
  					256, // Taille de la pile (en mots de 32 bits)
  					NULL, //paramètre non utilisé ici
  					10, //Prio
  					&handle_shell
  				   );

    if(ret != pdPASS)
    {
  	  printf("Error creating Task Shell task\r\n");
  	  Error_Handler();
    }

    //TASK spam
    ret = xTaskCreate(  task_spam,
    				"SPAM",
  					256, // Taille de la pile (en mots de 32 bits)
  					NULL, //paramètre non utilisé ici
  					5, //Prio
  					&handle_spam
  				   );

    if(ret != pdPASS)
    {
  	  printf("Error creating Task spam task\r\n");
  	  Error_Handler();
    }

    //TASK BIDON
    ret = xTaskCreate(  task_bidon,
						"bidon",
						256, // Taille de la pile (en mots de 32 bits)
						NULL, //paramètre non utilisé ici
						15, //Prio
						&handle_bidon
					   );

	if(ret != pdPASS)
	{
	  printf("Error creating Task bidon task\r\n");
	  Error_Handler();
	}
//
//    while(1)
//    {
//    	ret = xTaskCreate(  task_bidon,
//    	    				"bidon",
//    	  					256, // Taille de la pile (en mots de 32 bits)
//    	  					NULL, //paramètre non utilisé ici
//    	  					15, //Prio
//    	  					&handle_bidon
//    	  				   );
//
//		if(ret != pdPASS)
//		{
//		  printf("Error creating Task bidon task\r\n");
//		  Error_Handler();
//		}
//		printf("create task %d \r\n", count_bidon);
//		count_bidon ++;
//    }


    printf("Task Creation Successful\r\n");

    ////////////////////////////////////////////////////////////////////////////////////
    // SCHEDULER START
    ////////////////////////////////////////////////////////////////////////////////////

    vTaskStartScheduler(); //Boucle Infinie, rien ne s'execute apres !!!


  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
