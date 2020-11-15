

static void degugSerialOut(void *pvParameters) {
  portTASK_USES_FLOATING_POINT();
  const TickType_t xDelay = 500 / portTICK_PERIOD_MS;
  const TickType_t xTicksToWait = pdMS_TO_TICKS( 9500 );
  float heading;
  struct LatLongPosition currentPos;
  struct EnvironmentalData environmentalData;
  
  Serial.println("GPS + COMPASS Test Code - OpenGlider");
  
  for (;;) {
    //Serial.println("COMM loop pre delay");
    //vTaskDelay( xTicksToWait );
    
    //Serial.println("COMM loop post delay");
    
    if(xQueueReceive(queueHeading, &heading, ( TickType_t ) 10) == pdTRUE)
    {
      Serial.print("COMM heading=");
      Serial.println(heading);
    }
    
    if(xQueueReceive( queueLatLongPosition, &( currentPos ), ( TickType_t ) 10 ) == pdPASS )
    {
      Serial.print("latlong=");
      Serial.print(currentPos.lattitude);
      Serial.print(",");
      Serial.println(currentPos.longitude);
    }
    if(xQueueReceive( queueEnvironmental, &( environmentalData ), ( TickType_t ) 10 ) == pdPASS )
    {
      Serial.print("temp= ");
      Serial.print(environmentalData.tempF);
      Serial.print(", pressure Kpascals=");
      Serial.print(environmentalData.pressureKPascals);
      Serial.print(", humidity=");
      Serial.println(environmentalData.relativeHumidity);
      
//      Serial.print(", tempCO2=");
//      Serial.print(environmentalData.tempCO2);
//      Serial.print(", tempVOC=");
//      Serial.println(environmentalData.tempVOC);
    }
  }

  Serial.println("degugSerialOut: Deleting");
  vTaskDelete( NULL );
}

static char ptrTaskList[400]; //temporary string bufer for task stats

void taskMonitor(void *pvParameters)
{
    int x;
    int measurement;
    const TickType_t xTicksToWait = pdMS_TO_TICKS( 15000 );
    Serial.println("Task Monitor: Started");

    // run this task afew times before exiting forever
    while(1)
    {
      vTaskDelay(xTicksToWait); // print every 10 seconds

      Serial.println("****************************************************");
      Serial.print("Free Heap: ");
      Serial.print(xPortGetFreeHeapSize());
      Serial.println(" bytes");

      Serial.print("Min Heap: ");
      Serial.print(xPortGetMinimumEverFreeHeapSize());
      Serial.println(" bytes");

      Serial.println("****************************************************");
      Serial.println("Task            ABS             %Util");
      Serial.println("****************************************************");
#ifdef ESP32
   
#else 
    vTaskGetRunTimeStats(ptrTaskList); //save stats to char array
      Serial.println(ptrTaskList); //prints out already formatted stats
      Serial.println("****************************************************");
    Serial.println("Task            State   Prio    Stack   Num     Core" );
    Serial.println("****************************************************");

    vTaskList(ptrTaskList); //save stats to char array
    Serial.println(ptrTaskList); //prints out already formatted stats
#endif
      

    

    Serial.println("****************************************************");
    Serial.println("[Stacks Free Bytes Remaining] ");

    measurement = uxTaskGetStackHighWaterMark( handleTaskCompass );
    Serial.print("handleTaskCompass: ");
    Serial.println(measurement);

    measurement = uxTaskGetStackHighWaterMark( handleTaskDegugSerialOut );
    Serial.print("handleTaskDegugSerialOut: ");
    Serial.println(measurement);

//    measurement = uxTaskGetStackHighWaterMark( Handle_cTask );
//    Serial.print("Thread C: ");
//    Serial.println(measurement);

    measurement = uxTaskGetStackHighWaterMark( Handle_monitorTask );
    Serial.print("Monitor Stack: ");
    Serial.println(measurement);

    Serial.println("****************************************************");

    }

    // delete ourselves.
    // Have to call this or the system crashes when you reach the end bracket and then get scheduled.
    Serial.println("Task Monitor: Deleting");
    vTaskDelete( NULL );

}
