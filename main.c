// TITLE          :       EMBEDDED SENSOR DATA LOGGER SIMULATOR           
// AUTHOR         :       ichimaru001       
// DATE                   11/06/25
// -
// DESCRIPTION    :       SIMULATES A BASIC EMBEDDED DEVICE THAT COLLECTS DATA
//                        FROM SENSORS, LOGS THEM TO MEMORY, AND ALLOWS EXTERNAL 
//                        TO LOGS.               
// -
// TIME TAKEN     :       (STARTED AT 1HR 35MIN)
// TO COMPLETE

#include <stdio.h>

int main() {

  // *** SHELL
  printf("\n*** EMBEDDED SENSOR DATA LOGGER SIMULATOR ***\n");

  printf("Available commands:\n");
  printf("  help          -     Show this help message\n");
  printf("  sensor_list   -     List connected sensors\n");
  printf("  sensor_read   -     Read value from sensor\n");
  printf("  exit          -     Exit the program\n");

  return 0;
}