enum RefurbState {
  INIT,
  DATA,
  TESTING,
  BURN_IN,
  WAITING_MENU,
  INIT_DEPLOY
};

static RefurbState refurbState;

static void updateRefurbState(void *pvParameters) {
  refurbState = INIT;
  RefurbState newState = INIT;
  if ( xQueueSend( queueRefurbStateChange, &refurbState, ( TickType_t ) 10 ) != pdPASS )
  {
    Serial.println("updateRefurbState: failed to add to queue");
  }

  for (;;) {
    if (xQueueReceive(queueRefurbStateChange, &newState, ( TickType_t ) 10) == pdTRUE)
    {
      if (newState != refurbState) {
        Serial.print("refurb STATE CHANGE: from");
        Serial.print(refurbState);
        Serial.print(" to ");
        Serial.println(newState);

        switch (newState) {
          case INIT:
            refurbState = newState;


            newState = WAITING_MENU;
            if ( xQueueSend( queueRefurbStateChange, &newState, ( TickType_t ) 10 ) != pdPASS )
            {
              Serial.println("updaterefurbState: failed to add to queue");
            }
            break;
          case DATA:
            refurbState = newState;
            break;
          case TESTING:
            refurbState = newState;
            break;
          case BURN_IN:
            refurbState = newState;
            break;
          case WAITING_MENU:
            refurbState = newState;
            break;
          case INIT_DEPLOY:
            refurbState = newState;
            break;
            Serial.println("updaterefurbState: invalid state change");
        }
      }
    }

    if (refurbState == INIT) {
      //TODO: check external pressure.. if we are in water then proceed to 'deploy state'
    }
    else if (refurbState == DATA) {

    }
    else if (refurbState == TESTING) {

    }
    else if (refurbState == BURN_IN) {

    }
    else if (refurbState == WAITING_MENU) {

    }
    else if (refurbState == INIT_DEPLOY) {

    }

  }
}
