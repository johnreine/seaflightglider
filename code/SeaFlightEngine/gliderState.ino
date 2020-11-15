
enum GliderState {
  POWER_ON,
  POWER_OFF,
  DEPLOYING,
  DEPLOYED,
  RECOVERING,
  RECOVERED,
  REFURB
};

static GliderState gliderState;

static void updateGliderState(void *pvParameters) {
  gliderState = POWER_ON;
  GliderState newState = POWER_ON;
  
  Serial.println("updateGliderState init");
  
  if ( xQueueSend( queueGliderStateChange, &gliderState, ( TickType_t ) 10 ) != pdPASS )
  {
    Serial.println("updateGliderState: failed to add to queue");
  }
  
  for (;;) {
    if (xQueueReceive(queueGliderStateChange, &newState, ( TickType_t ) 10) == pdTRUE)
    {
      if (newState != gliderState) {
        Serial.print("GLIDER STATE CHANGE: from");
        Serial.print(gliderState);
        Serial.print(" to ");
        Serial.println(newState);

        switch (newState) {
          case POWER_ON:
            gliderState = newState;
            
            newState = REFURB;
            if ( xQueueSend( queueGliderStateChange, &newState, ( TickType_t ) 10 ) != pdPASS )
            {
              Serial.println("updateGliderState: failed to add to queue");
            }
            break;
          case POWER_OFF:
            gliderState = newState;
            break;
          case REFURB:
            
            gliderState = newState;
            break;
          case DEPLOYING:
            gliderState = newState;
            break;
          case DEPLOYED:
            gliderState = newState;
            break;
          case RECOVERING:
            gliderState = newState;
            break;
          case RECOVERED:
            gliderState = newState;
            break;
            Serial.println("updateGliderState: invalid state change");
        }
      }
    }
  }
}
