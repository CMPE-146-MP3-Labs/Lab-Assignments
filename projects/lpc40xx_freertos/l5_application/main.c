#include <stdio.h>

#include "FreeRTOS.h"
#include "acceleration.h"
#include "board_io.h"
#include "common_macros.h"
#include "event_groups.h"
#include "ff.h"
#include "periodic_scheduler.h"
#include "queue.h"
#include "sj2_cli.h"
#include "task.h"

QueueHandle_t sd_card_Q;
QueueHandle_t accel_data_Q;
EventGroupHandle_t Check_In;
char string[64];
const char *filename = "file.txt";
FIL file; // File handle

// 'static' to make these functions 'private' to this file
static void create_producer_task(void);
static void create_consumer_task(void);
static void create_watchdog_task(void);
static void producer_task(void *params);
static void consumer_task(void *params);
static void watchdog_task(void *params);

void write_file_using_fatfs_pi(void);

int main(void) {
  create_producer_task();
  create_consumer_task();
  create_watchdog_task();
  acceleration__init();
  Check_In = xEventGroupCreate();

  puts("Starting RTOS");
  vTaskStartScheduler(); // This function never returns unless RTOS scheduler
                         // runs out of memory and fails

  return 0;
}

static void create_producer_task() {
  xTaskCreate(producer_task, "Producer", (512U * 8) / sizeof(void *), NULL,
              PRIORITY_MEDIUM, NULL);
}

static void create_consumer_task() {
  xTaskCreate(consumer_task, "Consumer", (512U * 8) / sizeof(void *), NULL,
              PRIORITY_MEDIUM, NULL);
}

static void create_watchdog_task() {
  xTaskCreate(watchdog_task, "Watchdog", (512U * 8) / sizeof(void *), NULL,
              PRIORITY_HIGH, NULL);
}

static void producer_task(void *params) {
  while (1) {
    /*if (params == 0) {
      /*
      Create a producer task that reads a sensor value every 1ms.
      - The sensor can be any input type, such as a light sensor, or an
      acceleration sensor
      - After collecting 100 samples (after 100ms), compute the average
      - Write average value every 100ms (avg. of 100 samples) to the sensor
      queue
      - Use medium priority for this task
      
      acceleration__axis_data_s accel_data[100];
      acceleration__axis_data_s accel_data_avg;
      int16_t x, y, z = 0;

      for (int i = 0; i < 100; i++) {

        accel_data[i] = acceleration__get_data();
        x += accel_data[i].x;
        y += accel_data[i].y;
        z += accel_data[i].z;
        vTaskDelay(1);
      }
      accel_data_avg.x = x / 100;
      accel_data_avg.y = x / 100;
      accel_data_avg.z = x / 100;

      xQueueSend(accel_data_Q, &accel_data_avg, 0);

    }*/

    
      // Assume 100ms loop - vTaskDelay(100)
      // Sample code:
      // 1. get_sensor_value()
      // 2. xQueueSend(&handle, &sensor_value, 0);
      acceleration__axis_data_s accel_data[100];
      acceleration__axis_data_s accel_data_avg;
      int16_t x, y, z = 0;

      for (int i = 0; i < 100; i++) {

        accel_data[i] = acceleration__get_data();
        x += accel_data[i].x;
        y += accel_data[i].y;
        z += accel_data[i].z;
        vTaskDelay(1);
      }
      accel_data_avg.x = x / 100;
      accel_data_avg.y = x / 100;
      accel_data_avg.z = x / 100;

      if (xQueueSend(sd_card_Q, &accel_data_avg, 0)) {
      }
      xEventGroupSetBits(Check_In, 1 << 0);
      vTaskDelay(100);
      // 3. xEventGroupSetBits(checkin)
      // 4. vTaskDelay(100)
    
  }
}

static void consumer_task(void *params) {
  while (1) {
    /*if (params == 0) {
      /*
      Create a consumer task that pulls the data off the sensor queue
      - Use infinite timeout value during xQueueReceive API
      - Open a file (i.e.: sensor.txt), and append the data to an output file on
      the SD card
      - Save the data in this format: sprintf("%i, %i\n", time, light)"
      - Note that you can get the time using xTaskGetTickCount()
      - The sensor type is your choice (such as light or acceleration)
      - Note that if you write and close a file every 100ms, it may be very
      inefficient, so try to come up with a better method such that the file is
      only written once a second or so...
      - Also, note that periodically you may have to "flush" the file (or close
      it) otherwise data on the SD card may be cached and the file may not get
      written
      - Use medium priority for this task
      
      acceleration__axis_data_s accel_data_avg;
      xQueueReceive(accel_data_Q, &accel_data_avg, portMAX_DELAY);
      int16_t time = xTaskGetTickCount();
      sprintf(string, "%i x: %i, y: %i z: %i\n", time, accel_data_avg.x,
              accel_data_avg.y, accel_data_avg.z);
      write_file_using_fatfs_pi();
    }

    else if (params == 1) {*/
      // Assume 100ms loop
      // No need to use vTaskDelay() because the consumer will consume as fast
      // as production rate because we should block on xQueueReceive(&handle,
      // &item, portMAX_DELAY); Sample code:
      // 1. xQueueReceive(&handle, &sensor_value, portMAX_DELAY); // Wait
      // forever for an item
      // 2. xEventGroupSetBits(checkin)
      acceleration__axis_data_s accel_data_avg;

      if (xQueueReceive(sd_card_Q, &accel_data_avg, portMAX_DELAY)) {

        int16_t time = xTaskGetTickCount();
        sprintf(string, "%i x: %i, y: %i z: %i\n", time, accel_data_avg.x,
                accel_data_avg.y, accel_data_avg.z);
        write_file_using_fatfs_pi();
        xEventGroupSetBits(Check_In, 1 << 1);
      }
    
  }
}

/*
Create a watchdog task that monitors the operation of the two tasks.
- Use high priority for this task.
- Use a task delay of 1 second, and wait for all the bits to set. If there are
two tasks, wait for bit1, and bit2 etc.
- If you fail to detect the bits are set, that means that the other tasks did
not reach the end of the loop.
- Print a message when the Watchdog task is able to verify the check-in of other
tasks
- Print an error message clearly indicating which task failed to check-in with
the RTOS Event Groups API
*/
void watchdog_task(void *params) {
  while (1) {
    // ...
    vTaskDelay(200);
    // We either should vTaskDelay, but for better robustness, we should
    // block on xEventGroupWaitBits() for slightly more than 100ms because
    // of the expected production rate of the producer() task and its check-in
    const TickType_t xTicksToWait = 200;
    EventBits_t uxBits;
    uxBits = xEventGroupWaitBits(
        Check_In,      /* The event group being tested. */
        0x3,           /* The bits within the event group to wait for. */
        pdTRUE,        /* BIT_0 & BIT_1 should be cleared before returning. */
        pdTRUE,        /* Don't wait for both bits, either bit will do. */
        xTicksToWait); /* Wait a maximum of 100ms for either bit to be set. */

    if (uxBits == 0x0) {
      fprintf(stderr, "Task did not complete");
    } else if (uxBits == 0x3) {
      fprintf(stderr, "Task completed");
    }
  }
}

void write_file_using_fatfs_pi(void) {
  const char *filename = "file.txt";
  FIL file; // File handle
  UINT bytes_written = 0;
  FRESULT result = f_open(&file, filename, (FA_WRITE | FA_CREATE_ALWAYS));

  if (FR_OK == result) {
    // char string[64];
    // sprintf(string, "Value,%i\n", 123);
    if (FR_OK == f_write(&file, string, strlen(string), &bytes_written)) {
    } else {
      printf("ERROR: Failed to write data to file\n");
    }
    f_close(&file);
  } else {
    printf("ERROR: Failed to open: %s\n", filename);
  }
}
