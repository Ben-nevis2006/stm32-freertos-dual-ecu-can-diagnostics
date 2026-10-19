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
#include "tim.h"
#include "usart.h"

#include <stdio.h>
#include <string.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef struct
{
  uint8_t duty_percent;
  uint16_t compare;
} H3_FanPwmStep_t;

typedef struct
{
  uint32_t edge_total;
  uint32_t sample_total;
  uint32_t zero_delta_total;
  uint32_t window_samples;
  uint32_t last_edge_tick_ms;
  uint16_t last_period_ticks;
  uint16_t window_min_ticks;
  uint16_t window_max_ticks;
  uint8_t capture_seen;
} H3_TachSnapshot_t;

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
static volatile uint8_t g_h3_fan_profile_request;
static volatile uint32_t g_h3_tach_edge_count;
static volatile uint32_t g_h3_tach_period_sample_count;
static volatile uint32_t g_h3_tach_zero_delta_count;
static volatile uint32_t g_h3_tach_window_sample_count;
static volatile uint32_t g_h3_tach_last_edge_tick_ms;
static volatile uint16_t g_h3_tach_last_capture;
static volatile uint16_t g_h3_tach_last_period_ticks;
static volatile uint16_t g_h3_tach_window_min_ticks = UINT16_MAX;
static volatile uint16_t g_h3_tach_window_max_ticks;
static volatile uint8_t g_h3_tach_capture_seen;

static const H3_FanPwmStep_t g_h3_fan_pwm_profile[] =
{
  {100U, H3_FAN_PWM_COMPARE_100_PERCENT},
  { 50U, H3_FAN_PWM_COMPARE_50_PERCENT},
  { 25U, H3_FAN_PWM_COMPARE_25_PERCENT},
  {  0U, H3_FAN_PWM_COMPARE_0_PERCENT},
  {100U, H3_FAN_PWM_COMPARE_100_PERCENT}
};

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
static void H3_LogFanPwmProfile(void);
static void H3_ApplyFanPwmStep(uint8_t profile_index);
static void H3_LogFanPwmProfileDone(void);
static void H3_LogTachCaptureProfile(void);
static void H3_LogTachRaw(void);
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

static void H3_LogFanPwmProfile(void)
{
  char line[224];
  int length;

  length = snprintf(line, sizeof(line),
                    "H3,A,FAN_PWM_PROFILE,pin=PC7,timer=TIM3_CH2,freq_hz=%lu,idle_pct=%u,sequence=100-50-25-0-100,hold_ms=%lu,trigger=B1,mode=af_od\r\n",
                    (unsigned long)H3_FAN_PWM_FREQUENCY_HZ,
                    (unsigned int)H3_FAN_PWM_IDLE_DUTY_PERCENT,
                    (unsigned long)H3_FAN_PWM_PROFILE_HOLD_MS);

  if (length > 0)
  {
    line[sizeof(line) - 1U] = '\0';
    H2_UartWrite(line);
  }
}

static void H3_ApplyFanPwmStep(uint8_t profile_index)
{
  char line[160];
  int length;
  const uint8_t profile_step_count =
      (uint8_t)(sizeof(g_h3_fan_pwm_profile) / sizeof(g_h3_fan_pwm_profile[0]));

  if (profile_index >= profile_step_count)
  {
    return;
  }

  __HAL_TIM_SET_COMPARE(&htim3,
                        TIM_CHANNEL_2,
                        g_h3_fan_pwm_profile[profile_index].compare);
  length = snprintf(line, sizeof(line),
                    "H3,A,FAN_PWM_STEP,index=%u,total=%u,duty_pct=%u,compare=%u,hold_ms=%lu\r\n",
                    (unsigned int)(profile_index + 1U),
                    (unsigned int)profile_step_count,
                    (unsigned int)g_h3_fan_pwm_profile[profile_index].duty_percent,
                    (unsigned int)g_h3_fan_pwm_profile[profile_index].compare,
                    (unsigned long)H3_FAN_PWM_PROFILE_HOLD_MS);

  if (length > 0)
  {
    line[sizeof(line) - 1U] = '\0';
    H2_UartWrite(line);
  }
}

static void H3_LogFanPwmProfileDone(void)
{
  char line[96];
  int length;

  length = snprintf(line, sizeof(line),
                    "H3,A,FAN_PWM_PROFILE_DONE,final_pct=%u\r\n",
                    (unsigned int)H3_FAN_PWM_IDLE_DUTY_PERCENT);
  if (length > 0)
  {
    line[sizeof(line) - 1U] = '\0';
    H2_UartWrite(line);
  }
}

static void H3_LogTachCaptureProfile(void)
{
  char line[224];
  int length;

  length = snprintf(line, sizeof(line),
                    "H3,A,TACH_CAPTURE_PROFILE,pin=PA0,timer=TIM2_CH1,edge=falling,timer_hz=%lu,tick_us=%lu,ic_filter=%u,ckd_div=%u,rc=10k_series_10nF_gnd,ppr=TBD,rpm=INVALID\r\n",
                    (unsigned long)H3_TACH_TIMER_FREQUENCY_HZ,
                    (unsigned long)H3_TACH_TIMER_TICK_US,
                    (unsigned int)H3_TACH_INPUT_FILTER,
                    (unsigned int)H3_TACH_CLOCK_DIVISION_VALUE);

  if (length > 0)
  {
    line[sizeof(line) - 1U] = '\0';
    H2_UartWrite(line);
  }
}

static void H3_LogTachRaw(void)
{
  H3_TachSnapshot_t snapshot;
  uint32_t now;
  uint32_t age_ms;
  uint32_t period_us;
  uint32_t frequency_millihz;
  const char *state;
  char line[320];
  int length;

  taskENTER_CRITICAL();
  snapshot.edge_total = g_h3_tach_edge_count;
  snapshot.sample_total = g_h3_tach_period_sample_count;
  snapshot.zero_delta_total = g_h3_tach_zero_delta_count;
  snapshot.window_samples = g_h3_tach_window_sample_count;
  snapshot.last_edge_tick_ms = g_h3_tach_last_edge_tick_ms;
  snapshot.last_period_ticks = g_h3_tach_last_period_ticks;
  snapshot.window_min_ticks = (g_h3_tach_window_sample_count != 0U) ?
                              g_h3_tach_window_min_ticks : 0U;
  snapshot.window_max_ticks = (g_h3_tach_window_sample_count != 0U) ?
                              g_h3_tach_window_max_ticks : 0U;
  snapshot.capture_seen = g_h3_tach_capture_seen;
  g_h3_tach_window_sample_count = 0U;
  g_h3_tach_window_min_ticks = UINT16_MAX;
  g_h3_tach_window_max_ticks = 0U;
  taskEXIT_CRITICAL();

  now = HAL_GetTick();
  if (snapshot.capture_seen == 0U)
  {
    state = "WAIT_FIRST_EDGE";
    age_ms = UINT32_MAX;
  }
  else if (snapshot.sample_total == 0U)
  {
    state = "WAIT_SECOND_EDGE";
    age_ms = now - snapshot.last_edge_tick_ms;
  }
  else
  {
    state = "PERIOD_VALID";
    age_ms = now - snapshot.last_edge_tick_ms;
  }

  period_us = (uint32_t)snapshot.last_period_ticks * H3_TACH_TIMER_TICK_US;
  frequency_millihz = (snapshot.last_period_ticks != 0U) ?
      ((H3_TACH_TIMER_FREQUENCY_HZ * 1000U) /
       (uint32_t)snapshot.last_period_ticks) : 0U;

  length = snprintf(line, sizeof(line),
                    "H3,A,TACH_RAW,tick=%lu,state=%s,edge_total=%lu,sample_total=%lu,window_samples=%lu,last_ticks=%u,last_us=%lu,freq_mHz=%lu,window_min_ticks=%u,window_max_ticks=%u,zero_delta=%lu,age_ms=%lu,ppr=TBD,rpm=INVALID\r\n",
                    (unsigned long)now,
                    state,
                    (unsigned long)snapshot.edge_total,
                    (unsigned long)snapshot.sample_total,
                    (unsigned long)snapshot.window_samples,
                    (unsigned int)snapshot.last_period_ticks,
                    (unsigned long)period_us,
                    (unsigned long)frequency_millihz,
                    (unsigned int)snapshot.window_min_ticks,
                    (unsigned int)snapshot.window_max_ticks,
                    (unsigned long)snapshot.zero_delta_total,
                    (unsigned long)age_ms);

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

void HAL_GPIO_EXTI_Callback(uint16_t gpio_pin)
{
  if (gpio_pin == B1_Pin)
  {
    g_h3_fan_profile_request = 1U;
  }
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *tim_handle)
{
  uint16_t capture;
  uint16_t period_ticks;

  if ((tim_handle == NULL) ||
      (tim_handle->Instance != TIM2) ||
      (tim_handle->Channel != HAL_TIM_ACTIVE_CHANNEL_1))
  {
    return;
  }

  capture = (uint16_t)HAL_TIM_ReadCapturedValue(tim_handle, TIM_CHANNEL_1);
  g_h3_tach_edge_count++;
  g_h3_tach_last_edge_tick_ms = HAL_GetTick();

  if (g_h3_tach_capture_seen != 0U)
  {
    period_ticks = (uint16_t)(capture - g_h3_tach_last_capture);
    if (period_ticks == 0U)
    {
      g_h3_tach_zero_delta_count++;
    }
    else
    {
      g_h3_tach_last_period_ticks = period_ticks;
      g_h3_tach_period_sample_count++;
      g_h3_tach_window_sample_count++;
      if (period_ticks < g_h3_tach_window_min_ticks)
      {
        g_h3_tach_window_min_ticks = period_ticks;
      }
      if (period_ticks > g_h3_tach_window_max_ticks)
      {
        g_h3_tach_window_max_ticks = period_ticks;
      }
    }
  }
  else
  {
    g_h3_tach_capture_seen = 1U;
  }

  g_h3_tach_last_capture = capture;
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
  uint32_t profile_deadline = 0U;
  uint8_t profile_active = 0U;
  uint8_t profile_index = 0U;
  const uint8_t profile_step_count =
      (uint8_t)(sizeof(g_h3_fan_pwm_profile) / sizeof(g_h3_fan_pwm_profile[0]));

  g_h2_can_start_result = H2_CanStart();
  if (g_h2_can_start_result == 0U)
  {
    g_h2_can_ready = 1U;
  }
  H2_LogBoot(g_h2_can_start_result);
  H3_LogFanPwmProfile();
  H3_LogTachCaptureProfile();

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
    uint32_t now = HAL_GetTick();

    if ((profile_active == 0U) && (g_h3_fan_profile_request != 0U))
    {
      g_h3_fan_profile_request = 0U;
      profile_active = 1U;
      profile_index = 0U;
      H3_ApplyFanPwmStep(profile_index);
      profile_deadline = now + H3_FAN_PWM_PROFILE_HOLD_MS;
    }
    else if (profile_active != 0U)
    {
      /* Discard button bounce or extra presses while a profile is active. */
      g_h3_fan_profile_request = 0U;
      if ((int32_t)(now - profile_deadline) >= 0)
      {
        profile_index++;
        if (profile_index < profile_step_count)
        {
          H3_ApplyFanPwmStep(profile_index);
          profile_deadline = now + H3_FAN_PWM_PROFILE_HOLD_MS;
        }
        else
        {
          __HAL_TIM_SET_COMPARE(&htim3,
                                TIM_CHANNEL_2,
                                H3_FAN_PWM_COMPARE_50_PERCENT);
          profile_active = 0U;
          H3_LogFanPwmProfileDone();
        }
      }
    }

    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    half_second_count++;
    if ((half_second_count % 2U) == 0U)
    {
      H2_LogStatus();
      H3_LogTachRaw();
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

