#include <stdio.h>
#include <time.h>
#include <Windows.h>
#include <stdint.h>

int main() {
  clock_t start = clock();
  uint8_t countdown = 25; // 25 seconds
  uint8_t sensorReadingDelay = 5; // 5 seconds
  clock_t lastSensorRead = start;
  clock_t lastCountdownUpdate = start;




  while (1)
  {
    clock_t now = clock();
    double elapsed = (double)(now - start) / CLOCKS_PER_SEC;
    // update last sensor read 
    if (elapsed - lastSensorRead >= sensorReadingDelay) {
      printf("Sensor has been read: %f seconds", elapsed - lastSensorRead);
      lastSensorRead = elapsed;
    }
    // update last countdown update
    if (elapsed - lastCountdownUpdate >= 1) {
      countdown--;
      printf("Countdown is now %d", countdown);
      lastCountdownUpdate = elapsed;
    }
    if (countdown <= 0) {
      break;
    }
  }




  // clock_t end = clock();
  // printf("%d", end);


  return 0;
}