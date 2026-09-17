/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "app_ipc.h"

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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for A_ControlTask */
osThreadId_t A_ControlTaskHandle;
const osThreadAttr_t A_ControlTask_attributes = {
  .name = "A_ControlTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for A_ComRxTask */
osThreadId_t A_ComRxTaskHandle;
const osThreadAttr_t A_ComRxTask_attributes = {
  .name = "A_ComRxTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* Definitions for A_Cyclic100msTask */
osThreadId_t A_Cyclic100msTaskHandle;
const osThreadAttr_t A_Cyclic100msTask_attributes = {
  .name = "A_Cyclic100msTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for qA_CanRx */
osMessageQueueId_t qA_CanRxHandle;
const osMessageQueueAttr_t qA_CanRx_attributes = {
  .name = "qA_CanRx"
};
/* Definitions for mbA_CommandState */
osMessageQueueId_t mbA_CommandStateHandle;
const osMessageQueueAttr_t mbA_CommandState_attributes = {
  .name = "mbA_CommandState"
};
/* Definitions for mbA_StatusSnapshot */
osMessageQueueId_t mbA_StatusSnapshotHandle;
const osMessageQueueAttr_t mbA_StatusSnapshot_attributes = {
  .name = "mbA_StatusSnapshot"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartAControlTask(void *argument);
void StartAComRxTask(void *argument);
void StartACyclic100msTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);
void vApplicationMallocFailedHook(void);

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
}
/* USER CODE END 4 */

/* USER CODE BEGIN 5 */
void vApplicationMallocFailedHook(void)
{
   /* vApplicationMallocFailedHook() will only be called if
   configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. It is a hook
   function that will get called if a call to pvPortMalloc() fails.
   pvPortMalloc() is called internally by the kernel whenever a task, queue,
   timer or semaphore is created. It is also called by various parts of the
   demo application. If heap_1.c or heap_2.c are used, then the size of the
   heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
   FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
   to query the size of free heap space that remains (although it does not
   provide information on how the remaining heap might be fragmented). */
}
/* USER CODE END 5 */

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

  /* Create the queue(s) */
  /* creation of qA_CanRx */
  qA_CanRxHandle = osMessageQueueNew (4, sizeof(AppCanRxFrame_t), &qA_CanRx_attributes);

  /* creation of mbA_CommandState */
  mbA_CommandStateHandle = osMessageQueueNew (1, sizeof(ACommandState_t), &mbA_CommandState_attributes);

  /* creation of mbA_StatusSnapshot */
  mbA_StatusSnapshotHandle = osMessageQueueNew (1, sizeof(AStatusSnapshot_t), &mbA_StatusSnapshot_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of A_ControlTask */
  A_ControlTaskHandle = osThreadNew(StartAControlTask, NULL, &A_ControlTask_attributes);

  /* creation of A_ComRxTask */
  A_ComRxTaskHandle = osThreadNew(StartAComRxTask, NULL, &A_ComRxTask_attributes);

  /* creation of A_Cyclic100msTask */
  A_Cyclic100msTaskHandle = osThreadNew(StartACyclic100msTask, NULL, &A_Cyclic100msTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartAControlTask */
/**
  * @brief  Function implementing the A_ControlTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartAControlTask */
void StartAControlTask(void *argument)
{
  /* USER CODE BEGIN StartAControlTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartAControlTask */
}

/* USER CODE BEGIN Header_StartAComRxTask */
/**
* @brief Function implementing the A_ComRxTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartAComRxTask */
void StartAComRxTask(void *argument)
{
  /* USER CODE BEGIN StartAComRxTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartAComRxTask */
}

/* USER CODE BEGIN Header_StartACyclic100msTask */
/**
* @brief Function implementing the A_Cyclic100msTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartACyclic100msTask */
void StartACyclic100msTask(void *argument)
{
  /* USER CODE BEGIN StartACyclic100msTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartACyclic100msTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

