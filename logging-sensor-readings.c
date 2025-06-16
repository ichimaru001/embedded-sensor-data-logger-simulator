#include <stdio.h>
#include <time.h>
#include <Windows.h>
#include <stdint.h>

// *** BITWISE FLAGS
// ** BITWISE MASKS  
#define POWER_ON_MASK     ((uint8_t)(1U<<0))  // 00 00 00 01 (1)
#define BUSY_MASK         ((uint8_t)(1U<<1))  // 00 00 00 10 (2)
#define DATA_READY_MASK   ((uint8_t)(1U<<2))  // 00 00 01 00 (4)
#define ERROR_MASK        ((uint8_t)(1U<<3))  // 00 00 10 00 (8)
#define RESERVED_MASK     ((uint8_t)(15U<<4)) // 11 11 00 00 (240)

#define STATUS_ON 1
#define STATUS_OFF 0

#define E_SUCCESS 0
#define E_INVALID_SENSOR 1
#define E_INVALID_PARAM 2
#define E_FILE_ERROR 3

#define NUM_SENSORS 6

typedef struct {
  uint8_t sensorID;        // max 255
  uint8_t sensorReadDelay; // 255 seconds max delay
  uint8_t sensorStatus;
} sensorRegister;

int checkIfSensorBusy(sensorRegister *sensor) {
  if (sensor->sensorID > 255) {
    printf("\nError has occurred!\n");
    sensor->sensorStatus |= ERROR_MASK;
    return E_INVALID_SENSOR;
  }

  // printf("\nDATA_READY_MASK is %d\n", DATA_READY_MASK);
  // printf("\nBUSY_MASK is %d\n", BUSY_MASK);

  // check that sensor is not data ready
  sensor->sensorStatus &= ~DATA_READY_MASK;
  // set sensor to busy
  sensor->sensorStatus &= ~BUSY_MASK;
  sensor->sensorStatus |= BUSY_MASK;
  printf("\nSensor is set to busy!\n");
  // printf("\nSensor register is %X\n", sensor->sensorStatus);

  return E_SUCCESS;
}

int checkIfSensorDataReady(sensorRegister *sensor) {
  if (sensor->sensorID > 255) {
    printf("\nError has occurred!\n");
    sensor->sensorStatus |= ERROR_MASK;
    return E_INVALID_SENSOR;
  }

  // set sensor to data ready - ready to be logged
  sensor->sensorStatus &= ~DATA_READY_MASK;
  sensor->sensorStatus |= DATA_READY_MASK;
  printf("\nSensor is set to data ready!\n");
  // set sensor to not be busy
  sensor->sensorStatus &= ~BUSY_MASK;

  return E_SUCCESS;
}

int powerOnOffSensor(sensorRegister *sensor, uint8_t onOrOff) {
  if (sensor->sensorID > 255) {
    printf("\nError has occurred!\n");
    sensor->sensorStatus |= ERROR_MASK;
    return E_INVALID_SENSOR;
  }

  if (onOrOff == STATUS_ON) {
    sensor->sensorStatus |= POWER_ON_MASK;
    printf("\nSensor is powered on!\n");
    return E_SUCCESS;
  }
  else if (onOrOff == STATUS_OFF) {
    sensor->sensorStatus &= ~POWER_ON_MASK;
    printf("\nSensor is powered off!\n");
    return E_SUCCESS;
  }
  else {
    printf("\nError has occurred!\n");
    sensor->sensorStatus |= ERROR_MASK;
    return E_INVALID_PARAM;
  }
}



int main() {

  sensorRegister sensor1;
  sensor1.sensorID = 1;
  sensor1.sensorReadDelay = 5;
  sensor1.sensorStatus = 0;

  // powering on the sensor
  powerOnOffSensor(&sensor1, STATUS_ON);

  clock_t start = clock();
  uint8_t countdown = 25; // 25 seconds
  uint8_t sensorReadingDelay = sensor1.sensorReadDelay; // 5 seconds
  clock_t lastSensorRead = start;
  clock_t lastCountdownUpdate = start;


  while (sensor1.sensorStatus & POWER_ON_MASK) // when sensor is powered on
  {
    clock_t now = clock();
    clock_t elapsed = now - start;

    if (!(sensor1.sensorStatus & BUSY_MASK)) {
      checkIfSensorBusy(&sensor1);
    }

    // update last sensor read 
    if (elapsed - lastSensorRead >= sensorReadingDelay * CLOCKS_PER_SEC) {
      if (!(sensor1.sensorStatus & DATA_READY_MASK)) {
        checkIfSensorDataReady(&sensor1);
      }
      // prints if sensor is data ready
      if (sensor1.sensorStatus & DATA_READY_MASK) {
        printf("\nSensor has been read: %.2f milliseconds\n", (double)(elapsed - lastSensorRead) * 1000 / CLOCKS_PER_SEC);
      }
      lastSensorRead = elapsed;
    }


    // update last countdown update
    if (elapsed - lastCountdownUpdate >= 1 * CLOCKS_PER_SEC) {
      countdown--;
      printf("\nCountdown is now %d\n", countdown);
      lastCountdownUpdate = elapsed;
    }
    if (countdown <= 0) {
      powerOnOffSensor(&sensor1, 0);
      break;
    }

    Sleep(1);
  }


  return 0;
}