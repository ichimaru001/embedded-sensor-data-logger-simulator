// TITLE          :       EMBEDDED SENSOR DATA LOGGER SIMULATOR           
// AUTHOR         :       ichimaru001       
// DATE                   11/06/25
// -
// DESCRIPTION    :       SIMULATES A BASIC EMBEDDED DEVICE THAT COLLECTS DATA
//                        FROM SENSORS, LOGS THEM TO MEMORY, AND ALLOWS EXTERNAL 
//                        TO LOGS.               
// -
// TIME TAKEN     :       (17MINS, 1HR 44MINS, 3HR 19MIN)
// TO COMPLETE

#include <stdio.h>
#include <stdint.h>
#include <Windows.h>


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



int main() {
  typedef struct {
    uint8_t sensorID;        // max 255
    uint8_t sensorReadDelay; // 255 seconds max delay
    uint8_t sensorStatus;
  } sensorRegister;

  uint8_t readingSensorTime = 25;  // sets how long the sensors will take reading (255 seconds max)
  // unable to configure sensor


  sensorRegister sensorList[] = { 0 };

  // ** INITIALIZE ALL SENSOR REGISTERS
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    sensorList[i].sensorID = i; // id = index in array
    sensorList[i].sensorReadDelay = 3 * 1000;
    sensorList[i].sensorStatus = 0;
    sensorList[i].sensorStatus |= POWER_ON_MASK;
  }

  // ** GET READING





  // *** SHELL
  printf("\n*** EMBEDDED SENSOR DATA LOGGER SIMULATOR ***\n");

  printf("Available commands:\n");
  printf("  help          -     Show this help message\n");
  printf("  sensor_list   -     List connected sensors\n");
  printf("  sensor_read   -     Read value from sensor\n");
  printf("  exit          -     Exit the program\n");

  return 0;
}