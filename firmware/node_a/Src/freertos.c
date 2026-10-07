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
#include "can.h"
#include "usart.h"

#include <stdio.h>
#include <string.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define H2_CAN_BITRATE_BPS       500000U
#define H2_CAN_TX_STD_ID         0x180U
#define H2_CAN_RX_STD_ID         0x100U
#define H2_CAN_PERIOD_MS         100U
#define H2_CAN_STARTUP_DELAY_MS  10000U
#define H2_CAN_TX_DLC            5U
#define H2_CAN_RX_DLC            2U
#define H2_CAN_RX_COUNTER_INDEX  1U

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

static volatile uint8_t g_h2_can_ready;
static volatile uint8_t g_h2_can_start_result;
static volatile uint32_t g_h2_tx_attempts;
static volatile uint32_t g_h2_tx_queued;
static volatile uint32_t g_h2_tx_failures;
static volatile uint32_t g_h2_rx_irq_count;
static volatile uint32_t g_h2_rx_queue_drops;
static volatile uint32_t g_h2_rx_valid;
static volatile uint32_t g_h2_rx_invalid;
static volatile uint32_t g_h2_rx_sequence_errors;
static volatile uint8_t g_h2_rx_seen;
static volatile uint8_t g_h2_last_rx_counter;

/* USER CODE END Variables */
/* Definitions for A_ControlTask */
osThreadId_t A_ControlTaskHandle;
const osThreadAttr_t A_ControlTask_attributes = {
  .name = "A_ControlTask",
  .stack_size = 512 * 4,
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

static uint8_t H2_CanStart(void);
static void H2_UartWrite(const char *text);
static void H2_LogBoot(uint8_t start_result);
static void H2_LogStatus(void);
static void H2_SendFrame(uint8_t counter);
static uint8_t H2_IsValidPeerFrame(const AppCanRxFrame_t *frame);
static uint8_t H2_GetPeerCounter(const AppCanRxFrame_t *frame);

/* USER CODE END FunctionPrototypes */

void StartAControlTask(void *argument);
void StartAComRxTask(void *argument);
void StartACyclic100msTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);
void vApplicationMallocFailedHook(void);

/* USER CODE BEGIN 4 */
static uint8_t H2_CanStart(void)
{
  CAN_FilterTypeDef filter = {0};

  filter.FilterBank = 0;
  filter.FilterMode = CAN_FILTERMODE_IDMASK;
  filter.FilterScale = CAN_FILTERSCALE_32BIT;
  filter.FilterIdHigh = 0;
  filter.FilterIdLow = 0;
  filter.FilterMaskIdHigh = 0;
  filter.FilterMaskIdLow = 0;
  filter.FilterFIFOAssignment = CAN_RX_FIFO0;
  filter.FilterActivation = ENABLE;
  filter.SlaveStartFilterBank = 14;

  if (HAL_CAN_ConfigFilter(&hcan, &filter) != HAL_OK)
  {
    return 1U;
  }
  if (HAL_CAN_Start(&hcan) != HAL_OK)
  {
    return 2U;
  }
  if (HAL_CAN_ActivateNotification(&hcan,
                                   CAN_IT_RX_FIFO0_MSG_PENDING |
                                   CAN_IT_RX_FIFO0_OVERRUN) != HAL_OK)
  {
    return 3U;
  }

  return 0U;
}

static void H2_UartWrite(const char *text)
{
  size_t length;

  if (text == NULL)
  {
    return;
  }

  length = strlen(text);
  if (length > UINT16_MAX)
  {
    length = UINT16_MAX;
  }
  (void)HAL_UART_Transmit(&huart2, (uint8_t *)text, (uint16_t)length, 100U);
}

static void H2_LogBoot(uint8_t start_result)
{
  char line[192];
  int length;

  if (start_result == 0U)
  {
    length = snprintf(line, sizeof(line),
                      "H2,A,BOOT,profile=formal_baseline,bitrate=%lu,tx_id=0x%03lX,tx_dlc=%u,rx_id=0x%03lX,rx_dlc=%u,period_ms=%lu,start_delay_ms=%lu\r\n",
                      (unsigned long)H2_CAN_BITRATE_BPS,
                      (unsigned long)H2_CAN_TX_STD_ID,
                      (unsigned int)H2_CAN_TX_DLC,
                      (unsigned long)H2_CAN_RX_STD_ID,
                      (unsigned int)H2_CAN_RX_DLC,
                      (unsigned long)H2_CAN_PERIOD_MS,
                      (unsigned long)H2_CAN_STARTUP_DELAY_MS);
  }
  else
  {
    length = snprintf(line, sizeof(line),
                      "H2,A,BOOT_FAIL,start_result=%u,hal_error=0x%08lX\r\n",
                      (unsigned int)start_result,
                      (unsigned long)HAL_CAN_GetError(&hcan));
  }

  if (length > 0)
  {
    line[sizeof(line) - 1U] = '\0';
    H2_UartWrite(line);
  }
}

static void H2_LogStatus(void)
{
  char line[256];
  int length;

  length = snprintf(line, sizeof(line),
                    "H2,A,STAT,tick=%lu,tx_try=%lu,tx_q=%lu,tx_fail=%lu,rx_irq=%lu,rx=%lu,rx_bad=%lu,seq_err=%lu,q_drop=%lu,seen=%u,last=%u,mb_free=%lu,hal_err=0x%08lX,esr=0x%08lX\r\n",
                    (unsigned long)HAL_GetTick(),
                    (unsigned long)g_h2_tx_attempts,
                    (unsigned long)g_h2_tx_queued,
                    (unsigned long)g_h2_tx_failures,
                    (unsigned long)g_h2_rx_irq_count,
                    (unsigned long)g_h2_rx_valid,
                    (unsigned long)g_h2_rx_invalid,
                    (unsigned long)g_h2_rx_sequence_errors,
                    (unsigned long)g_h2_rx_queue_drops,
                    (unsigned int)g_h2_rx_seen,
                    (unsigned int)g_h2_last_rx_counter,
                    (unsigned long)HAL_CAN_GetTxMailboxesFreeLevel(&hcan),
                    (unsigned long)HAL_CAN_GetError(&hcan),
                    (unsigned long)hcan.Instance->ESR);

  if (length > 0)
  {
    line[sizeof(line) - 1U] = '\0';
    H2_UartWrite(line);
  }
}

static void H2_SendFrame(uint8_t counter)
{
  CAN_TxHeaderTypeDef header = {0};
  uint32_t mailbox = 0U;
  uint8_t data[8] = {0};

  header.StdId = H2_CAN_TX_STD_ID;
  header.ExtId = 0U;
  header.IDE = CAN_ID_STD;
  header.RTR = CAN_RTR_DATA;
  header.DLC = H2_CAN_TX_DLC;
  header.TransmitGlobalTime = DISABLE;

  /* 0x180 Actuator_Status: FULL, IDLE_OFF, no fault, RPM invalid. */
  data[0] = 0x00U;
  data[1] = 0x00U;
  data[2] = (uint8_t)(counter & 0x0FU);
  data[3] = 0xFFU;
  data[4] = 0xFFU;

  g_h2_tx_attempts++;
  if (HAL_CAN_AddTxMessage(&hcan, &header, data, &mailbox) == HAL_OK)
  {
    g_h2_tx_queued++;
  }
  else
  {
    g_h2_tx_failures++;
  }
}

static uint8_t H2_IsValidPeerFrame(const AppCanRxFrame_t *frame)
{
  if ((frame == NULL) ||
      (frame->std_id != H2_CAN_RX_STD_ID) ||
      (frame->dlc != H2_CAN_RX_DLC))
  {
    return 0U;
  }

  /* 0x100 Cooling_Command: demand 0..100 and reserved nibble clear. */
  if ((frame->data[0] > 100U) ||
      ((frame->data[1] & 0xF0U) != 0U))
  {
    return 0U;
  }

  return 1U;
}

static uint8_t H2_GetPeerCounter(const AppCanRxFrame_t *frame)
{
  return (uint8_t)(frame->data[H2_CAN_RX_COUNTER_INDEX] & 0x0FU);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *can_handle)
{
  CAN_RxHeaderTypeDef header = {0};
  AppCanRxFrame_t frame = {0};
  uint8_t data[8] = {0};
  uint32_t index;

  if ((can_handle == NULL) || (can_handle->Instance != CAN1))
  {
    return;
  }

  g_h2_rx_irq_count++;
  if (HAL_CAN_GetRxMessage(can_handle, CAN_RX_FIFO0, &header, data) != HAL_OK)
  {
    g_h2_rx_queue_drops++;
    return;
  }

  frame.rx_tick = HAL_GetTick();
  frame.std_id = (uint16_t)header.StdId;
  frame.dlc = (uint8_t)((header.DLC <= 8U) ? header.DLC : 8U);
  for (index = 0U; index < frame.dlc; ++index)
  {
    frame.data[index] = data[index];
  }

  if ((qA_CanRxHandle == NULL) ||
      (osMessageQueuePut(qA_CanRxHandle, &frame, 0U, 0U) != osOK))
  {
    g_h2_rx_queue_drops++;
  }
}

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
  uint32_t half_second_count = 0U;

  g_h2_can_start_result = H2_CanStart();
  if (g_h2_can_start_result == 0U)
  {
    g_h2_can_ready = 1U;
  }
  H2_LogBoot(g_h2_can_start_result);

  if (g_h2_can_start_result != 0U)
  {
    for (;;)
    {
      HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
      osDelay(100U);
    }
  }

  /* Infinite loop */
  for(;;)
  {
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    half_second_count++;
    if ((half_second_count % 2U) == 0U)
    {
      H2_LogStatus();
    }
    osDelay(500U);
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
  AppCanRxFrame_t frame;
  uint8_t peer_counter;

  while (g_h2_can_ready == 0U)
  {
    osDelay(10U);
  }

  /* Infinite loop */
  for(;;)
  {
    if (osMessageQueueGet(qA_CanRxHandle, &frame, NULL, osWaitForever) == osOK)
    {
      if (H2_IsValidPeerFrame(&frame) != 0U)
      {
        peer_counter = H2_GetPeerCounter(&frame);
        if ((g_h2_rx_seen != 0U) &&
            ((uint8_t)((g_h2_last_rx_counter + 1U) & 0x0FU) != peer_counter))
        {
          g_h2_rx_sequence_errors++;
        }
        g_h2_last_rx_counter = peer_counter;
        g_h2_rx_seen = 1U;
        g_h2_rx_valid++;
      }
      else
      {
        g_h2_rx_invalid++;
      }
    }
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
  uint8_t counter = 0U;

  while (g_h2_can_ready == 0U)
  {
    osDelay(10U);
  }
  osDelay(H2_CAN_STARTUP_DELAY_MS);

  /* Infinite loop */
  for(;;)
  {
    H2_SendFrame(counter);
    counter = (uint8_t)((counter + 1U) & 0x0FU);
    osDelay(H2_CAN_PERIOD_MS);
  }
  /* USER CODE END StartACyclic100msTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

