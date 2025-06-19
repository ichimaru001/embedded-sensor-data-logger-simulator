#include <stdio.h>
#include <time.h>
#include <Windows.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

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
  uint8_t sensorValue; // max 255 value
} sensorRegister;

void replaceChar(char *string, char target, char replacement) {
  for (int i = 0; i < strlen(string); i++)
  {
    if (string[i] == target) {
      string[i] = replacement;
    }
  }
}

void getTime(char *stringBuffer, size_t bufferSize) {
  time_t rawtime = 0;
  struct tm *pTime = NULL;

  time(&rawtime);
  pTime = localtime(&rawtime);

  snprintf(stringBuffer, bufferSize, "%02d/%02d/%02d - %02d:%02d:%02d", pTime->tm_mday, pTime->tm_mon + 1, pTime->tm_year - 100, pTime->tm_hour, pTime->tm_min, pTime->tm_sec);
}

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

int startSensorLogFile(char *fileName) {
  // get file name
  char suffixTime[50];
  char prefixFileName[50] = "sensor_log_file-"; // will be the final file name
  // get time
  getTime(suffixTime, sizeof(suffixTime));
  // replace [space] with '_'
  replaceChar(suffixTime, ' ', '_');
  replaceChar(suffixTime, ':', '-');
  replaceChar(suffixTime, '/', '-');

  // connect "sensor_log_file-" with timestamp
  strcat(prefixFileName, suffixTime);
  // connect result above with ".txt"
  strcat(prefixFileName, ".txt");
  // printf("%s\n", prefixFileName);

  char finalFileName[70] = "./sensor_logs/";
  strcat(finalFileName, prefixFileName);

  // finalFileName[strlen(finalFileName) - 1] = '\0';

  FILE *fptr = fopen(finalFileName, "w");

  if (fptr == NULL) {
    printf("\nError occurred!\nThe file is not opened!\n");
    return E_FILE_ERROR;
  }

  fclose(fptr);

  strcpy(fileName, finalFileName);
}
int writeSensorLogFile(char *log, char *fileName) { // (char *log)
  FILE *fptr = fopen(fileName, "a");

  if (fptr == NULL) {
    printf("\nError occurred!\nThe file is not opened!\n");
    return E_FILE_ERROR;
  }

  fprintf(fptr, "%s\n", log);

  fclose(fptr);
}

int startCountdownTakeReadings(sensorRegister *sensorList, int numCreatedSensors, int sensorCountdown) {
  int numPoweredOnSensors = 0;

  for (int i = 0; i < numCreatedSensors; i++)
  {
    // initialize elapsed since last read for each sensor
    sensorList[i].elapsedSinceLastRead = 0;
    // check how many sensors are on
    if (sensorList[i].sensorStatus & POWER_ON_MASK) {
      numPoweredOnSensors++;
    }
  }

  printf("\nThere are currently %d powered on sensors!\n", numPoweredOnSensors);

  clock_t start = clock();
  uint8_t countdown = sensorCountdown; // 25 seconds
  clock_t lastCountdownUpdate = start;

  char logFileName[70];
  // create file and get file name
  startSensorLogFile(logFileName);

  while (1) // when sensor is powered on
  {
    clock_t now = clock();
    clock_t elapsed = now - start;



    for (int i = 0; i < numCreatedSensors; i++)
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

        // set sensor's value to random value
        if (sensorList[i].sensorStatus & DATA_READY_MASK) {
          int randomSensorValue = (rand() % 255) + 1;
          sensorList[i].sensorValue = randomSensorValue;
        }

        // prints if sensor is data ready
        if ((sensorList[i].sensorStatus & DATA_READY_MASK) && (sensorList[i].sensorStatus & LOG_MASK)) {
          // printf("Sensor with ID %d has been read: %d seconds\n", sensorList[i].sensorID, sensorList[i].elapsedSinceLastRead);

          // get time
          char timeString[20];
          getTime(timeString, sizeof(timeString));
          // print log
          printf("%s | Sensor ID: %3d | Value: %3d | Status: OK\n", timeString, sensorList[i].sensorID, sensorList[i].sensorValue);
          // write log to file
          char logLine[65];
          snprintf(logLine, sizeof(logLine), "%s | Sensor ID: %3d | Value: %3d | Status: OK\n", timeString, sensorList[i].sensorID, sensorList[i].sensorValue);
          writeSensorLogFile(logLine, logFileName);
        }

        // resets elapsed since last read for each sensor to 0
        sensorList[i].elapsedSinceLastRead = 0;
      }
    }

    // update last countdown update
    if (elapsed - lastCountdownUpdate >= 1 * CLOCKS_PER_SEC) {
      countdown--;
      for (int i = 0; i < numCreatedSensors; i++)
      {
        if (sensorList[i].sensorStatus & POWER_ON_MASK) {
          sensorList[i].elapsedSinceLastRead++;
        }
      }

      printf("Countdown is now %d\n", countdown);
      lastCountdownUpdate = elapsed;
    }
    if (countdown <= 0) {
      for (int i = 0; i < numCreatedSensors; i++)
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
int setSensorCountdown(int *sensorCountdown, int userSensorCountdown) {
  // max 255 seconds
  if (userSensorCountdown <= 255 && userSensorCountdown >= 1) {
    *sensorCountdown = userSensorCountdown;
  }
  else {
    printf("\nError occurred!\nCountdown entered must be in between 1 and 255 inclusive!\n");
    return E_INVALID_PARAM;
  }
}

int checkMallocSensorList(sensorRegister **mallocSensorList, int *sensorListCapacity, int numCreatedSensors) {
  if (numCreatedSensors >= *sensorListCapacity) {
    *sensorListCapacity *= 2;
    *mallocSensorList = realloc(*mallocSensorList, *sensorListCapacity * sizeof(sensorRegister));
    if (*mallocSensorList == NULL) {
      printf("\nError occurred!\nMemory reallocation failed!\n");
      return 1;
    }
  }
  else if (numCreatedSensors < *sensorListCapacity / 2 && numCreatedSensors >= 2) {
    *sensorListCapacity /= 2;
    *mallocSensorList = realloc(*mallocSensorList, *sensorListCapacity * sizeof(sensorRegister));
    if (*mallocSensorList == NULL) {
      printf("\nError occurred!\nMemory reallocation failed!\n");
      return 1;
    }
  }

  printf("\nmallocSensorList size is now %d!\n", *sensorListCapacity);
}



void shell(sensorRegister *sensorList, int sensorListCapacity, int *numCreatedSensors, int *sensorCountdown) {
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
          checkMallocSensorList(&sensorList, &sensorListCapacity, *numCreatedSensors);
        }
        if (strcmp(userChoiceSensorList, "remove") == 0) {
          uint8_t userSensorID = 0;
          printf("** DELETING A SENSOR **\n");
          printf("Enter the ID of the sensor: ");
          scanf(" %d", &userSensorID);

          // clears the input buffer
          while (getchar() != '\n');

          removeSensor(sensorList, numCreatedSensors, userSensorID);
          checkMallocSensorList(&sensorList, &sensorListCapacity, *numCreatedSensors);
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
    if (strcmp(userChoice, "sensor_read") == 0) {
      char userChoiceSensorRead[13] = "\0";
      while (!(strcmp(userChoiceSensorRead, "previous") == 0))
      {
        printf("\n** SENSOR READ **\n");
        // admin commands
        printf("  countdown     -     Sets how long the sensors will take readings for (max 255 seconds)\n");
        printf("  start         -     starts the countdown and sensor begin taking readings\n");
        // list of sensors e.g.
        printf("  ---\n");
        printf("  Total:              %d\n", *numCreatedSensors);
        printAllSensorStatuses(sensorList, *numCreatedSensors);
        printf("  ---\n");
        printf("  Current countdown: %d seconds\n", *sensorCountdown);
        printf("  ---\n");
        printf("  previous      -     Go to the main menu\n");

        printf("Enter a command: ");
        fgets(userChoiceSensorRead, sizeof(userChoiceSensorRead), stdin);
        userChoiceSensorRead[strlen(userChoiceSensorRead) - 1] = '\0';

        if (strcmp(userChoiceSensorRead, "countdown") == 0) {
          int userSensorCountdown = 0;
          printf("** SETTING COUNTDOWN **\n");
          printf("Enter the number of seconds (1-255 inclusive): ");
          scanf(" %d", &userSensorCountdown);

          // clears the input buffer
          while (getchar() != '\n');

          setSensorCountdown(sensorCountdown, userSensorCountdown);
        }
        if (strcmp(userChoiceSensorRead, "start") == 0) {
          printf("** COUNTDOWN STARTED **\n");
          startCountdownTakeReadings(sensorList, *numCreatedSensors, *sensorCountdown);
        }
      }

    }
  } while (!(strcmp(userChoice, "exit") == 0));
}



int main() {
  // random based on time
  srand(time(NULL));

  char stringBuffer[20];
  getTime(stringBuffer, sizeof(stringBuffer));
  // replaceChar(stringBuffer, ' ', '_');
  printf("%s\n", stringBuffer);

  // char logLine[65];
  // snprintf(logLine, sizeof(logLine), "%s | Sensor ID: %3d | Value: %3d | Status: OK", stringBuffer, 3, 144);
  // writeSensorLogFile(logLine);

  // writeSensorLogFile();

  int sensorListCapacity = 2;
  int numCreatedSensors = 0;
  int sensorCountdown = 30; // default value is 30 seconds

  sensorRegister *mallocSensorList = (sensorRegister *)malloc(sizeof(sensorRegister) * sensorListCapacity);
  // checking if failed or pass
  if (mallocSensorList == NULL) {
    printf("\nError occurred!\nMemory allocation failed!\n");
    return 1;
  }

  for (int i = 0; i < 8; i++)
  {
    numCreatedSensors++;

    checkMallocSensorList(&mallocSensorList, &sensorListCapacity, numCreatedSensors);

    mallocSensorList[i].sensorID = i;
    mallocSensorList[i].sensorReadDelay = 5;

    initializeSensor(&mallocSensorList[i]);
    logOnOffSensor(mallocSensorList, &numCreatedSensors, STATUS_ON, i);
    powerOnOffSensor(mallocSensorList, &numCreatedSensors, STATUS_ON, i);
  }

  shell(mallocSensorList, sensorListCapacity, &numCreatedSensors, &sensorCountdown);


  return 0;
}

