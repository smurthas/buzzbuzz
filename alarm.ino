#define SWITCH 0
#define LED 1
#define VIBE 2


const int DEBUG_MILLIS = 5000;

const unsigned long DEFAULT_SLEEP_TIME = 27900000L;
//const unsigned long DEFAULT_SLEEP_TIME = 10000L;
const int DEBUG_SLEEP_TIME = 2000;


unsigned long sleepTime = DEBUG_SLEEP_TIME;
//unsigned long sleepTime = DEFAULT_SLEEP_TIME;

bool wasSwitchDown = LOW;
bool isVibrating = false;
unsigned long now;
unsigned long downAt = 0;
bool toggledDebug = false;

unsigned long setTime = 0;


void toggleAlarm(bool state) {
  digitalWrite(LED, state);    // light up the LED
  digitalWrite(VIBE, state);    // vibe on
}

void quickVibe(int times, int wait) {
  for (int i = 0; i < times; i++) {
    toggleAlarm(HIGH);
    delay(wait);
    toggleAlarm(LOW);
    delay(wait);
  }
}


void setup() {
  // initialize the LED pin as an output.
  pinMode(LED, OUTPUT);
  pinMode(VIBE, OUTPUT);
  
  // initialize the SWITCH pin as an input.
  pinMode(SWITCH, INPUT);
  // ...with a pullup
  digitalWrite(SWITCH, HIGH);
}


void loop() {
  bool isSwitchDown = !digitalRead(SWITCH);
  bool isDebugMode = sleepTime == DEBUG_SLEEP_TIME;
  bool switchWentDown = !wasSwitchDown && isSwitchDown;
  bool switchWentUp = wasSwitchDown && !isSwitchDown;
  bool switchHeldDown = wasSwitchDown && isSwitchDown;
  wasSwitchDown = isSwitchDown;
  
  bool pastDebugHoldTime = (millis() - downAt) > DEBUG_MILLIS;
  
  if (switchWentDown) {
    downAt = millis();
  }
  
  if (switchHeldDown && pastDebugHoldTime && !toggledDebug) {
    // been holding the switch down for more than DEBUG_MILLIS.
    // give some feedback
    toggledDebug = true;
    if (!isDebugMode) {
      // switch to debug mode
      sleepTime = DEBUG_SLEEP_TIME;
      quickVibe(2, 1000);
    } else {
      // switch back to normal mode
      sleepTime = DEFAULT_SLEEP_TIME;
      quickVibe(2, 300);
    }
  }
  
  if (switchWentUp && pastDebugHoldTime) {
    toggledDebug = false;
    delay(250);
  } else if (switchWentUp) { // && !pastDebugHoldTime
    // button released, not debug

    if (setTime > 0 && !isVibrating) {
      // alarm already set, but not yet going off, deactivate
      setTime = 0;
      delay(100);
      if (isDebugMode) {
        quickVibe(2, 75);
      } else {
        quickVibe(2, 150);
      }
    } else if (isVibrating) {
      // alarm was sounding, turn it off
      setTime = 0;
      isVibrating = false;
      toggleAlarm(LOW);
      delay(100);
    } else {
      // activate alarm
      delay(100);
      if (isDebugMode) {
        quickVibe(3, 75);
      } else {
        // 5 buzzes to indicate 5 sleep cycles
        quickVibe(5, 150);
      }

      // set the time to go off in the future
      // round to the nearest even second plus a milli to ensure 
      // that the alarm goes off with a full vibration pulse instead 
      // of a partial one due to starting mid-second
      setTime = ((millis() + sleepTime) / 2000L) * 2000L + 1L;
    }
    delay(100);
  }

  if (setTime > 0 && setTime < millis() ) {
    // if alarm is set and we've passed the alarm time
    isVibrating = true;
  }


  if (isVibrating) {
    // alternate vibration on and off every second
    now = millis()/1000;
    if (now % 2 == 0) {
      toggleAlarm(HIGH);
    } else {
      toggleAlarm(LOW);
    }
  } else {
    toggleAlarm(LOW);
  }
}

