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
#include "adc.h"
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
#define H2_CAN_TX_STD_ID         0x100U
#define H2_CAN_RX_STD_ID         0x180U
#define H2_CAN_PERIOD_MS         100U
#define H2_CAN_STARTUP_DELAY_MS  10000U
#define H2_CAN_TX_DLC            2U
#define H2_CAN_RX_DLC            5U
#define H2_CAN_RX_COUNTER_INDEX  2U

#define H3_NTC_SAMPLE_PERIOD_MS  H2_CAN_PERIOD_MS
#define H3_NTC_LOG_DIVIDER       10U
#define H3_ADC_FULL_SCALE        4095U
#define H3_VDDA_CONFIGURED_MV    3300U
#define H3_R_FIXED_OHM           9911U

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

static uint8_t g_h3_adc_cal_result;
static uint8_t g_h3_adc_ready;
static uint8_t g_h3_adc_last_result;
static uint16_t g_h3_adc_latest_raw;
static uint16_t g_h3_adc_window_min = H3_ADC_FULL_SCALE;
static uint16_t g_h3_adc_window_max;
static uint32_t g_h3_adc_samples_total;
static uint32_t g_h3_adc_failures_total;
static uint32_t g_h3_adc_window_samples;

/* USER CODE END Variables */
/* Definitions for B_ComRxTask */
osThreadId_t B_ComRxTaskHandle;
const osThreadAttr_t B_ComRxTask_attributes = {
  .name = "B_ComRxTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for B_Cyclic100msSupervisorTask */
osThreadId_t B_Cyclic100msSupervisorTaskHandle;
const osThreadAttr_t B_Cyclic100msSupervisorTask_attributes = {
  .name = "B_Cyclic100msSupervisorTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for qB_CanRx */
osMessageQueueId_t qB_CanRxHandle;
const osMessageQueueAttr_t qB_CanRx_attributes = {
  .name = "qB_CanRx"
};
/* Definitions for mbB_NodeAStatus */
osMessageQueueId_t mbB_NodeAStatusHandle;
const osMessageQueueAttr_t mbB_NodeAStatus_attributes = {
  .name = "mbB_NodeAStatus"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

static uint8_t H2_CanStart(void);
static void App_UartWrite(const char *text);
static void H2_LogBoot(uint8_t start_result);
static void H2_LogStatus(void);
static void H2_SendFrame(uint8_t counter);
static uint8_t H2_IsValidPeerFrame(const AppCanRxFrame_t *frame);
static uint8_t H2_GetPeerCounter(const AppCanRxFrame_t *frame);
static uint8_t H3_AdcSample(uint16_t *raw);
static void H3_LogBoot(void);
static void H3_LogNtcStatus(void);

/* USER CODE END FunctionPrototypes */

void StartBComRxTask(void *argument);
void StartBCyclic100msSupervisorTask(void *argument);

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

static void App_UartWrite(const char *text)
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
                      "H2,B,BOOT,profile=formal_baseline,bitrate=%lu,tx_id=0x%03lX,tx_dlc=%u,rx_id=0x%03lX,rx_dlc=%u,period_ms=%lu,start_delay_ms=%lu\r\n",
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
                      "H2,B,BOOT_FAIL,start_result=%u,hal_error=0x%08lX\r\n",
                      (unsigned int)start_result,
                      (unsigned long)HAL_CAN_GetError(&hcan));
  }

  if (length > 0)
  {
    line[sizeof(line) - 1U] = '\0';
    App_UartWrite(line);
  }
}

static void H2_LogStatus(void)
{
  char line[256];
  int length;

  length = snprintf(line, sizeof(line),
                    "H2,B,STAT,tick=%lu,tx_try=%lu,tx_q=%lu,tx_fail=%lu,rx_irq=%lu,rx=%lu,rx_bad=%lu,seq_err=%lu,q_drop=%lu,seen=%u,last=%u,mb_free=%lu,hal_err=0x%08lX,esr=0x%08lX\r\n",
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
    App_UartWrite(line);
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

  /* 0x100 Cooling_Command: zero demand and 4-bit alive counter. */
  data[0] = 0x00U;
  data[1] = (uint8_t)(counter & 0x0FU);

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

  /* 0x180 Actuator_Status: validate reserved bits and RPM validity. */
  if (((frame->data[0] & 0xC0U) != 0U) ||
      ((frame->data[0] & 0x03U) == 0x03U) ||
      (frame->data[1] != 0x00U) ||
      ((frame->data[2] & 0xF0U) != 0U))
  {
    return 0U;
  }

  if ((((frame->data[0] & 0x20U) == 0U) &&
       ((frame->data[3] != 0xFFU) || (frame->data[4] != 0xFFU))) ||
      (((frame->data[0] & 0x20U) != 0U) &&
       (frame->data[3] == 0xFFU) && (frame->data[4] == 0xFFU)))
  {
    return 0U;
  }

  return 1U;
}

static uint8_t H2_GetPeerCounter(const AppCanRxFrame_t *frame)
{
  return (uint8_t)(frame->data[H2_CAN_RX_COUNTER_INDEX] & 0x0FU);
}

static uint8_t H3_AdcSample(uint16_t *raw)
{
  uint32_t value;

  if (raw == NULL)
  {
    return 4U;
  }
  if (HAL_ADC_Start(&hadc1) != HAL_OK)
  {
    return 1U;
  }
  if (HAL_ADC_PollForConversion(&hadc1, 5U) != HAL_OK)
  {
    (void)HAL_ADC_Stop(&hadc1);
    return 2U;
  }

  value = HAL_ADC_GetValue(&hadc1);
  if (HAL_ADC_Stop(&hadc1) != HAL_OK)
  {
    return 3U;
  }
  if (value > H3_ADC_FULL_SCALE)
  {
    return 4U;
  }

  *raw = (uint16_t)value;
  return 0U;
}

static void H3_LogBoot(void)
{
  char line[192];
  int length;

  length = snprintf(line, sizeof(line),
                    "H3,B,NTC_BOOT,adc=ADC1_IN0,sample_ms=%lu,log_ms=%lu,vdda_cfg_mv=%lu,r_fixed_ohm=%lu,cal=%u,ready=%u\r\n",
                    (unsigned long)H3_NTC_SAMPLE_PERIOD_MS,
                    (unsigned long)(H3_NTC_SAMPLE_PERIOD_MS * H3_NTC_LOG_DIVIDER),
                    (unsigned long)H3_VDDA_CONFIGURED_MV,
                    (unsigned long)H3_R_FIXED_OHM,
                    (unsigned int)g_h3_adc_cal_result,
                    (unsigned int)g_h3_adc_ready);

  if (length > 0)
  {
    line[sizeof(line) - 1U] = '\0';
    App_UartWrite(line);
  }
}

static void H3_LogNtcStatus(void)
{
  char line[256];
  uint32_t adc_mv_nominal = 0U;
  uint32_t r_ntc_ohm = 0U;
  uint8_t calculation_valid = 0U;
  int length;

  if (g_h3_adc_window_samples > 0U)
  {
    adc_mv_nominal = ((uint32_t)g_h3_adc_latest_raw * H3_VDDA_CONFIGURED_MV) /
                     H3_ADC_FULL_SCALE;
    if (g_h3_adc_latest_raw > 0U)
    {
      r_ntc_ohm = (uint32_t)(((uint64_t)H3_R_FIXED_OHM *
                              (H3_ADC_FULL_SCALE - g_h3_adc_latest_raw)) /
                             g_h3_adc_latest_raw);
      calculation_valid = 1U;
    }
  }

  length = snprintf(line, sizeof(line),
                    "H3,B,NTC,tick=%lu,cal=%u,ready=%u,raw=%u,raw_min=%u,raw_max=%u,adc_mv_nom=%lu,r_ntc_ohm=%lu,calc_valid=%u,samples=%lu,fail=%lu,last=%u\r\n",
                    (unsigned long)HAL_GetTick(),
                    (unsigned int)g_h3_adc_cal_result,
                    (unsigned int)g_h3_adc_ready,
                    (unsigned int)g_h3_adc_latest_raw,
                    (unsigned int)((g_h3_adc_window_samples > 0U) ?
                                   g_h3_adc_window_min : 0U),
                    (unsigned int)((g_h3_adc_window_samples > 0U) ?
                                   g_h3_adc_window_max : 0U),
                    (unsigned long)adc_mv_nominal,
                    (unsigned long)r_ntc_ohm,
                    (unsigned int)calculation_valid,
                    (unsigned long)g_h3_adc_samples_total,
                    (unsigned long)g_h3_adc_failures_total,
                    (unsigned int)g_h3_adc_last_result);

  if (length > 0)
  {
    line[sizeof(line) - 1U] = '\0';
    App_UartWrite(line);
  }

  g_h3_adc_window_min = H3_ADC_FULL_SCALE;
  g_h3_adc_window_max = 0U;
  g_h3_adc_window_samples = 0U;
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

  if ((qB_CanRxHandle == NULL) ||
      (osMessageQueuePut(qB_CanRxHandle, &frame, 0U, 0U) != osOK))
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
  /* creation of qB_CanRx */
  qB_CanRxHandle = osMessageQueueNew (4, sizeof(AppCanRxFrame_t), &qB_CanRx_attributes);

  /* creation of mbB_NodeAStatus */
  mbB_NodeAStatusHandle = osMessageQueueNew (1, sizeof(BNodeAStatus_t), &mbB_NodeAStatus_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of B_ComRxTask */
  B_ComRxTaskHandle = osThreadNew(StartBComRxTask, NULL, &B_ComRxTask_attributes);

  /* creation of B_Cyclic100msSupervisorTask */
  B_Cyclic100msSupervisorTaskHandle = osThreadNew(StartBCyclic100msSupervisorTask, NULL, &B_Cyclic100msSupervisorTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartBComRxTask */
/**
  * @brief  Function implementing the B_ComRxTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartBComRxTask */
void StartBComRxTask(void *argument)
{
  /* USER CODE BEGIN StartBComRxTask */
  AppCanRxFrame_t frame;
  uint8_t peer_counter;

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
    if (osMessageQueueGet(qB_CanRxHandle, &frame, NULL, osWaitForever) == osOK)
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
  /* USER CODE END StartBComRxTask */
}

/* USER CODE BEGIN Header_StartBCyclic100msSupervisorTask */
/**
* @brief Function implementing the B_Cyclic100msSupervisorTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartBCyclic100msSupervisorTask */
void StartBCyclic100msSupervisorTask(void *argument)
{
  /* USER CODE BEGIN StartBCyclic100msSupervisorTask */
  uint8_t counter = 0U;
  uint32_t cycle_count = 0U;
  uint16_t adc_raw = 0U;

  g_h3_adc_cal_result = (uint8_t)HAL_ADCEx_Calibration_Start(&hadc1);
  if (g_h3_adc_cal_result == (uint8_t)HAL_OK)
  {
    g_h3_adc_ready = 1U;
  }
  else
  {
    g_h3_adc_last_result = 5U;
  }

  while (g_h2_can_ready == 0U)
  {
    osDelay(10U);
  }
  osDelay(H2_CAN_STARTUP_DELAY_MS);
  H3_LogBoot();

  /* Infinite loop */
  for(;;)
  {
    if (g_h3_adc_ready != 0U)
    {
      g_h3_adc_last_result = H3_AdcSample(&adc_raw);
      if (g_h3_adc_last_result == 0U)
      {
        g_h3_adc_latest_raw = adc_raw;
        if (adc_raw < g_h3_adc_window_min)
        {
          g_h3_adc_window_min = adc_raw;
        }
        if (adc_raw > g_h3_adc_window_max)
        {
          g_h3_adc_window_max = adc_raw;
        }
        g_h3_adc_samples_total++;
        g_h3_adc_window_samples++;
      }
      else
      {
        g_h3_adc_failures_total++;
      }
    }

    H2_SendFrame(counter);
    counter = (uint8_t)((counter + 1U) & 0x0FU);
    cycle_count++;

    if ((cycle_count % 5U) == 0U)
    {
      HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    }
    if ((cycle_count % 10U) == 0U)
    {
      H2_LogStatus();
      H3_LogNtcStatus();
    }

    osDelay(H2_CAN_PERIOD_MS);
  }
  /* USER CODE END StartBCyclic100msSupervisorTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

