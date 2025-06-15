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
    clock_t elapsed = now - start;

    // update last sensor read 
    if (elapsed - lastSensorRead >= sensorReadingDelay * CLOCKS_PER_SEC) {
      printf("\nSensor has been read: %.2f milliseconds\n", (double)(elapsed - lastSensorRead) * 1000 / CLOCKS_PER_SEC);
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