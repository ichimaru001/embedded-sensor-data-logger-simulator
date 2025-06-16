#include <stdio.h>
#include <time.h>
#include <Windows.h>
#include <stdint.h>

// *** BITWISE FLAGS
// ** BITWISE MASKS
#define POWER_ON_MASK     0b0001<<0 // 00 00 00 01
#define BUSY_MASK         0b0001<<1 // 00 00 00 10
#define DATA_READY_MASK   0b0001<<2 // 00 00 01 00
#define ERROR_MASK        0b0001<<3 // 00 00 10 00
#define RESERVED_MASK     0b1111<<4 // 11 11 00 00

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

int sensorReadOn(sensorRegister *sensor) {
  if (sensor->sensorID < 0 || sensor->sensorID > 255) {
    printf("\nError has occurred!\n");
    return E_INVALID_SENSOR;
  }

  // check that sensor is not data ready
  sensor->sensorStatus &= ~DATA_READY_MASK;
  // set sensor to busy
  sensor->sensorStatus &= ~BUSY_MASK;
  sensor->sensorStatus |= BUSY_MASK;
  printf("\nSensor is set to busy!\n");

  return E_SUCCESS;
}

int sensorLog(sensorRegister *sensor) {
  if (sensor->sensorID < 0 || sensor->sensorID > 255) {
    printf("\nError has occurred!\n");
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


int main() {

  sensorRegister sensor1;
  sensor1.sensorID = 1;
  sensor1.sensorReadDelay = 5;
  sensor1.sensorStatus = 0;

  uint8_t ifLogOn = 1;

  clock_t start = clock();
  uint8_t countdown = 25; // 25 seconds
  uint8_t sensorReadingDelay = sensor1.sensorReadDelay; // 5 seconds
  clock_t lastSensorRead = start;
  clock_t lastCountdownUpdate = start;



  while (1) // when sensor is busy
  {
    clock_t now = clock();
    clock_t elapsed = now - start;

    if (!(sensor1.sensorStatus & BUSY_MASK)) {
      sensorReadOn(&sensor1);
    }

    // update last sensor read 
    if (elapsed - lastSensorRead >= sensorReadingDelay * CLOCKS_PER_SEC) {
      if (!(sensor1.sensorStatus & DATA_READY_MASK)) {
        sensorLog(&sensor1);
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
      break;
    }

    Sleep(1);
  }



  return 0;
}