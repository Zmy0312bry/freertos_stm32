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
#include "../../Utils/Inc/ps2mouse.h"
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

// PS2鼠标任务句柄
osThreadId_t mouseTaskHandle;
const osThreadAttr_t mouse_attributes = {
    .name = "mouseTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,  // 修改为最高优先级
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

// 电机占空比全�?????变量
volatile uint8_t dutyCycle = 0;
volatile uint8_t displayValue_x = 0;
volatile uint8_t displayValue_y = 0;

// mouse data global variable
volatile PS2MouseData_t mouseData;

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
void MouseTask(void *argument);

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
  mouseTaskHandle = osThreadNew(MouseTask, NULL, &mouse_attributes);
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

    // 设置电机占空�?????
    Motor_SetDutyCycle(dutyCycle);

    // 等待3s
    osDelay(3000);
  }
}

void DisplayTask(void *argument)
{
  // 缓冲区用于字符串格式�??
  char str[16];
  
  // 确保OLED已经初始�??
  // 清屏
  ssd1306_Fill(Black);
  ssd1306_UpdateScreen();
  
  // 显示标题
  ssd1306_SetCursor(0, 0);
  ssd1306_WriteString("Mouse Data:", Font_7x10, White);
  
  for(;;)
  {
    // 清除显示区域（保留标题）
    // ssd1306_FillRectangle(0, 16, 128, 64, Black);
    
    // 格式化鼠标X�??
    snprintf(str, sizeof(str), "X: %d", displayValue_x);
    ssd1306_SetCursor(10, 20);
    ssd1306_WriteString(str, Font_7x10, White);
    
    // 格式化鼠标Y�??
    snprintf(str, sizeof(str), "Y: %d", displayValue_y);
    ssd1306_SetCursor(10, 30);
    ssd1306_WriteString(str, Font_7x10, White);
    
    // 格式化鼠标按键状�??
    snprintf(str, sizeof(str), "BTN:%c%c%c", 
             mouseData.leftBtn ? 'L' : '-',
             mouseData.middleBtn ? 'M' : '-',
             mouseData.rightBtn ? 'R' : '-');
    ssd1306_SetCursor(10, 40);
    ssd1306_WriteString(str, Font_7x10, White);
    
    // 格式化电机占空比
    snprintf(str, sizeof(str), "PWM: %d%%", dutyCycle);
    ssd1306_SetCursor(10, 50);
    ssd1306_WriteString(str, Font_7x10, White);
    
    // 更新屏幕
    ssd1306_UpdateScreen();
    osDelay(200);
  }
}

void MouseTask(void *argument)
{
  
  // 设置鼠标分辨率
  PS2Mouse_SetResolution(2);  // 4 counts/mm
  
  // 设置采样率
  PS2Mouse_SetSampleRate(100);  // 100 samples/sec
  
  // 为xTaskDelayUntil添加静态变量记录上次唤醒时间
  TickType_t xLastWakeTime;
  const TickType_t xFrequency = pdMS_TO_TICKS(100); // 每10ms采样一次，而非1tick
  
  // 初始化上次唤醒时间
  xLastWakeTime = xTaskGetTickCount();
  
  for(;;)
  {
    // 读取鼠标数据
    PS2Mouse_ReadData((PS2MouseData_t*)&mouseData);
    
    // 处理鼠标数据，例如通过鼠标控制速度
    if(mouseData.leftBtn) {
      // 如果左键按下，提高占空比
      dutyCycle = (dutyCycle + 10) % 120;
      if(dutyCycle > 100) {
        dutyCycle = 100;
      }
    }
    
    displayValue_x = mouseData.x;
    displayValue_y = mouseData.y;
    
    if(mouseData.rightBtn) {
      // 如果右键按下，降低占空比
      if(dutyCycle >= 10) {
        dutyCycle -= 10;
      } else {
        dutyCycle = 0;
      }
    }
    
    // 使用xTaskDelayUntil确保周期性执行，但周期不要太短
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

/* USER CODE END Application */

