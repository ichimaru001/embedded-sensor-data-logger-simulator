#include <stdio.h>
#include <time.h>
#include <Windows.h>
#include <stdint.h>

// bitwise masks
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
#define SLEEP_INTERVAL_MS 10


typedef struct {
  uint8_t sensorID;        // max 255
  uint8_t sensorReadDelay; // 255 seconds max delay - 1 second increment
  uint8_t elapsedSinceLastRead; // 255 seconds max elapsed
  uint8_t sensorStatus;
} sensorRegister;

int setSensorBusy(sensorRegister *sensor) {
  // printf("DATA_READY_MASK is %d\n", DATA_READY_MASK);
  // printf("BUSY_MASK is %d\n", BUSY_MASK);

  // check that sensor is not data ready
  sensor->sensorStatus &= ~DATA_READY_MASK;
  // set sensor to busy
  sensor->sensorStatus |= BUSY_MASK;
  printf("Sensor with ID %d is set to busy!\n", sensor->sensorID);
  // printf("Sensor register is %X\n", sensor->sensorStatus);

  return E_SUCCESS;
}
int setSensorDataReady(sensorRegister *sensor) {
  // set sensor to data ready - ready to be logged
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
int printSensorStatus(sensorRegister *sensor) {
  printf("Sensor ID %d has status 0x%02X\n", sensor->sensorID, sensor->sensorStatus);
  return E_SUCCESS;
}
int startCountdownTakeReadings() {
  sensorRegister sensorList[NUM_SENSORS] = { 0 };

  // sensor0
  sensorList[0].sensorID = 0;
  sensorList[0].sensorReadDelay = 3;
  // sensor1
  sensorList[1].sensorID = 1;
  sensorList[1].sensorReadDelay = 4;
  // sensor2
  sensorList[2].sensorID = 2;
  sensorList[2].sensorReadDelay = 5;
  // sensor3
  sensorList[3].sensorID = 3;
  sensorList[3].sensorReadDelay = 6;

  // test sensor1
  initializeSensor(&sensorList[1]);
  logOnOffSensor(&sensorList[1], STATUS_ON);
  powerOnOffSensor(&sensorList[1], STATUS_ON);
  // test sensor3
  initializeSensor(&sensorList[3]);
  logOnOffSensor(&sensorList[3], STATUS_ON);
  powerOnOffSensor(&sensorList[3], STATUS_ON);

  uint8_t numPoweredOnSensors = 0;

  // check how many sensors are on
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    if (sensorList[i].sensorStatus & POWER_ON_MASK) {
      numPoweredOnSensors++;
    }
  }

  printf("\nThere are currently %d powered on sensors!\n", numPoweredOnSensors);

  clock_t start = clock();
  uint8_t countdown = 25; // 25 seconds
  clock_t lastCountdownUpdate = start;


  while (1) // when sensor is powered on
  {
    clock_t now = clock();
    clock_t elapsed = now - start;


    for (int i = 0; i < NUM_SENSORS; i++)
    {
      if ((sensorList[i].sensorStatus & POWER_ON_MASK) && (!(sensorList[i].sensorStatus & BUSY_MASK))) {
        setSensorBusy(&sensorList[i]);
      }

      // update last sensor read 
      if ((sensorList[i].sensorStatus & POWER_ON_MASK) && (sensorList[i].elapsedSinceLastRead >= sensorList[i].sensorReadDelay)) {
        // sensor is set to data ready
        if (!(sensorList[i].sensorStatus & DATA_READY_MASK)) {
          setSensorDataReady(&sensorList[i]);
        }
        // prints if sensor is data ready
        if ((sensorList[i].sensorStatus & DATA_READY_MASK) && (sensorList[i].sensorStatus & LOG_MASK)) {
          printf("Sensor with ID %d has been read: %d seconds\n", sensorList[i].sensorID, sensorList[i].elapsedSinceLastRead);
        }

        // resets elapsed since last read for each sensor to 0
        sensorList[i].elapsedSinceLastRead = 0;
      }
    }

    // update last countdown update
    if (elapsed - lastCountdownUpdate >= 1 * CLOCKS_PER_SEC) {
      countdown--;
      for (int i = 0; i < NUM_SENSORS; i++)
      {
        if (sensorList[i].sensorStatus & POWER_ON_MASK) {
          sensorList[i].elapsedSinceLastRead++;
        }
      }

      printf("Countdown is now %d\n", countdown);
      lastCountdownUpdate = elapsed;
    }
    if (countdown <= 0) {
      for (int i = 0; i < NUM_SENSORS; i++)
      {
        if (sensorList[i].sensorStatus & POWER_ON_MASK) {
          powerOnOffSensor(&sensorList[i], STATUS_OFF);
          initializeSensor(&sensorList[i]);
        }
      }

      numPoweredOnSensors = 0;
      break;
    }

    Sleep(SLEEP_INTERVAL_MS);
  }

  return E_SUCCESS;
}

void tempFunction() {

  char userChoice[13] = "\0";

  do
  {
    // title
    printf("\n*** EMBEDDED SENSOR DATA LOGGER SIMULATOR ***\n");
    // commands
    printf("Available commands:\n");
    printf("  sensor_list   -     List connected sensors\n");
    printf("  sensor_read   -     Read value from sensors\n");
    printf("  exit          -     Exit the program\n");
    // user input for userChoice
    printf("Enter a command: ");
    fgets(userChoice, sizeof(userChoice), stdin);
    userChoice[strlen(userChoice) - 1] = '\0';
    // converts userChoice to all lowercase if contains any uppercase
    for (int i = 0; i < strlen(userChoice); i++)
    {
      userChoice[i] = tolower(userChoice[i]);
    }

    if (userChoice == "sensor_list") {
      printf("\n** SENSOR LIST **\n");
      // admin commands
      printf("  create        -     Creates a new sensor with random ID (max 4 sensors)\n");
      printf("  remove        -     Deletes a sensor according to ID");
      printf("  power         -     Choose which sensor to power on or off\n");
      printf("  log           -     Set which sensors will log their readings into a file\n");
      // list of sensors e.g.
      printf("-\n");

      printf("  sensor ID 3   -     Read Delay: 5 seconds, Powered: Off, Log:  On");
      printf("  sensor ID 245 -     Read Delay: 3 seconds, Powered:  On, Log: Off");

      printf("-\n");
      printf("  previous      -     Go to the main menu\n");
    }
    if (userChoice == "sensor_read") {
      printf("\n** SENSOR READ **\n");
      printf("  countdown     -     Sets how long the sensors will take readings for (max 255 seconds)\n");
      printf("  start         -     starts the countdown and sensor begin taking readings\n");
      // list of sensors e.g.
      printf("-\n");

      printf("  sensor ID 3   -     Read Delay: 5 seconds, Powered: Off, Log:  On");
      printf("  sensor ID 245 -     Read Delay: 3 seconds, Powered:  On, Log: Off");

      printf("-\n");
      printf("  previous      -     Go to the main menu\n");
    }


  } while (userChoice != "exit");

}



int main() {

  startCountdownTakeReadings();





  return 0;
}

