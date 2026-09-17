#ifndef NODE_B_APP_IPC_H
#define NODE_B_APP_IPC_H

#include <stdint.h>

/* CAN RX ISR -> B_ComRxTask: one raw CAN reception event */
typedef struct
{
  uint32_t rx_tick;
  uint16_t std_id;
  uint8_t dlc;
  uint8_t data[8];
} AppCanRxFrame_t;

/*
 * B_ComRxTask -> B_Cyclic100msSupervisorTask:
 * latest trusted Node A status plus communication supervision result
 */
typedef struct
{
  uint32_t last_raw_rx_tick;
  uint32_t last_trusted_rx_tick;

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

  uint8_t status_alive_counter;
  uint8_t has_raw_status;
  uint8_t has_trusted_status;

  uint8_t fb03_timeout_active;
  uint8_t fb04_sequence_fault_active;
  uint8_t fb04_safe_escalated;
} BNodeAStatus_t;

#endif