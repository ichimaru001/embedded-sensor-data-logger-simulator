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
#define LOG_MASK          ((uint8_t)(1U<<4))  // 00 01 00 00 (16)
#define RESERVED_MASK     ((uint8_t)(14U<<4)) // 11 10 00 00 (224)

#define STATUS_ON 1
#define STATUS_OFF 0

#define E_SUCCESS 0
#define E_INVALID_SENSOR 1
#define E_INVALID_PARAM 2
#define E_FILE_ERROR 3

#define NUM_SENSORS 4


typedef struct {
  uint8_t sensorID;        // max 255
  uint8_t sensorReadDelay; // 255 seconds max delay
  uint8_t sensorStatus;
} sensorRegister;

int checkIfSensorBusy(sensorRegister *sensor) {
  // printf("DATA_READY_MASK is %d\n", DATA_READY_MASK);
  // printf("BUSY_MASK is %d\n", BUSY_MASK);

  // check that sensor is not data ready
  sensor->sensorStatus &= ~DATA_READY_MASK;
  // set sensor to busy
  sensor->sensorStatus &= ~BUSY_MASK;
  sensor->sensorStatus |= BUSY_MASK;
  printf("Sensor with ID %d is set to busy!\n", sensor->sensorID);
  // printf("Sensor register is %X\n", sensor->sensorStatus);

  return E_SUCCESS;
}
int checkIfSensorDataReady(sensorRegister *sensor) {
  // set sensor to data ready - ready to be logged
  sensor->sensorStatus &= ~DATA_READY_MASK;
  sensor->sensorStatus |= DATA_READY_MASK;
  printf("Sensor with ID %d is set to data ready!\n", sensor->sensorID);
  // set sensor to not be busy
  sensor->sensorStatus &= ~BUSY_MASK;

  return E_SUCCESS;
}
int powerOnOffSensor(sensorRegister *sensor, uint8_t onOrOff) {
  if (onOrOff == STATUS_ON) {
    sensor->sensorStatus |= POWER_ON_MASK;
    printf("Sensor with ID %d is powered on!\n", sensor->sensorID);
    return E_SUCCESS;
  }
  else if (onOrOff == STATUS_OFF) {
    sensor->sensorStatus &= ~POWER_ON_MASK;
    printf("Sensor with ID %d is powered off!\n", sensor->sensorID);
    return E_SUCCESS;
  }
  else {
    printf("\nError has occurred in sensor with ID %d!\n", sensor->sensorID);
    sensor->sensorStatus |= ERROR_MASK;
    return E_INVALID_PARAM;
  }
}
int initializeSensor(sensorRegister *sensor) {
  sensor->sensorStatus = 0;
  printf("Sensor with ID %d has been initialized!\n", sensor->sensorID);

  return E_SUCCESS;
}
int logOnOffSensor(sensorRegister *sensor, uint8_t onOrOff) {
  if (onOrOff == STATUS_ON) {
    sensor->sensorStatus |= LOG_MASK;
    printf("Sensor with ID %d has log on!\n", sensor->sensorID);
    return E_SUCCESS;
  }
  else if (onOrOff == STATUS_OFF) {
    sensor->sensorStatus &= ~LOG_MASK;
    printf("Sensor with ID %d has log off!\n", sensor->sensorID);
    return E_SUCCESS;
  }
  else {
    printf("\nError has occurred in sensor with ID %d!\n", sensor->sensorID);
    sensor->sensorStatus |= ERROR_MASK;
    return E_INVALID_PARAM;
  }
}

int main() {
  sensorRegister sensor1;
  sensor1.sensorID = 1;
  sensor1.sensorReadDelay = 5;

  sensorRegister sensor2;
  sensor2.sensorID = 2;
  sensor2.sensorReadDelay = 3;

  sensorRegister *currentSensor;
  currentSensor = &sensor2;

  // initializing / powering on the sensor
  initializeSensor(currentSensor);
  logOnOffSensor(currentSensor, STATUS_ON);
  powerOnOffSensor(currentSensor, STATUS_ON);

  clock_t start = clock();
  uint8_t countdown = 25; // 25 seconds
  uint8_t sensorReadingDelay = currentSensor->sensorReadDelay; // 5 seconds
  clock_t lastSensorRead = start;
  clock_t lastCountdownUpdate = start;


  while (currentSensor->sensorStatus & POWER_ON_MASK) // when sensor is powered on
  {
    clock_t now = clock();
    clock_t elapsed = now - start;

    if (!(currentSensor->sensorStatus & BUSY_MASK)) {
      checkIfSensorBusy(currentSensor);
    }

    // update last sensor read 
    if (elapsed - lastSensorRead >= sensorReadingDelay * CLOCKS_PER_SEC) {
      if (!(currentSensor->sensorStatus & DATA_READY_MASK)) {
        checkIfSensorDataReady(currentSensor);
      }
      // prints if sensor is data ready
      if ((currentSensor->sensorStatus & DATA_READY_MASK) && (currentSensor->sensorStatus & LOG_MASK)) {
        printf("Sensor with ID %d has been read: %.2f milliseconds\n", currentSensor->sensorID, (double)(elapsed - lastSensorRead) * 1000 / CLOCKS_PER_SEC);
      }
      lastSensorRead = elapsed;
    }

    // update last countdown update
    if (elapsed - lastCountdownUpdate >= 1 * CLOCKS_PER_SEC) {
      countdown--;
      printf("Countdown is now %d\n", countdown);
      lastCountdownUpdate = elapsed;
    }
    if (countdown <= 0) {
      powerOnOffSensor(currentSensor, 0);
      break;
    }

    Sleep(1);
  }


  return 0;
}