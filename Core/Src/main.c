/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"   // Include UART library
#include "gpio.h"
#include "lcd_i2c.h"
#include <string.h>
#include "keypad.h"
#include <stdbool.h>
#include <stdio.h>
#include "cmsis_os.h" // FreeRTOS library
#include "FreeRTOS.h"  // FreeRTOS library
#include "task.h"      // FreeRTOS library

/* USER CODE BEGIN PD */
extern I2C_HandleTypeDef hi2c1;

#define PASSWORD "5555"
#define MAX_ATTEMPTS 3
#define SERVO_UNLOCK 2500
#define SERVO_LOCK 500
#define LOCKOUT_TIME 120
#define UART_BUFFER_SIZE 100  // Buffer size for UART communication

/* USER CODE END PD */

/* USER CODE BEGIN PV */
char entered_password[5] = "";
uint8_t password_index = 0;
uint8_t attempt_count = 0;
bool keypad_locked = false;
bool lockout_active = false;
char uart_rx_buffer[UART_BUFFER_SIZE];  // Buffer for receiving UART data

/* USER CODE END PV */

void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
void set_servo_angle(TIM_HandleTypeDef *htim, uint16_t channel, uint8_t angle);
void reset_password_input();
void check_password();
void handle_lockout();
void send_uart_command(const char *cmd);
void receive_uart_response();
void send_firebase_alert();  // New function to send alert to Firebase
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
void send_uart_command(const char *cmd) {
    HAL_UART_Transmit(&huart1, (uint8_t *)cmd, strlen(cmd), HAL_MAX_DELAY);
    HAL_Delay(100);  // Small delay for stability
}

void receive_uart_response() {
    memset(uart_rx_buffer, 0, UART_BUFFER_SIZE);
    HAL_UART_Receive(&huart1, (uint8_t *)uart_rx_buffer, UART_BUFFER_SIZE - 1, 1000);
    // Display ESP32 response on LCD
    lcd_clear();
    lcd_send_string("          ESP32 Response:");
    LCD_SetCursor(1, 0);
    lcd_send_string(uart_rx_buffer);
    HAL_Delay(2000);
}

// 🔹 New function to send an alert to Firebase after 3 failed attempts
void send_firebase_alert() {
    lcd_clear();
    lcd_send_string("          Sending Alert...");
    HAL_Delay(2000);

    send_uart_command("FIREBASE_ALERT\n");  // Command to notify Firebase
    HAL_Delay(2000);

    lcd_clear();
    lcd_send_string("          Alert Sent!");
    HAL_Delay(2000);
}

void reset_password_input() {
    memset(entered_password, 0, sizeof(entered_password));
    password_index = 0;
    lcd_clear();
    lcd_send_string("          Password:");
}

void set_servo_angle(TIM_HandleTypeDef *htim, uint16_t channel, uint8_t angle) {
    uint16_t pulse_length = SERVO_LOCK + ((SERVO_UNLOCK - SERVO_LOCK) * angle) / 180;
    __HAL_TIM_SET_COMPARE(htim, channel, pulse_length);
}

void check_password() {
    if (password_index != 4) return;
    entered_password[password_index] = '\0';

    if (strcmp(entered_password, PASSWORD) == 0) {
        lcd_clear();
        lcd_send_string("          Access Granted");
        HAL_Delay(2000);
        lcd_clear();
        lcd_send_string("          Door Unlocked");
        set_servo_angle(&htim3, TIM_CHANNEL_4, 210);
        HAL_Delay(3000);
        attempt_count = 0;
    } else {
        attempt_count++;
        lcd_clear();
        lcd_send_string("          Wrong Password!");
        HAL_Delay(2000);

        if (attempt_count >= MAX_ATTEMPTS) {
            keypad_locked = true;
            lockout_active = true;

            // 🔹 Send "CAPTURE" command to ESP32-CAM after 3rd failed attempt
            lcd_clear();
            lcd_send_string("          CapturingImage...");
            HAL_Delay(500);
            send_uart_command("C");  // Send UART command
            HAL_Delay(2000);
            send_uart_command("C");  // Send UART command
            HAL_Delay(2000);
            send_uart_command("C");  // Send UART command
            HAL_Delay(2000);
            receive_uart_response();  // Optional: Receive response from ESP32-CAM

            send_firebase_alert();
        } else {
            set_servo_angle(&htim3, TIM_CHANNEL_4, 0);
        }
    }
    reset_password_input();
}

void handle_lockout() {
    if (!lockout_active) return;
    lcd_clear();
    lcd_send_string("          Too many tries");
    HAL_Delay(1000);
    lcd_clear();
    lcd_send_string("          Locking for 2min");
    HAL_Delay(2000);

    for (int i = LOCKOUT_TIME; i >= 0; i--) {
        char buffer[120];
        sprintf(buffer, "            Wait: %dsec", i);
        lcd_clear();
        lcd_send_string(buffer);
        HAL_Delay(1000);
    }
    keypad_locked = false;
    lockout_active = false;
    attempt_count = 0;
    lcd_clear();
    lcd_send_string("             Try again!");
    HAL_Delay(1000);
    reset_password_input();
}

int main(void) {
    char key;

    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_TIM3_Init();
    MX_USART1_UART_Init();  // Initialize UART1 for ESP32-CAM communication

    keypad_init();
    lcd_init();
    lcd_clear();
    lcd_send_string("          Smart Lock Ready");
    HAL_Delay(5000);
    reset_password_input();

    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
    set_servo_angle(&htim3, TIM_CHANNEL_4, 0);

    // Send a test command to ESP32-CAM
    send_uart_command("AT+RST\r\n");
    HAL_Delay(1000);
    receive_uart_response();

    while (1) {
        if (keypad_locked) {
            handle_lockout();
        } else {
            key = keypad_get_key();

            if (key != 0) {
                if (key == '*' && password_index == 4) {
                    check_password();
                } else if (key == '0') {
                    reset_password_input();
                } else if (password_index < 4) {
                    entered_password[password_index++] = key;
                    lcd_send_data(key);
                }
            }
        }
    }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
    Error_Handler();
  }
}

/**
  * @brief Error Handler
  */
void Error_Handler(void) {
  __disable_irq();
  while (1) { }
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) {
}
#endif /* USE_FULL_ASSERT */
