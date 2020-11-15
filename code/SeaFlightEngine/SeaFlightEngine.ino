//  SeaFlight Glider engine code
//
//  
//
//
//
//  Version 0.01    11/14/2020    John Reine    Initial code testing sensors + freertos
//

#ifdef ESP32
#include <FreeRTOS.h>
#else 
#include <FreeRTOS_SAMD51.h>
#endif

// Libraries for I2C and the HMC6343 sensor
#include <Wire.h>
#include "SFE_HMC6343.h"
#include "SparkFun_Ublox_Arduino_Library.h" //http://librarymanager/All#SparkFun_Ublox_GPS
//#include <avr/dtostrf.h>

#define DEBUG_SERIAL 1

TaskHandle_t handleTaskGPS;
TaskHandle_t handleTaskCompass;
TaskHandle_t handleTaskEnvironmental;
TaskHandle_t handleTaskDegugSerialOut;
TaskHandle_t Handle_monitorTask;
TaskHandle_t Handle_TaskBlink;
TaskHandle_t Handle_updateGliderState;

//sensor data queues
QueueHandle_t queueHeading;
QueueHandle_t queueLatLongPosition;
QueueHandle_t queueEnvironmental;

//state machines queues
QueueHandle_t queueGliderStateChange;
QueueHandle_t queueRefurbStateChange;
QueueHandle_t queueDeployStateChange;

//semaphores
SemaphoreHandle_t semaphoreI2C;

struct LatLongPosition
{
  long lattitude;
  long longitude;
  long alt;
} xMessage;

struct EnvironmentalData
{
  float tempF;
  float pressureKPascals;
  float altitudeFeet;
  float relativeHumidity;
  int tempCO2;
  int tempVOC;
} yMessage;

void TaskBlink( void *pvParameters );


void setup() {
  // Start serial communication at 115200 baud
  // Give the HMC6343 a half second to wake up
  const TickType_t xDelay = 2000 / portTICK_PERIOD_MS;
  delay(1000);
  Serial.begin(115200);
  while (!Serial);  //todo add timeout with error if serial can't be setup
  
  Wire.begin();

  queueHeading = xQueueCreate(10, sizeof(float));
  queueLatLongPosition = xQueueCreate(10, sizeof(LatLongPosition));
  queueGliderStateChange = xQueueCreate(1, sizeof(int));
  queueRefurbStateChange = xQueueCreate(1, sizeof(int));
  queueEnvironmental = xQueueCreate(10, sizeof(EnvironmentalData));
  
  semaphoreI2C = xSemaphoreCreateMutex();
  if ( semaphoreI2C == NULL )
  {
    Serial.println("ERRRR: There was insufficient FreeRTOS heap available for the semaphore to be created successfully.");
  }
  
  xTaskCreate(degugSerialOut, "degugSerialOut", 2048, NULL, tskIDLE_PRIORITY + 2, &handleTaskDegugSerialOut);
  xTaskCreate(readCompass,     "readCompass",       2048, NULL, tskIDLE_PRIORITY + 2, &handleTaskCompass);
  xTaskCreate(readGPS,          "readGPS",          2048, NULL, tskIDLE_PRIORITY + 2, &handleTaskGPS);
  xTaskCreate(readEnvironmental,"readEnvironmental",2048, NULL, tskIDLE_PRIORITY + 2, &handleTaskEnvironmental);
  //xTaskCreate(updateGliderState,    "updateGliderState",    1024, NULL, tskIDLE_PRIORITY + 1, &Handle_updateGliderState);
  //xTaskCreate(changeGliderState,  "changeGliderState",  1024, NULL, tskIDLE_PRIORITY + 1, &Handle_changeGliderState);
  xTaskCreate(taskMonitor, "Task Monitor", 1024, NULL, tskIDLE_PRIORITY + 1, &Handle_monitorTask);
  xTaskCreate(TaskBlink, "TaskBlink", 2048, NULL, tskIDLE_PRIORITY + 1, &Handle_TaskBlink);

#ifdef ESP32
// vTaskStartScheduler();
#else 
  vTaskStartScheduler();
#endif

}

void loop() {

}

void TaskBlink(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  // initialize digital LED_BUILTIN on pin 13 as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  for (;;) // A Task shall never return or exit.
  {
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    vTaskDelay(1000);  // one tick delay (15ms) in between reads for stability
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    vTaskDelay(1000);  // one tick delay (15ms) in between reads for stability
  }
}
