/* The SparkFun CCS811/BME280 Environmental Combo Breakout takes care of all your atmospheric-quality sensing
  needs with the popular CCS811 and BME280 ICs. This unique breakout provides a variety of environmental data,
  including barometric pressure, humidity, temperature, TVOCs and equivalent CO2 (or eCO2) levels.*/
//The gas released by a venting Li-ion cell is mainly carbon dioxide (CO2).

//CCS811 i2c Address 0x5B  (with jumper 0x5A)
//BME280 i2c Address 0x77  (with jumper 0x76)

#define CCS811_ADDR 0x5B //Default I2C Address
#include "SparkFunBME280.h"
#include <SparkFunCCS811.h>

static void readEnvironmental(void *pvParameters) {
  TickType_t xLastWakeTime;
  portTASK_USES_FLOATING_POINT();
  const TickType_t xFrequency = 1000;
  const TickType_t xTicksToWait = pdMS_TO_TICKS( 1000 );
  BME280 myBME280;

  struct EnvironmentalData environmentalData;

  vTaskDelay( xTicksToWait );
  Serial.println("....readEnvironmental...init()");
  //CCS811 myCCS811(CCS811_ADDR);

  if ( semaphoreI2C != NULL )
  {
    if ( xSemaphoreTake( semaphoreI2C, portMAX_DELAY ) == pdTRUE )
    {
      myBME280.settings.commInterface = I2C_MODE;
      myBME280.settings.I2CAddress = 0x77;
      myBME280.settings.runMode = 3; //Normal mode
      myBME280.settings.tStandby = 0;
      myBME280.settings.filter = 4;
      myBME280.settings.tempOverSample = 5;
      myBME280.settings.pressOverSample = 5;
      myBME280.settings.humidOverSample = 5;

      byte id = myBME280.begin();
      if (id != 0x60)
      {
        Serial.println("Problem with BME280");
      }
      else
      {
        Serial.println("BME280 online");
      }
      xSemaphoreGive( semaphoreI2C );
    }
  }
  else {
    Serial.println("ERRR: EnvironmentalData: semaphoreI2C == NULL");
  }
//  if ( semaphoreI2C != NULL )
//  {
//    if ( xSemaphoreTake( semaphoreI2C, portMAX_DELAY ) == pdTRUE )
//    {
//      //      byte returnCode = myCCS811.begin();
//      //      Serial.print("myCCS8 init error: ");
//      //      Serial.println(myCCS811.statusString(returnCode));
//      if (myCCS811.begin() == false) //Connect to the CCS811 using Wire port
//      {
//        Serial.println(F("SparkFunCCS811 not detected at default I2C address. Please check wiring. Freezing."));
//      }
//      xSemaphoreGive( semaphoreI2C );
//    }
//  }
  xLastWakeTime = xTaskGetTickCount();

  for (;;) {
    //vTaskDelayUntil( &xLastWakeTime, xFrequency );
    vTaskDelay( xTicksToWait );
    if ( semaphoreI2C != NULL )
    {
      if (xSemaphoreTake(semaphoreI2C, portMAX_DELAY) == pdTRUE )
      {
        environmentalData.relativeHumidity = myBME280.readFloatHumidity();
        environmentalData.tempF = myBME280.readTempF();
        environmentalData.altitudeFeet = myBME280.readFloatAltitudeFeet();
        environmentalData.pressureKPascals = myBME280.readFloatPressure();//in PA  //for Hg: multiply by 0.0002953;

        xSemaphoreGive( semaphoreI2C );
        //environmentalData

        //VALIDATE

        if (queueEnvironmental != NULL) {
          if ( xQueueSend( queueEnvironmental, &environmentalData, portMAX_DELAY ) != pdPASS )
          {
            Serial.println("readEnvironmental: failed to add to queueEnvironmental");
          }
        }
        else {
          //error condition: queue not created
        }
      }
    }
    else {
      /* We could not obtain the semaphore and can therefore not access
        the shared resource safely. */
    }
  }

//  if ( semaphoreI2C != NULL )
//  {
//    if ( xSemaphoreTake(semaphoreI2C, portMAX_DELAY) == pdTRUE )
//    {
//      if (myCCS811.dataAvailable())
//      {
//        myCCS811.readAlgorithmResults();
//        environmentalData.tempCO2 = myCCS811.getCO2();
//        environmentalData.tempVOC = myCCS811.getTVOC();
//      }
//      else if (myCCS811.checkForStatusError())
//      {
//
//      }
//      xSemaphoreGive( semaphoreI2C );
//      //environmentalData
//      //VALIDATE
//      //add to queue
//      if (queueEnvironmental != NULL) {
//        if ( xQueueSend( queueEnvironmental, &environmentalData, portMAX_DELAY ) != pdPASS )
//        {
//          Serial.println("readEnvironmental: failed to add to queueEnvironmental");
//        }
//      }
//      else {
//        //error condition: queue not created
//      }
//    }
//    else {
//      /* We could not obtain the semaphore and can therefore not access
//        the shared resource safely. */
//    }
//  }
//  else {
//    //error condition: semaphone queue null
//  }

}
