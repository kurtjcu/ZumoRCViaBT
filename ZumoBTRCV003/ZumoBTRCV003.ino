/* 2017-08-10
 * This file was created by Kurt@nqmakersupplies.com.au
 * Converting the throttle and steering into motor speeds is mostly code from
 * Pololu RC Zumo https://www.pololu.com/docs/0J57/7.a this code comes in the examples provided with the zumo libraries.
 * Note: on my version of arduino some of the Plololu Examples are broken and I had to find the example via the desktop.
 * Desigend to use with https://github.com/kurtjcu/JoystickControlToBT
 */

/**** if you are new or having some problems see the bottom of the file for sopme debugging hints ***/
 

#include <ZumoMotors.h>

#define LED_PIN       13 // user LED pin

#define MAX_SPEED          400 // max motor speed
#define SPEED_DIVISOR      .666 //
#define JOYSTICK_DEADBAND   10 // compensate for control centering offset
#define JOYSTICK_STEERING_RANGE     512 // make larger than 512 to make steering LESS sensitive
#define JOYSTICK_THROTTLE_RANGE     512 // pulse width difference from 512 us to be treated as full scale input (for example, a value of 512 means
                               //   any value <= 0 or >= 1023 is considered full scale)

#define STEERING_SENSITIVITY 2.5  //what do i divide the steering by?
String inputString = "";
char str[255];

int throttle, newThrottle; //Y joystick
int steering, newSteering; //X joystick

int left_speed, right_speed;

void setup() {

  // Setup throttle and steering to be at the modpoint.
  // this prevents the zumo running away before a control link is established.
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
    
    // RC signals encode information centered on 512 (analog in midpoint 1 - 1023); subtract 512 to get a value centered on 0
    // this conversion get us ready to use +- values for ZumoMotors
    int throttleForCalc = throttle - 512;
    int steeringForCalc = (steering - 512) / STEERING_SENSITIVITY;

    // apply deadband, deadband is giving us a little play with the center (home) position of the joystick.
    // cheap joysticks rarely sit EXACTLY on the midpoint, this code stops the robot crawling away when joystick is at rest.
    if (abs(throttleForCalc) <= JOYSTICK_DEADBAND)
      throttleForCalc = 0;
    if (abs(steeringForCalc) <= JOYSTICK_DEADBAND / STEERING_SENSITIVITY)
      steeringForCalc = 0;

    // mix throttle and steering inputs to obtain left & right motor speeds
    left_speed  = ((long)throttleForCalc * MAX_SPEED / JOYSTICK_THROTTLE_RANGE) + ((long)steeringForCalc * MAX_SPEED / JOYSTICK_STEERING_RANGE);
    right_speed = ((long)throttleForCalc * MAX_SPEED / JOYSTICK_THROTTLE_RANGE) - ((long)steeringForCalc * MAX_SPEED / JOYSTICK_STEERING_RANGE);

    // cap speeds to max
    left_speed = min(max(left_speed, -MAX_SPEED), MAX_SPEED);
    right_speed = min(max(right_speed, -MAX_SPEED), MAX_SPEED);

    //left_speed  = (int)(((float) left_speed) * SPEED_DIVISOR);
    //right_speed = (int)(((float) right_speed) * SPEED_DIVISOR);
    
    left_speed  = (float)left_speed * SPEED_DIVISOR;
    right_speed = (float)right_speed * SPEED_DIVISOR;
  
  }
  
  // Send the motor speeds to the motors.
  ZumoMotors::setSpeeds(left_speed, right_speed);
  
 /*     //uncomment to debug
 Serial.print("Throttle: "); 
 Serial.println(throttle); 
 Serial.print("Steering: "); 
 Serial.println(steering); 
 Serial.print("Left Speed: "); 
 Serial.println(left_speed);
 Serial.print("Right Speed: "); 
 Serial.println(right_speed);
  */
  
  /* This file was designed to use the HC-05 bluetooth module https://nqmakersupplies.com.au/shop/product/hc-05-bluetooth-rx-or-tx-142 
   * The BT modules have to be paired VIA AT commands to work in this sketch. This sketch will also function using zigbee or any other transparent wireless link.
   * For testing you can try connecting the RX/TX from each arduino without your radio links, this can help you rile out any radio configuration issues.
 */
 


}
