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

#include <stdarg.h>
#include <string.h>
#include <stdint.h>
 //user defined
#include "main.h"
#include "stm32f4xx_hal.h"
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
CRC_HandleTypeDef hcrc;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_CRC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

	char text2[] = "The bootloader is currently running\r\n";
	char textcrc_success[] = "Crc value is correct. Good integrity of data!\n\r";
	char textcrc_fail[] = "Crc value is not correct. Error in sending data\n\r";

	//array for received data
	uint8_t data_received[200];
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
  MX_USART2_UART_Init();
  MX_CRC_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
		/*HAL_UART_Transmit(&huart2,(uint8_t*)text2, sizeof(text2), HAL_MAX_DELAY);
    uint32_t tick= HAL_GetTick(); 
		while(HAL_GetTick()<= (tick+400)); // forsing delay*/
		
	if ( HAL_GPIO_ReadPin(B1_GPIO_Port,B1_Pin) == GPIO_PIN_RESET )
  {
	  //Button pressed starting BL mode
	  start_bl_mode();

  }
  else
  { 
		//Button not pressed starting user_app mode
		start_app_mode();

  }
		
		
		/* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}


//implementing two key functions

void  start_bl_mode(void)
{
    uint8_t bytesleft=0;

	while(1)
	{
		//first byte is the size of a package; we know how many bytes till the end
		memset(data_received,0,200); // clearing
		
    HAL_UART_Receive(&huart2,data_received,1,HAL_MAX_DELAY); //the name of the array a pointer to start
		bytesleft= data_received[0];
		HAL_UART_Receive(&huart2,&data_received[1],bytesleft,HAL_MAX_DELAY); //reaading the rest
		switch(data_received[1]) //second byte is a command code
		{
								case 0x61:
                give_all_codes(data_received); //ALL INSTRUCTIONS
                break;
								case 0x62:
							  jump_to_given_adress(data_received); //JUMP TO LOCATION
                break;
							  case 0x63:
							  download_code_to_given_adress(data_received); //WRITE CODE
                break;							
								case 0x64:
                clear_flash_memory(data_received); //ERASE FLASH
                break;
								case 0x65:
                look_into_rdp(data_received); //PROTECTION  ON READING
                break;
								case 0x66:
                activate_protection(data_received); //TURN ON PROTECTION
                break;
							  case 0x67:
                deactivate_protection(data_received); //TURN ON PROTECTION
                break;
								
								default:
                 //not proper code
                break;

		}

	}}

void printmsg(char *format,...)
 {
#ifdef BL_DEBUG_MSG_EN
	char str[80];

	/*Extract the the argument list using VA apis */
	va_list args;
	va_start(args, format);
	vsprintf(str, format,args);
	HAL_UART_Transmit(D_UART,(uint8_t *)str, strlen(str),HAL_MAX_DELAY);
	va_end(args);
#endif
 }

/*code to jump to user application
 *Here we are assuming FLASH_SECTOR2_BASE_ADDRESS
 *is where the user application is stored
 */
void start_app_mode(void)
{

   //holding reset handler for user_app (to take care of IRQ handlers)
    void (*user_app_handlers)(void);


    // filling the main stack pointer; very first adress
	  //Read the content the content that is 32 bit value
		//using API from CIMSIS
    __set_MSP(*(volatile uint32_t *)0x08008000U);

	
		//the next adress is holding the value of the reset handler
    uint32_t user_app_rh_adress = *(volatile uint32_t *) (0x08008000U + 4);

    user_app_handlers = (void*) user_app_rh_adress;

    //going to the user app
    user_app_handlers();

}


//functions for crc and message from a mcu to PC about integrity of the data 
//CRC
uint8_t crc_check (uint8_t *data_rec, uint32_t bytes, uint32_t crc_correct) //pointer to data, how many bytes, crc from host
{
    uint32_t calculated_CRC=0xff; //empty
		uint32_t i=0; 
    for (i=0; i < bytes ; i++)
	{
        uint32_t next_sequence = data_rec[i];
        calculated_CRC = HAL_CRC_Accumulate(&hcrc, &next_sequence, 1); //API from st driver code
	}

	 /* Reset CRC Calculation Unit */
  __HAL_CRC_DR_RESET(&hcrc);

	if( calculated_CRC == crc_correct )
	{
		return 0;
	}

	return 1;
}


/*This function sends ACK if CRC matches along with "len to follow"*/
void all_good_continue(uint8_t host_code, uint8_t bytes)
{
	 //here we send 2 byte.. first byte is ack and the second byte is len value
	uint8_t message_2_host[2];
	message_2_host[0] = SUCCESS;
	message_2_host[1] = bytes;
	HAL_UART_Transmit(&huart2,message_2_host,2,HAL_MAX_DELAY);

}

/*This function sends NACK */
void error_occured(void)
{
	uint8_t message_2_host = FAILURE;
	HAL_UART_Transmit(&huart2,&message_2_host,1,HAL_MAX_DELAY);
}


uint8_t correct_address(uint32_t jump_address)
{
	
	if ( jump_address >= SRAM1_BASE && jump_address <= SRAM1_END)
	{
		return CORRECT;
	}
	else if ( jump_address >= SRAM2_BASE && jump_address <= SRAM2_END)
	{
		return CORRECT;
	}
	else if ( jump_address >= FLASH_BASE && jump_address <= FLASH_END)
	{
		return CORRECT;
	}
	else if ( jump_address >= BKPSRAM_BASE && jump_address <= BKPSRAM_END)
	{
		return CORRECT;
	}
	else
		return INCORRECT;
}



//***********************
//function in bl mode



void jump_to_given_adress(uint8_t *data){


		uint32_t jump_address=0;
    uint8_t valid = CORRECT;
    //uint8_t invalid = INCORRECT;

   
	uint32_t all_bytes = 1 + data_received[0];

	uint32_t correct_crc = *((uint32_t * ) (data_received+all_bytes - 4));

	if (! crc_check(&data_received[0],all_bytes-4,correct_crc))
	{
        all_good_continue(data[0],1);

        jump_address = *((uint32_t *)&data[2]); //get the adress

        if( correct_address(jump_address) == CORRECT ) //can BL jump to it? USER FLASH
        {
            HAL_UART_Transmit(&huart2, &valid, 1, HAL_MAX_DELAY);


            jump_address+=1;

            void (*lets_jump)(void) = (void *)jump_address;

            lets_jump();

		}else
		{ 
           
            HAL_UART_Transmit(&huart2, &valid, 1, HAL_MAX_DELAY);
		}

	}else
	{
        error_occured();
	}






}


void download_code_to_given_adress(uint8_t *data){

	uint8_t status = 0x00;


	uint8_t payload_len = data[6];

	uint32_t WRITE_address = *((uint32_t *) ( &data[2]) );



    //Total length of the command packet
	uint32_t all_bytes = data[0]+1 ;

	//extract the CRC32 sent by the Host
	uint32_t correct_crc = *((uint32_t * ) (data_received+all_bytes - 4) ) ;


	if (! crc_check(&data_received[0],all_bytes-4,correct_crc))
	{
        all_good_continue(data_received[0],1);


		if( correct_address(WRITE_address) == CORRECT )
		{
          
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
            status = write(&data[7],WRITE_address, payload_len);
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
			
						HAL_UART_Transmit(&huart2, &status, 1, HAL_MAX_DELAY);

		}else
		{
            status = INCORRECT;
						HAL_UART_Transmit(&huart2, &status, 1, HAL_MAX_DELAY);
		}

	}else
	{
        error_occured();
	}


}

//help function for mem write 
uint8_t write(uint8_t *data, uint32_t address, uint32_t len)
{
    uint8_t status=HAL_OK;

    //We have to unlock flash module to get control of registers
    HAL_FLASH_Unlock();

    for(uint32_t i = 0 ; i <len ; i++)
    {
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,address+i,data[i] );
    }

    HAL_FLASH_Lock();

    return status;
}


void clear_flash_memory(uint8_t *data){


		uint8_t status = 0x00;

	uint32_t all_bytes = 1 + data_received[0];


	uint32_t correct_crc = *((uint32_t * ) (data_received+all_bytes - 4) ) ;

	if (! crc_check(&data_received[0],all_bytes-4,correct_crc))
	{
        all_good_continue(data[0],1);
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,1);  
        status = erase(data[2] , data[3]);//pass sector number and how many sectors should be erased
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,0);
				HAL_UART_Transmit(&huart2, &status, 1, HAL_MAX_DELAY);

	}else
	{
        error_occured();
	}

}

//help function for erasing 

uint8_t erase(uint8_t number , uint8_t how_many)
{
   
	//Code needs to modified if your MCU supports more flash sectors
	FLASH_EraseInitTypeDef flashErase;
	uint32_t Error;
	HAL_StatusTypeDef st;


	if( how_many > 8 )
		return INVALID_SECTOR;

	if( (number > 7) && (number != 0xff )){
		return INVALID_SECTOR;
	}
	else
	{
		if(number == (uint8_t) 0xff)
		{
			flashErase.TypeErase = FLASH_TYPEERASE_MASSERASE;
		}else
		{
		    
			uint8_t left = 8 - number;
            if( how_many > left)
            {
            	how_many = left;
            }
			flashErase.TypeErase = FLASH_TYPEERASE_SECTORS;
			flashErase.Sector = number; 
			flashErase.NbSectors = how_many;
		}
		flashErase.Banks = FLASH_BANK_1;

		HAL_FLASH_Unlock();
		flashErase.VoltageRange = FLASH_VOLTAGE_RANGE_3;  
		st = (uint8_t) HAL_FLASHEx_Erase(&flashErase, &Error);
		HAL_FLASH_Lock();

		return st;
	}

}

	
	
	
	
	
	
	
void look_into_rdp( uint8_t *data_received){}


void activate_protection(uint8_t *data_received){}
void deactivate_protection(uint8_t *data_received){}
void read_from_memory(uint8_t *data_received){} 








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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

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
  huart2.Init.BaudRate = 115200;
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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
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
