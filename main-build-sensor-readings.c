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

int getIndexSensor(sensorRegister *sensorList, int numCreatedSensors, int userSensorID) {
  int sensorIndex = -1;
  // printf("The number of numCreatedSensors received in getIndexSensor is %d!\n", numCreatedSensors);
  // printf("\nI have received this many elements within the sensorList array: %d\n", sizeof(sensorList) / sizeof(sensorList[0]));
  for (int i = 0; i < numCreatedSensors; i++)
  {
    // printf("Have gone through sensor with ID %d\n", sensorList[i].sensorID);
    if (sensorList[i].sensorID == userSensorID) {
      sensorIndex = i;
      printf("userSensorID received is %d!\n", userSensorID);
      // break;
    }
  }

  // printf("\nSensor index is %d!\n", sensorIndex);

  if (sensorIndex == -1) { // if userSensorID is invalid (cannot be found)
    printf("\nError occurred!\nInvalid Sensor!\n");
    return E_INVALID_SENSOR;
  }

  return sensorIndex;
}

int powerOnOffSensor(sensorRegister *sensorList, int *numCreatedSensors, int onOrOff, int userSensorID) {
  int sensorIndex = 0;
  sensorIndex = getIndexSensor(sensorList, *numCreatedSensors, userSensorID);

  // printf("This is the index received from sensorIndex in powerOnOffSensor: %d\n", sensorIndex);

  if (onOrOff == STATUS_ON) {
    sensorList[sensorIndex].sensorStatus |= POWER_ON_MASK;
    printf("Sensor with ID %d is powered on!\n", sensorList[sensorIndex].sensorID);
    return E_SUCCESS;
  }
  else if (onOrOff == STATUS_OFF) {
    sensorList[sensorIndex].sensorStatus &= ~POWER_ON_MASK;
    printf("Sensor with ID %d is powered off!\n", sensorList[sensorIndex].sensorID);
    return E_SUCCESS;
  }
  else {
    printf("\nError has occurred in sensor with ID %d!\n", sensorList[sensorIndex].sensorID);
    sensorList[sensorIndex].sensorStatus |= ERROR_MASK;
    return E_INVALID_PARAM;
  }
}
int initializeSensor(sensorRegister *sensor) {
  sensor->sensorStatus = 0;
  printf("Sensor with ID %d has been initialized!\n", sensor->sensorID);

  return E_SUCCESS;
}
int logOnOffSensor(sensorRegister *sensorList, int *numCreatedSensors, int onOrOff, int userSensorID) {
  int sensorIndex = 0;
  sensorIndex = getIndexSensor(sensorList, *numCreatedSensors, userSensorID);

  if (onOrOff == STATUS_ON) {
    sensorList[sensorIndex].sensorStatus |= LOG_MASK;
    printf("Sensor with ID %d has log on!\n", sensorList[sensorIndex].sensorID);
    return E_SUCCESS;
  }
  else if (onOrOff == STATUS_OFF) {
    sensorList[sensorIndex].sensorStatus &= ~LOG_MASK;
    printf("Sensor with ID %d has log off!\n", sensorList[sensorIndex].sensorID);
    return E_SUCCESS;
  }
  else {
    printf("\nError has occurred in sensor with ID %d!\n", sensorList[sensorIndex].sensorID);
    sensorList[sensorIndex].sensorStatus |= ERROR_MASK;
    return E_INVALID_PARAM;
  }
}
int printSensorStatus(sensorRegister *sensor) {
  printf("Sensor ID %d has status 0x%02X\n", sensor->sensorID, sensor->sensorStatus);
  return E_SUCCESS;
}
void printAllSensorStatuses(sensorRegister *sensorList, int numCreatedSensors) {
  for (int i = 0; i < numCreatedSensors; i++)
  {
    printf("  sensor ID %3d -     Read Delay: %3d seconds, Powered: %3s, Log: %3s\n",
      sensorList[i].sensorID,
      sensorList[i].sensorReadDelay,
      (sensorList[i].sensorStatus & POWER_ON_MASK) ? "ON" : "OFF",
      (sensorList[i].sensorStatus & LOG_MASK) ? "ON" : "OFF"
    );
  }

}
int startCountdownTakeReadings(sensorRegister *sensorList, int numCreatedSensors) {
  int numPoweredOnSensors = 0;

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
          powerOnOffSensor(sensorList, &numCreatedSensors, STATUS_OFF, sensorList[i].sensorID);
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

int createSensor(sensorRegister *sensorList, int *numCreatedSensors) {
  uint8_t randomID = (rand() % 255) + 1;
  uint8_t checkIDCount = 1; // how many times the program should check between new ID and other IDs

  while (checkIDCount != 0)
  {
    for (int i = 1; i <= *numCreatedSensors; i++)
    {
      if (sensorList[i - 1].sensorID == randomID) {
        randomID = (rand() % 255) + 1;
        checkIDCount++;
      }
    }
    checkIDCount--;
  }

  sensorList[*numCreatedSensors].sensorID = randomID;
  sensorList[*numCreatedSensors].sensorReadDelay = 5; // 5 seconds delay is the default
  initializeSensor(&sensorList[*numCreatedSensors]);

  (*numCreatedSensors)++;
  // printf("\nA sensor had been created!\n");

  return E_SUCCESS;
}



int removeSensor(sensorRegister *sensorList, int *numCreatedSensors, int userSensorID) {
  int sensorIndex = 0;
  sensorIndex = getIndexSensor(sensorList, *numCreatedSensors, userSensorID);

  for (int i = sensorIndex; i < *numCreatedSensors; i++)
  {
    sensorList[i] = sensorList[i + 1];
  }

  (*numCreatedSensors)--;
  printf("\nA sensor had been removed!\n");

  return E_SUCCESS;
}

int setSensorReadDelay(sensorRegister *sensorList, int *numCreatedSensors, int userReadDelay, int userSensorID) {
  int sensorIndex = 0;
  sensorIndex = getIndexSensor(sensorList, *numCreatedSensors, userSensorID);

  if (userReadDelay <= 255 && userReadDelay >= 1) {
    sensorList[sensorIndex].sensorReadDelay = userReadDelay;
  }
  else {
    printf("\nError occurred!\nRead delay entered must be in between 0 and 255 inclusive!\n");
    return E_INVALID_PARAM;
  }
}


void shell(sensorRegister *sensorList, int *numCreatedSensors) {
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

    // printf("You have typed the following: %s\n", userChoice);

    if (strcmp(userChoice, "sensor_list") == 0) {
      char userChoiceSensorList[13] = "\0";

      while (!(strcmp(userChoiceSensorList, "previous") == 0))
      {
        printf("\n** SENSOR LIST **\n");
        // admin commands
        printf("  create        -     Creates a new sensor with random ID (max 4 sensors)\n");
        printf("  remove        -     Deletes a sensor according to ID\n");
        printf("  power         -     Choose which sensor to power on or off\n");
        printf("  log           -     Set which sensors will log their readings into a file\n");
        printf("  delay         -     Set the read delay of a sensor (in seconds)\n");
        // list of sensors e.g.
        printf("  ---\n");
        printf("  Total:              %d\n", *numCreatedSensors);
        printAllSensorStatuses(sensorList, *numCreatedSensors);
        printf("  ---\n");
        printf("  previous      -     Go to the main menu\n");

        printf("Enter a command: ");
        fgets(userChoiceSensorList, sizeof(userChoiceSensorList), stdin);
        userChoiceSensorList[strlen(userChoiceSensorList) - 1] = '\0';

        if (strcmp(userChoiceSensorList, "create") == 0) {
          createSensor(sensorList, numCreatedSensors);
        }
        if (strcmp(userChoiceSensorList, "remove") == 0) {
          uint8_t userSensorID = 0;
          printf("** DELETING A SENSOR **\n");
          printf("Enter the ID of the sensor: ");
          scanf(" %d", &userSensorID);

          // clears the input buffer
          while (getchar() != '\n');

          removeSensor(sensorList, numCreatedSensors, userSensorID);
        }
        if (strcmp(userChoiceSensorList, "power") == 0) {
          int userSensorID = 0;
          int userPowerOnOff = 0;

          printf("** SETTING POWER FOR SENSOR **\n");
          printf("Enter the ID of the sensor: ");
          scanf(" %d", &userSensorID);

          printf("Obtained userSensorID of %d in power!\n", userSensorID);

          // clears the input buffer
          while (getchar() != '\n');

          printf("Type 1 for power on and 0 for power off: ");
          scanf(" %d", &userPowerOnOff);

          // clears the input buffer
          while (getchar() != '\n');

          printf("Obtained userSensorID of %d in power!\n", userSensorID);

          powerOnOffSensor(sensorList, numCreatedSensors, userPowerOnOff, userSensorID);
        }
        if (strcmp(userChoiceSensorList, "log") == 0) {
          int userSensorID = 0;
          int userLogOnOff = 0;

          printf("** SETTING LOG FOR SENSOR **\n");
          printf("Enter the ID of the sensor: ");
          scanf(" %d", &userSensorID);

          // clears the input buffer
          while (getchar() != '\n');

          printf("Type 1 for log on and 0 for log off: ");
          scanf(" %d", &userLogOnOff);

          // clears the input buffer
          while (getchar() != '\n');

          logOnOffSensor(sensorList, numCreatedSensors, userLogOnOff, userSensorID);
        }
        if (strcmp(userChoiceSensorList, "delay") == 0) {
          int userSensorID = 0;
          int userReadDelay = 0;

          printf("** SETTING DELAY FOR SENSOR **\n");
          printf("Enter the ID of the sensor: ");
          scanf(" %d", &userSensorID);

          // clears the input buffer
          while (getchar() != '\n');

          printf("Enter a value between 1 and 255 inclusive: ");
          scanf(" %d", &userReadDelay);

          // clears the input buffer
          while (getchar() != '\n');

          setSensorReadDelay(sensorList, numCreatedSensors, userReadDelay, userSensorID);
        }
      }
    }
  } while (!(strcmp(userChoice, "exit") == 0));
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

      printf("  sensor ID 003 -     Read Delay: 5 seconds, Powered: Off, Log:  On");
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
  // random based on time
  srand(time(NULL));

  sensorRegister sensorList[NUM_SENSORS] = { 0 };
  int numCreatedSensors = 2;

  // if (numCreatedSensors > 0) {
  //   startCountdownTakeReadings(sensorList);
  // }
  // else {
  //   printf("No sensors detected!\nCountdown did not start!\n");
  // }

  sensorList[0].sensorID = 0;
  sensorList[0].sensorReadDelay = 3;

  sensorList[1].sensorID = 1;
  sensorList[1].sensorReadDelay = 4;

  // sensorList[2].sensorID = 2;
  // sensorList[2].sensorReadDelay = 5;

  // sensorList[3].sensorID = 3;
  // sensorList[3].sensorReadDelay = 6;

  // test sensor1
  initializeSensor(&sensorList[1]);
  logOnOffSensor(sensorList, &numCreatedSensors, STATUS_OFF, 0);
  powerOnOffSensor(sensorList, &numCreatedSensors, STATUS_OFF, 0);
  // test sensor3
  initializeSensor(&sensorList[3]);
  logOnOffSensor(sensorList, &numCreatedSensors, STATUS_ON, 1);
  powerOnOffSensor(sensorList, &numCreatedSensors, STATUS_ON, 1);

  shell(sensorList, &numCreatedSensors);


  return 0;
}

