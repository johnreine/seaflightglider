
static void readCompass(void *pvParameters) {
  TickType_t xLastWakeTime;
  portTASK_USES_FLOATING_POINT();
  const TickType_t xFrequency = 1000;
  const TickType_t xTicksToWait = pdMS_TO_TICKS( 1100 );
  float head = 1.2;
  SFE_HMC6343 compass; // Declare the sensor object

  vTaskDelay( xTicksToWait );
  Serial.println("....readCompass...init()");

  if ( semaphoreI2C != NULL )
  {
    if ( xSemaphoreTake( semaphoreI2C, ( TickType_t ) 10 ) == pdTRUE )
    {
      if (!compass.init())
      {
        Serial.println("Sensor Initialization Failed\n\r"); // Report failure, is the sensor wiring correct?
      }
      xSemaphoreGive( semaphoreI2C );
    }
  }
  else {
    Serial.println("ERRR: COMPASS: semaphoreI2C == NULL");
  }
  //xLastWakeTime = xTaskGetTickCount();
  for ( ;; ) {
    vTaskDelay( xTicksToWait );
    //vTaskDelayUntil( &xLastWakeTime, xFrequency );
    if (queueHeading != NULL) {
      if ( semaphoreI2C != NULL )
      {
        if ( xSemaphoreTake( semaphoreI2C, ( TickType_t ) 10 ) == pdTRUE )
        {
          compass.readHeading();
          xSemaphoreGive(semaphoreI2C);
          head = (float) compass.heading / 10.0;
          //verify number is in range of degrees
          if (head >= 0.0 && head <= 360.0) {
            if ( xQueueSend( queueHeading, (void*)&head, ( TickType_t ) 10 ) != pdPASS )
            {
              Serial.println("Failed to post the message, even after 10 ticks.");
              //error condition:Failed to post the message, even after 10 ticks.
            }
            else {
              //        if (DEBUGQUEU) {
              //          UBaseType_t numMessages = uxQueueMessagesWaiting(queueHeading);
              //          Serial.print("Q sent message: ");
              //          Serial.println(numMessages);
              //        }
            }
          }
        }
      }

    }
    else {
      Serial.println("ERRR: queueHeading == NULL");
    }

    /*
      vTaskDelayUntil( &xLastWakeTime, xFrequency );
      if ( semaphoreI2C != NULL )
      {
      if ( xSemaphoreTake( semaphoreI2C, ( TickType_t ) 10 ) == pdTRUE )
      {
        compass.readHeading();
        xSemaphoreGive( semaphoreI2C );
        float headingInDegrees = (float) compass.heading / 10.0;
        //verify number is in range of degrees
        if (headingInDegrees >= 0.0 && headingInDegrees <= 360.0) {
          //add to queue
          if (queueHeading != NULL) {
            if ( xQueueSend( queueHeading,
                             ( void * ) &headingInDegrees,
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
          //error condition: degrees is not valid
        }
      }
      else
      {
        //error condition:We could not obtain the semaphore and can therefore not access the shared resource safely.
      }
      }
    */


  }

  Serial.println("readCompass: Deleting");
  vTaskDelete( NULL );

}
