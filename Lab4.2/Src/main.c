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
#include <stdio.h> // For printf

int __io_putchar(int ch);  // Function declaration for __io_putchar
int __io_getchar(void);    // Function declaration for __io_getchar

//  pins and clock for the LED
#define LED2_PIN                         GPIO_PIN_14
#define LED2_GPIO_PORT                   GPIOB
#define LED2_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()

// pins and clocks for USART
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

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
void LED2_Init(void);
void BSP_COM_Init(UART_HandleTypeDef *);
void LED2_Toggle(void);
void *sbrk(int incr);

int main(void)
{
    /* STM32L4xx HAL library initialization */
    HAL_Init();
    SystemClock_Config();
    LED2_Init();
    BSP_COM_Init(&hDiscoUart);

    char buffer[10]; // Buffer to store input characters
    int index = 0;
    char ch;

    /* Prompt the user for input */
    printf("Please enter a number: ");
    fflush(stdout); // Ensure the prompt message is displayed immediately

    /* Read input character by character */
    while (1)
    {
        ch = __io_getchar(); // Get a character from the user
        if (ch == '\n' || ch == '\r') // Stop reading when Enter is pressed
        {
            buffer[index] = '\0'; // Null-terminate the string
            break;
        }
        else if (index < sizeof(buffer) - 1) // Avoid buffer overflow
        {
            buffer[index++] = ch; // Store the character
            __io_putchar(ch);     // Echo back the character
        }
    }

    /* Print back the received number */
    printf("\nYou entered: %s\n", buffer);
    fflush(stdout); // Ensure the message is displayed immediately

    /* Main loop */
    while (1)
    {
        LED2_Toggle(); // Toggle the LED for visual confirmation
        HAL_Delay(500); // Delay for 500ms
    }
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

/* System Clock Configuration */
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 40;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV4;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
        while (1);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
        while (1);
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
