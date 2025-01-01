/**
  ******************************************************************************
  * @file    BSP/Src/main.c 
  * @author  MCD Application Team
  * @brief   Main program body with newlib support
  ******************************************************************************
  * @brief  Main program
  * @param  None
  * @retval None
  */
  
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // For srand() and rand()

int __io_putchar(int ch);  // Function declaration for __io_putchar
int __io_getchar(void);    // Function declaration for __io_getchar

// GPIO pin definitions for LED2
#define LED2_PIN                         GPIO_PIN_14
#define LED2_GPIO_PORT                   GPIOB
#define LED2_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()

// GPIO pin definitions for USART
#define DISCOVERY_COM1                   USART1
#define DISCOVERY_COM1_CLK_ENABLE()      __HAL_RCC_USART1_CLK_ENABLE()
#define DISCOVERY_COM1_TX_PIN            GPIO_PIN_6
#define DISCOVERY_COM1_TX_GPIO_PORT      GPIOB
#define DISCOVERY_COM1_TX_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()
#define DISCOVERY_COM1_TX_AF             GPIO_AF7_USART1
#define DISCOVERY_COM1_RX_PIN            GPIO_PIN_7
#define DISCOVERY_COM1_RX_GPIO_PORT      GPIOB
#define DISCOVERY_COM1_RX_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()
#define DISCOVERY_COM1_RX_AF             GPIO_AF7_USART1

UART_HandleTypeDef hDiscoUart;

// Global variables for memory sections
int global_var = 42;  // Example of an initialized global variable
int uninit_var;       // Example of an uninitialized global variable
extern char _end;     // End of the data section (start of heap)
extern char _sdata;   // Start of initialized data
extern char _edata;   // End of initialized data
extern char *heap_end;

// Function prototypes
static void SystemClock_Config(void);
void LED2_Init(void);
void BSP_COM_Init(UART_HandleTypeDef *);
void LED2_Toggle(void);
void print_memory_info(void);

int main(void)
{
    /* STM32L4xx HAL library initialization */
    HAL_Init();
    SystemClock_Config();
    LED2_Init();
    BSP_COM_Init(&hDiscoUart);

    srand(HAL_GetTick()); // Seed random number generator with system tick

    int correct_answers = 0;
    int total_questions = 5; // Number of questions to ask
    int user_answer, num1, num2, correct_answer;
    char buffer[10]; // Buffer to handle user input as a string
    int index; // Index for buffer

    printf("Welcome to the Math Quiz!\n\n");

	for (int i = 0; i < total_questions; i++)
	{
	    // Generate random numbers for the question
 	   num1 = rand() % 10 + 1; // Random number between 1 and 10
 	   num2 = rand() % 10 + 1; // Random number between 1 and 10
 	   correct_answer = num1 + num2;

 	   // Repeat the question until valid input is given
  	  while (1)
 	   {
   	     // Ask the question
   	     printf("\nQuestion %d: What is %d + %d?\n", i + 1, num1, num2);
   	     printf("Your answer: ");
   	     fflush(stdout);

        	// Reset buffer and index
        	memset(buffer, 0, sizeof(buffer));
        	index = 0;

        	// Input handling loop
        	while (1)
        	{
        	    char ch = __io_getchar(); // Read a character
        	    if (ch == '\r' || ch == '\n') // Enter key pressed
        	    {
        	        buffer[index] = '\0'; // Null-terminate the string
        	        break; // Exit the input loop
        	    }
        	    else if (index < sizeof(buffer) - 1) // Ensure no overflow
        	    {
        	        buffer[index++] = ch; // Add character to buffer
        	        __io_putchar(ch);     // Echo back the character
	            }	
        	}	

        	// Convert the input buffer to an integer
        	if (sscanf(buffer, "%d", &user_answer) == 1)
        	{
        	    // Check the answer
        	    if (user_answer == correct_answer)
        	    {
        	        printf("\nCorrect!\n");
        	        correct_answers++; // Increment correct_answers
        	        break; // Exit the repeat loop for valid input
        	    }
        	    else
        	    {
        	        printf("\nWrong! The correct answer is %d.\n", correct_answer);
        	        break; // Exit the repeat loop for valid input
        	    }
        	}
        	else
        	{
        	    printf("\nInvalid input. Please try again.\n");
        	}
  	  }	

	    // Print memory info after each question
	    printf("\n");
 	    print_memory_info();
 	    printf("\n");
	}

    // Display results
    printf("Quiz complete!\n");
    printf("You got %d out of %d questions correct.\n", correct_answers, total_questions);

    /* Main loop */
    while (1)
    {
        LED2_Toggle(); // Toggle the LED for visual confirmation
        HAL_Delay(500); // Delay for 500ms
    }
}

/* System Clock Configuration */
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* Configure the main internal regulator output voltage */
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* Initializes the CPU, AHB, and APB busses clocks */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;  // 4 MHz
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    RCC_OscInitStruct.PLL.PLLM = 1;    // PLL prescaler
    RCC_OscInitStruct.PLL.PLLN = 40;   // PLL multiplier
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        /* Initialization Error */
        while (1);
    }

    /* Initializes the CPU, AHB, and APB clocks */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
    {
        /* Initialization Error */
        while (1);
    }
}

void LED2_Init(void)
{
    GPIO_InitTypeDef gpio_init_structure;

    LED2_GPIO_CLK_ENABLE();
    gpio_init_structure.Pin = LED2_PIN;
    gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(LED2_GPIO_PORT, &gpio_init_structure);
}

void BSP_COM_Init(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef gpio_init_structure;

    DISCOVERY_COM1_TX_GPIO_CLK_ENABLE();
    DISCOVERY_COM1_RX_GPIO_CLK_ENABLE();
    DISCOVERY_COM1_CLK_ENABLE();

    gpio_init_structure.Pin = DISCOVERY_COM1_TX_PIN;
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_structure.Alternate = DISCOVERY_COM1_TX_AF;
    HAL_GPIO_Init(DISCOVERY_COM1_TX_GPIO_PORT, &gpio_init_structure);

    gpio_init_structure.Pin = DISCOVERY_COM1_RX_PIN;
    gpio_init_structure.Alternate = DISCOVERY_COM1_RX_AF;
    HAL_GPIO_Init(DISCOVERY_COM1_RX_GPIO_PORT, &gpio_init_structure);

    huart->Instance = DISCOVERY_COM1;
    huart->Init.BaudRate = 38400;
    huart->Init.WordLength = UART_WORDLENGTH_8B;
    huart->Init.StopBits = UART_STOPBITS_1;
    huart->Init.Parity = UART_PARITY_NONE;
    huart->Init.Mode = UART_MODE_TX_RX;
    huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart->Init.OverSampling = UART_OVERSAMPLING_16;

    HAL_UART_Init(huart);
}

void LED2_Toggle(void)
{
    HAL_GPIO_TogglePin(LED2_GPIO_PORT, LED2_PIN);
}

int __io_putchar(int ch)
{
    HAL_UART_Transmit(&hDiscoUart, (uint8_t *)&ch, 1, 30000);
    return ch;
}

int __io_getchar(void)
{
    uint8_t ch = 0;
    /* Receive one byte of data from UART with a timeout of 30 seconds */
    while (HAL_UART_Receive(&hDiscoUart, &ch, 1, 30000) != HAL_OK)
    {
        // Stay here until a valid input is received
    }
    return ch;
}

// Debug function to print memory information
void print_memory_info(void)
{
    register char *sp asm("sp"); // Get the stack pointer

    printf("\nMemory Info:\n");
    printf("Initialized Data: 0x%08lx - 0x%08lx\n", (unsigned long)&_sdata, (unsigned long)&_edata);
    printf("Heap End: 0x%08lx\n", (unsigned long)heap_end);
    printf("Stack Pointer: 0x%08lx\n", (unsigned long)sp);
    fflush(stdout);
}

