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
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_cdc_if.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define labMaxLn 101
#define labMaxCol 101
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

I2S_HandleTypeDef hi2s3;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
uint8_t rxData;
uint8_t labirynthMatrix[labMaxLn][labMaxCol] = {0};
uint8_t currX = 50, currY = 50;
uint8_t currHeading = 0; //0 = north, 1 = east, 2 = south, 3 = west

#define TRIG_PIN_1 GPIO_PIN_8
#define TRIG_PORT_1 GPIOE
#define ECHO_PIN_1 GPIO_PIN_7
#define ECHO_PORT_1 GPIOE
#define TRIG_PIN_2 GPIO_PIN_10
#define TRIG_PORT_2 GPIOE
#define ECHO_PIN_2 GPIO_PIN_9
#define ECHO_PORT_2 GPIOE
#define TRIG_PIN_3 GPIO_PIN_12
#define TRIG_PORT_3 GPIOE
#define ECHO_PIN_3 GPIO_PIN_11
#define ECHO_PORT_3 GPIOE

uint32_t giveDistSensorFront(){
	uint32_t pMillis;
	uint32_t Value1_Sensor1 = 0;
	uint32_t Value2_Sensor1 = 0;
	uint16_t Distance_Sensor1  = 0; // cm

	HAL_GPIO_WritePin(TRIG_PORT_1, TRIG_PIN_1, GPIO_PIN_SET);  // pull the TRIG_1 pin HIGH
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	while (__HAL_TIM_GET_COUNTER (&htim1) < 40);  // wait for 40 us
	HAL_GPIO_WritePin(TRIG_PORT_1, TRIG_PIN_1, GPIO_PIN_RESET);  // pull the TRIG_1 pin low
	pMillis = HAL_GetTick(); // used this to avoid infinite while loop  (for timeout)
	// wait for the echo pin to go high
	while (!(HAL_GPIO_ReadPin (ECHO_PORT_1, ECHO_PIN_1)) && pMillis + 100 >  HAL_GetTick());
	Value1_Sensor1 = __HAL_TIM_GET_COUNTER (&htim1);

	pMillis = HAL_GetTick(); // used this to avoid infinite while loop (for timeout)
	// wait for the echo pin to go low
	while ((HAL_GPIO_ReadPin (ECHO_PORT_1, ECHO_PIN_1)) && pMillis + 500 > HAL_GetTick());
	Value2_Sensor1 = __HAL_TIM_GET_COUNTER (&htim1);
	Distance_Sensor1 = (Value2_Sensor1 - Value1_Sensor1) * 0.034/2;

	return Distance_Sensor1;
}
uint32_t giveDistSensorLeft(){
	uint32_t pMillis;
	uint32_t Value1_Sensor2 = 0;
	uint32_t Value2_Sensor2 = 0;
	uint16_t Distance_Sensor2  = 0;  // cm

	HAL_GPIO_WritePin(TRIG_PORT_2, TRIG_PIN_2, GPIO_PIN_SET);  // pull the TRIG_1 pin HIGH
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	while (__HAL_TIM_GET_COUNTER (&htim1) < 40);  // wait for 40 us
	HAL_GPIO_WritePin(TRIG_PORT_2, TRIG_PIN_2, GPIO_PIN_RESET);  // pull the TRIG_1 pin low
	pMillis = HAL_GetTick(); // used this to avoid infinite while loop  (for timeout)
	// wait for the echo pin to go high
	while (!(HAL_GPIO_ReadPin (ECHO_PORT_2, ECHO_PIN_2)) && pMillis + 50 >  HAL_GetTick());
	Value1_Sensor2 = __HAL_TIM_GET_COUNTER (&htim1);
	pMillis = HAL_GetTick(); // used this to avoid infinite while loop (for timeout)
	// wait for the echo pin to go low
	while ((HAL_GPIO_ReadPin (ECHO_PORT_2, ECHO_PIN_2)) && pMillis + 200 > HAL_GetTick());
	Value2_Sensor2 = __HAL_TIM_GET_COUNTER (&htim1);
	Distance_Sensor2 = (Value2_Sensor2 - Value1_Sensor2) * 0.034/2;

	return Distance_Sensor2;
}
uint32_t giveDistSensorRight(){
	uint32_t pMillis;
	uint32_t Value1_Sensor3 = 0;
	uint32_t Value2_Sensor3 = 0;
	uint16_t Distance_Sensor3  = 0;  // cm
	HAL_GPIO_WritePin(TRIG_PORT_3, TRIG_PIN_3, GPIO_PIN_SET);  // pull the TRIG_1 pin HIGH
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	while (__HAL_TIM_GET_COUNTER (&htim1) < 40);  // wait for 40 us
	HAL_GPIO_WritePin(TRIG_PORT_3, TRIG_PIN_3, GPIO_PIN_RESET);  // pull the TRIG_1 pin low
	pMillis = HAL_GetTick(); // used this to avoid infinite while loop  (for timeout)
	// wait for the echo pin to go high
	while (!(HAL_GPIO_ReadPin (ECHO_PORT_3, ECHO_PIN_3)) && pMillis + 50 >  HAL_GetTick());
	Value1_Sensor3 = __HAL_TIM_GET_COUNTER (&htim1);

	pMillis = HAL_GetTick(); // used this to avoid infinite while loop (for timeout)
	// wait for the echo pin to go low
	while ((HAL_GPIO_ReadPin (ECHO_PORT_3, ECHO_PIN_3)) && pMillis + 200 > HAL_GetTick());
	Value2_Sensor3 = __HAL_TIM_GET_COUNTER (&htim1);
	Distance_Sensor3 = (Value2_Sensor3 - Value1_Sensor3) * 0.034/2;

	return Distance_Sensor3;
}
void printMatrix(){
	//HAL_UART_Transmit(&huart2, (uint8_t *)"Now printing file\r\n", 19, 100);
	uint8_t i, j;
	uint8_t str[labMaxCol + 2];

	for(i = 0; i < labMaxLn; i++){
		//HAL_UART_Transmit(&huart2, (uint8_t *)"here\r\n", 6, 100);
		for(j = 0; j < labMaxCol; j++)
			str[j] = labirynthMatrix[i][j] + '0';
		str[j] = '\n';
		str[j + 1] = '\0';
		//labirynthMatrix[i][labMaxCol - 1] = '\0';
		HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen(str), 100);
//		for(j = 0; j < labMaxCol; j++){
//			HAL_UART_Transmit(&huart2, (uint8_t *)&(labirynthMatrix[i][j]), 1, 100);
//			fputc(labirynthMatrix[i][j], input);
//		}
//		HAL_UART_Transmit(&huart2, (uint8_t *)'\n', 1, 100);
//		fputc('\n', input);
		//HAL_Delay(10);
	}
}
void moveForward(){
	//right motor
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2,GPIO_PIN_RESET);

	//left motor
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,GPIO_PIN_RESET);

	//right motor
	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1, 490);
	//left motor
	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3, 540);

	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);

	HAL_Delay(550);
}
void turnLeft(){
		//right motor
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1,GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2,GPIO_PIN_RESET);

		//left motor
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,GPIO_PIN_SET);

		//right motor
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1, 480);
		//left motor
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3, 570);

		HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);

		HAL_Delay(750);
	}
void turnRight(){
		//right motor
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2,GPIO_PIN_SET);

		//left motor
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8,GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,GPIO_PIN_RESET);

		//right motor
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1, 450);
		//left motor
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3, 570);

		HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
		HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);

		HAL_Delay(400);
	}
void moveStop(){
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,GPIO_PIN_RESET);

	HAL_Delay(100);
}
void moveBack(){
	//right motor
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2,GPIO_PIN_SET);

	//left motor
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9,GPIO_PIN_SET);

	//right motor
	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1, 450);
	//left motor
	__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3, 570);

	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);

	HAL_Delay(300);
}
void labirynthTest(){
	uint32_t distFront, distLeft, distRight;

	distFront = giveDistSensorFront();
	distLeft = giveDistSensorLeft();
	distRight = giveDistSensorRight();

	HAL_Delay(250);

	//regula stangii
	if(distFront < 7){
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
		moveBack();
	}
	else if(distRight > 26){ //daca poate, mereu merge la dreapta
		//culoar liber - merge acolo
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
		turnRight();
		switch(currHeading){
			case 0:
				currHeading = 1;
				break;
			case 1:
				currHeading = 2;
				break;
			case 2:
				currHeading = 3;
				break;
			case 3:
				currHeading = 0;
				break;
			default:
				return;
		}
		HAL_Delay(200);
		moveForward();
		switch(currHeading){
			case 0:
				currX--;
				break;
			case 1:
				currY++;
				break;
			case 2:
				currX++;
				break;
			case 3:
				currY--;
				break;
			default:
				return;
		}
	}
	/*else if(distRight > 10){
		//dead end, marcheaza ca 'parcurs' si mergi mai departe
		turnRight();
		moveStop();
		//HAL_Delay(1000);
	}*/
	else if(distFront > 26){
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET); //red LED
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
		moveForward();
		switch(currHeading){
			case 0:
				currX--;
				break;
			case 1:
				currY++;
				break;
			case 2:
				currX++;
				break;
			case 3:
				currY--;
				break;
			default:
				return;
		}
		//nu poate merge la dreapta
	}
	else if(distLeft > 26){
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
		turnLeft();
		switch(currHeading){
			case 0:
				currHeading = 3;
				break;
			case 1:
				currHeading = 0;
				break;
			case 2:
				currHeading = 1;
				break;
			case 3:
				currHeading = 2;
				break;
			default:
				return;
		}
		HAL_Delay(200);
		moveForward();
		switch(currHeading){
			case 0:
				currX--;
				break;
			case 1:
				currY++;
				break;
			case 2:
				currX++;
				break;
			case 3:
				currY--;
				break;
			default:
				return;
		}
	}else{
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
		moveBack();
		HAL_Delay(2000);
	}

	labirynthMatrix[currX][currY] = 1;
	moveStop();
	//printMatrix();
	HAL_Delay(50);
}

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2S3_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

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
  MX_I2C1_Init();
  MX_I2S3_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_USB_DEVICE_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  HAL_TIM_Base_Start(&htim1);
  HAL_GPIO_WritePin(TRIG_PORT_1, TRIG_PIN_1, GPIO_PIN_RESET);  // pull the TRIG pin for sensor 1 low
  HAL_GPIO_WritePin(TRIG_PORT_2, TRIG_PIN_2, GPIO_PIN_RESET);  // pull the TRIG pin for sensor 2 low
  HAL_GPIO_WritePin(TRIG_PORT_3, TRIG_PIN_3, GPIO_PIN_RESET);  // pull the TRIG pin for sensor 3 low
  HAL_UART_Receive_IT(&huart2, &rxData, 1); // Enabling interrupt receive for USART2  /* USER CODE END 2 */


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (!rxData)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  //HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
	HAL_Delay(500);

    labirynthTest();

//    if(dist1 <= 5){
//	 HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
//	}
//	else{
//	 HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
//	}
//	if(dist2 <= 5){
//	 HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
//	}
//	else{
//	 HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
//	}
//	if(dist3 <= 5){
//	 HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
//	}
//	else{
//	 HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
//	}
//    turnLeft();
//    HAL_Delay(500);
//    moveStop();
//    HAL_Delay(500);
//    turnRight();
//    HAL_Delay(500);
//    moveForward();
//    moveStop();
//    HAL_Delay(3000);


//    if(dist1 > 5 && dist2 > 5 && dist3 > 5){ //cm
//    	//move robot forward
//    	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
//    	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
//    	moveForward();
//    }
//    else{
//    	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
//    	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
//    	//stop robot
//    	moveStop();
//    }
  }

  HAL_Delay(500);
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
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2S3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2S3_Init(void)
{

  /* USER CODE BEGIN I2S3_Init 0 */

  /* USER CODE END I2S3_Init 0 */

  /* USER CODE BEGIN I2S3_Init 1 */

  /* USER CODE END I2S3_Init 1 */
  hi2s3.Instance = SPI3;
  hi2s3.Init.Mode = I2S_MODE_MASTER_TX;
  hi2s3.Init.Standard = I2S_STANDARD_PHILIPS;
  hi2s3.Init.DataFormat = I2S_DATAFORMAT_16B;
  hi2s3.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
  hi2s3.Init.AudioFreq = I2S_AUDIOFREQ_96K;
  hi2s3.Init.CPOL = I2S_CPOL_LOW;
  hi2s3.Init.ClockSource = I2S_CLOCK_PLL;
  hi2s3.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
  if (HAL_I2S_Init(&hi2s3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2S3_Init 2 */

  /* USER CODE END I2S3_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 83;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 30;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, CS_I2C_SPI_Pin|GPIO_PIN_8|GPIO_PIN_10|GPIO_PIN_12, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(OTG_FS_PowerSwitchOn_GPIO_Port, OTG_FS_PowerSwitchOn_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin
                          |Audio_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pins : CS_I2C_SPI_Pin PE8 PE10 PE12 */
  GPIO_InitStruct.Pin = CS_I2C_SPI_Pin|GPIO_PIN_8|GPIO_PIN_10|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : OTG_FS_PowerSwitchOn_Pin PC1 PC2 PC9 */
  GPIO_InitStruct.Pin = OTG_FS_PowerSwitchOn_Pin|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PDM_OUT_Pin */
  GPIO_InitStruct.Pin = PDM_OUT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(PDM_OUT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BOOT1_Pin */
  GPIO_InitStruct.Pin = BOOT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BOOT1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PE7 PE9 PE11 */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_9|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : CLK_IN_Pin */
  GPIO_InitStruct.Pin = CLK_IN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(CLK_IN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD4_Pin LD3_Pin LD5_Pin LD6_Pin
                           Audio_RST_Pin */
  GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin
                          |Audio_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : OTG_FS_OverCurrent_Pin */
  GPIO_InitStruct.Pin = OTG_FS_OverCurrent_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(OTG_FS_OverCurrent_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : MEMS_INT2_Pin */
  GPIO_InitStruct.Pin = MEMS_INT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MEMS_INT2_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

  if(huart->Instance == USART2)  // Changed to check for USART2
  {
    if(rxData == 78) // Ascii value of 'N' is 78 (N for NO)
    {
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, 0);
        printMatrix();
    }
    else if (rxData == 89) // Ascii value of 'Y' is 89 (Y for YES)
    {
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, 1);
        //STOP PROGRAM EXECUTION
    }
    HAL_UART_Receive_IT(&huart2, &rxData, 1); // Enabling interrupt receive again for USART2
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
