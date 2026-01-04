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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdio.h"
#include "stdbool.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define WAIT_TIME 50
#define SEND_MASS "AT+CMGS=\"+989938879952\"\r\n"
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
// Flags
//-------------
bool flag_sensor;
bool flag_inc_pass;
bool flag_c_pass;
bool Ready_flag;
bool access_granted;
bool access_denied;
//---------------
uint16_t Counter = 0;
uint16_t Count_det = 0;
char buffer[100]; // For AT Command
char EPass_c[10];
char pass_c;
char Main_pass[] = "1934" ;
uint8_t pass_inx = 0;
int keypad_code = 0;
char matrix[16] =   {'3','#','9','6','A','D','C','B','1','*', '7', '4', '2', '0', '8', '5'};
//char matrix[16] = {'0','1','2','3','4','5','6','7','8','9','10','11','12','13','14','15'};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Send_AT_Command(char *command) {
  HAL_UART_Transmit(&huart1, (uint8_t *)command, strlen(command), 1000);
}
void Keypad(void){
    if (pass_c) {
        if (pass_c >= '0' && pass_c <= '9') {
            if (pass_inx < 15) {
                EPass_c[pass_inx++] = pass_c;
            }
        }
        else if (pass_c == 'D') {
            flag_c_pass = 1;
            for (int i = 0; i < strlen(Main_pass); i++) {
                if (EPass_c[i] != Main_pass[i]) {
                    flag_c_pass = 0;
                    break;
                }
            }

            if (flag_c_pass && pass_inx == strlen(Main_pass)) {
                access_granted = 1;
            }
            else {
                access_denied = 1;
            }
            pass_inx = 0;
        }
        else if (pass_c == 'A') {
            memset(EPass_c, 0, sizeof(EPass_c));
            pass_inx = 0;
            access_granted = 0;
            access_denied = 0;
        }


        pass_c = 0;

        HAL_NVIC_EnableIRQ(EXTI4_IRQn);
    }
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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  	 Ready_flag = 1;
  	 HAL_Delay(5000);
  	 Send_AT_Command("AT\r\n");
  	 Ready_flag = 2;
  	 HAL_Delay(500);
  	 Send_AT_Command("AT+CMGF=1\r\n");
  	 Ready_flag = 3;
  	 HAL_Delay(500);






  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0))
	  {
		  HAL_Delay(70);
		  if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)){
		  flag_sensor = 1;
		  HAL_Delay(600);
		  //if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)){
		  Send_AT_Command(SEND_MASS);
		  HAL_Delay(100);
		  Send_AT_Command("Alarm! Sensor Detect a motion.\x1A");
		  Count_det ++;
		  flag_sensor = 0;
		  HAL_Delay(3500);

		  }
	  }
	  if (access_denied == 1)
	  	  {
	  		  Send_AT_Command(SEND_MASS);
	  		  HAL_Delay(100);
	  		  sprintf(buffer , "Alarm! Incorrect password entered. \ninput pass:%s\x1A",EPass_c);
	  		  Send_AT_Command(buffer);
	  		  HAL_Delay(1000);
	  		  memset(EPass_c, 0, sizeof(EPass_c));
	  		  //flag_inc_pass = 0;
	  		  access_denied = 0;
	  	  }
	  if (access_granted == true)
	  	  	  {
	  	  		  Send_AT_Command(SEND_MASS);
	  	  		  HAL_Delay(100);
	  	  		  Send_AT_Command("correct password entered. \x1A");
	  	  		  HAL_Delay(1000);
	  	  		  HAL_NVIC_DisableIRQ(EXTI0_IRQn);
	  	  		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
	  	  			    while(Counter <= WAIT_TIME)
	  	  			     {
	  	  			          Counter ++;
	  	  			          HAL_Delay(1000);
	  	  			     }
	  	  			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
	  	  		Counter = 0 ;
	  	  		  flag_c_pass = 0;
	  	  		 access_granted = 0;
	  	  		HAL_NVIC_EnableIRQ(EXTI0_IRQn);
	  	  	  }

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB3 PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : DATA_ENABLE_Pin */
  GPIO_InitStruct.Pin = DATA_ENABLE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(DATA_ENABLE_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_4)
    {
    //	HAL_NVIC_DisableIRQ(EXTI4_IRQn);
        uint8_t b0 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
        uint8_t b1 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);
        uint8_t b2 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6);
        uint8_t b3 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3);
        keypad_code = (b3 << 0) | (b2 << 1) | (b1 << 2) | (b0 << 3);
        pass_c = matrix[keypad_code];
        Keypad();
        //HAL_Delay(30);
       // HAL_NVIC_EnableIRQ(EXTI4_IRQn);

    }


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
