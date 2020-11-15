

static void readGPS(void *pvParameters) {
  TickType_t xLastWakeTime;
  portTASK_USES_FLOATING_POINT();
  const TickType_t xFrequency = 1000;
  const TickType_t xTicksToWait = pdMS_TO_TICKS( 2000 );
  SFE_UBLOX_GPS myGPS;
  struct LatLongPosition currentPos;

  vTaskDelay( xTicksToWait );
  Serial.println("....readGPS...init()");

  if ( semaphoreI2C != NULL )
  {
    if ( xSemaphoreTake( semaphoreI2C, ( TickType_t ) 10 ) == pdTRUE )
    {
      Serial.println("GPS got semaphore");
      if (myGPS.begin() == false) //Connect to the Ublox module using Wire port
      {
        Serial.println(F("Ublox GPS not detected at default I2C address. Please check wiring. Freezing."));
      }
      else {
        myGPS.setI2COutput(COM_TYPE_UBX); //Set the I2C port to output UBX only (turn off NMEA noise)
        myGPS.saveConfiguration(); //Save the current settings to flash and BBR
      }
      xSemaphoreGive( semaphoreI2C );
    }
  }
  else {
    Serial.println("ERRR: GPS: semaphoreI2C == NULL");
  }
  xLastWakeTime = xTaskGetTickCount();
  for (;;) {
    //vTaskDelayUntil( &xLastWakeTime, xFrequency );
    vTaskDelay( xTicksToWait );
    if ( semaphoreI2C != NULL )
    {
      if ( xSemaphoreTake( semaphoreI2C, ( TickType_t ) 10 ) == pdTRUE )
      {
        currentPos.lattitude = myGPS.getLatitude();
        currentPos.longitude = myGPS.getLongitude();
        currentPos.alt = myGPS.getAltitude();

        xSemaphoreGive( semaphoreI2C );
        //VALIDATE
        //add to queue
        if (queueLatLongPosition != NULL) {
          if ( xQueueSend( queueLatLongPosition,
                           ( void * ) &currentPos,
                           ( TickType_t ) 10 ) != pdPASS )
          {
            //error condition:Failed to post the message, even after 10 ticks.
          }
        }
        else {
          //error condition: queue not created
        }
      }
      else {
        /* We could not obtain the semaphore and can therefore not access
          the shared resource safely. */
      }
    }
    else {
      //error condition: semaphone queue null
    }
  }
}
