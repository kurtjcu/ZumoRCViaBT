#include <ZumoMotors.h>

#define LED_PIN       13 // user LED pin

#define MAX_SPEED          400 // max motor speed
#define JOYSTICK_DEADBAND   10 // compensate for control centering offset
#define JOYSTICK_RANGE     512 // pulse width difference from 512 us to be treated as full scale input (for example, a value of 512 means
                               //   any value <= 0 or >= 1023 is considered full scale)
String inputString = "";
char str[255];

int throttle, newThrottle; //Y joystick
int steering, newSteering; //X joystick

int left_speed, right_speed;

void setup() {

  throttle = 512;
  newThrottle = throttle;
  steering = 512;
  newSteering = steering;
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  Serial.begin(9600); // Default communication rate of the Bluetooth module
}
void loop() {
  if(Serial.available() > 0){ // Checks whether data is comming from the serial port
    inputString = Serial.readStringUntil('\n'); // Reads the data from the serial port
    inputString.toCharArray(str,255);

    sscanf(str,"%i-%i", &newSteering, &newThrottle);

    if(throttle != newThrottle) {
      throttle = newThrottle;
    } else if ( steering != newSteering ){
      steering = newSteering;
    }
  } else {
    
    // RC signals encode information in pulse width centered on 512 (microseconds); subtract 512 to get a value centered on 0
    int throttleForCalc = throttle - 512;
    int steeringForCalc = steering - 512;

    // apply deadband
    if (abs(throttleForCalc) <= JOYSTICK_DEADBAND)
      throttleForCalc = 0;
    if (abs(steeringForCalc) <= JOYSTICK_DEADBAND)
      steeringForCalc = 0;

    // mix throttle and steering inputs to obtain left & right motor speeds
    left_speed = ((long)throttleForCalc * MAX_SPEED / JOYSTICK_RANGE) - ((long)steeringForCalc * MAX_SPEED / JOYSTICK_RANGE);
    right_speed = ((long)throttleForCalc * MAX_SPEED / JOYSTICK_RANGE) + ((long)steeringForCalc * MAX_SPEED / JOYSTICK_RANGE);

    // cap speeds to max
    left_speed = min(max(left_speed, -MAX_SPEED), MAX_SPEED);
    right_speed = min(max(right_speed, -MAX_SPEED), MAX_SPEED);
    
  }

  Serial.print("left: ");
  Serial.println(left_speed);
  Serial.print("Right: ");
  Serial.println(right_speed);

  ZumoMotors::setSpeeds(left_speed, right_speed);
  
 /*
 if (state == '0') {
  digitalWrite(ledPin, LOW); // Turn LED OFF
  Serial.println("LED: OFF"); // Send back, to the phone, the String "LED: ON"
  state = 0;
 }
 else if (state == '1') {
  digitalWrite(ledPin, HIGH);
  Serial.println("LED: ON");;
  state = 0;
 } 
 */
}
