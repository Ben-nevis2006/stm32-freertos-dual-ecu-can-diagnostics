#ifndef NODE_A_APP_IPC_H
#define NODE_A_APP_IPC_H

#include <stdint.h>

/* CAN RX ISR -> A_ComRxTask: one raw CAN reception event */
typedef struct
{
  uint32_t rx_tick;
  uint16_t std_id;
  uint8_t dlc;
  uint8_t data[8];
} AppCanRxFrame_t;

/* A_ComRxTask -> A_ControlTask: latest qualified command state */
typedef struct
{
  uint32_t last_valid_tick;
  uint8_t cooling_demand_pct;
  uint8_t alive_counter;
  uint8_t has_valid_command;
  uint8_t fa07_active;
} ACommandState_t;

/* A_ControlTask -> A_Cyclic100msTask: latest actuator state */
typedef struct
{
  uint16_t actual_rpm;
  uint8_t availability;
  uint8_t control_mode;
  uint8_t fallback_active;
  uint8_t rpm_valid;

  uint8_t fa01_active;
  uint8_t fa02_level;
  uint8_t fa03_active;
  uint8_t fa04_active;
  uint8_t fa05_active;
  uint8_t fa06_active;
  uint8_t fa07_active;
} AStatusSnapshot_t;

#endif