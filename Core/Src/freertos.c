/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../../Utils/Inc/motor.h"
#include "../../Utils/Inc/ssd1306.h"
#include "../../Utils/Inc/ssd1306_fonts.h"
#include <stdio.h>
#include "cmsis_os2.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

osThreadId_t motorControlTaskHandle;
const osThreadAttr_t motorControl_attributes = {
    .name = "motorControl",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};

// OLED显示任务句柄
osThreadId_t displayTaskHandle;
const osThreadAttr_t display_attributes = {
    .name = "displayTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

// 电机占空比全�???变量
volatile uint8_t dutyCycle = 0;

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

void MotorControlTask(void *argument);
void DisplayTask(void *argument);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */

  displayTaskHandle = osThreadNew(DisplayTask, NULL, &display_attributes);
  motorControlTaskHandle = osThreadNew(MotorControlTask, NULL, &motorControl_attributes);

  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for (;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void MotorControlTask(void *argument)
{
  // 电机控制初始
  Motor_Init();

  for (;;)
  {
    // 增加占空比，超过100%后归0
    dutyCycle = (dutyCycle + 20) % 120;
    if (dutyCycle > 100)
    {
      dutyCycle = 0;
    }

    // 设置电机占空�???
    Motor_SetDutyCycle(dutyCycle);

    // 等待3s
    osDelay(3000);
  }
}

void DisplayTask(void *argument)
{
  // 缓冲区用于字符串格式�???
  char str[16];
  
  // 确保OLED已经初始�???
  // 清屏
  ssd1306_Fill(Black);
  ssd1306_UpdateScreen();
  
  // 显示标题
  ssd1306_SetCursor(0, 0);
  ssd1306_WriteString("Show Variable", Font_7x10, White);
  
  for(;;)
  {
    // 清除显示区域（保留标题）
    ssd1306_FillRectangle(0, 16, 128, 64, Black);
    
    // 格式化变量�?�到字符�???
    snprintf(str, sizeof(str), "Value: %d", dutyCycle);
    
    // 设置光标并显�???
    ssd1306_SetCursor(10, 25);
    ssd1306_WriteString(str, Font_11x18, White);
    
    // 更新屏幕
    ssd1306_UpdateScreen();
    
    
    // �???500ms更新�???�???
    osDelay(500);
  }
}
/* USER CODE END Application */

