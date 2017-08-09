#include <ZumoMotors.h>
#include <SoftwareSerial.h>

//#define THROTTLE_PIN   4 // throttle channel from RC receiver //changed to use serial
//#define STEERING_PIN   5 // steering channel from RC receiver //changed to use serial
#define LED_PIN       13 // user LED pin

#define MAX_SPEED             400 // max motor speed
#define JOYSTICK_DEADBAND   10 // compensate for control centering offset
#define JOYSTICK_RANGE     512 // pulse width difference from 512 us to be treated as full scale input (for example, a value of 350 means
                                  //   any pulse width <= 1150 us or >= 1850 us is considered full scale)

  
SoftwareSerial BTserial(2, 4); // RX | TX
// Connect the HC-05 TX to Arduino pin 2 RX. 
// Connect the HC-05 RX to Arduino pin 4 TX through a voltage divider.

int throttle; //Y joystick
int steering; //X joystick

void setup()
{
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(9600);
  Serial.println("Connected");
  
  BTserial.begin(9600); 

  throttle = 512; //set motors to stop and intial values to 0 on joystick;
  steering = 512; //set motors to stop and intial values to 0 on joystick;

  //uncomment one or both of the following lines if your motors' directions need to be flipped
  //motors.flipLeftMotor(true);
  //motors.flipRightMotor(true);
}

void loop()
{

    // Keep reading from HC-05 and send to Arduino Serial Monitor
    
    if (BTserial.available())
    {  
        c = BTserial.read();
        Serial.write(c);
    }


  
  //int throttle = pulseIn(THROTTLE_PIN, HIGH); //change to use serial values.
  //int steering = pulseIn(STEERING_PIN, HIGH);

  int left_speed, right_speed;

  if (throttle > 0 && steering > 0) //may need to change this condition as we use 0,0 for rev left full!
  {
    // both RC signals are good; turn on LED
    digitalWrite(LED_PIN, HIGH);

    // RC signals encode information in pulse width centered on 512 (microseconds); subtract 512 to get a value centered on 0
    throttle -= 512;
    steering -= 512;

    // apply deadband
    if (abs(throttle) <= JOYSTICK_DEADBAND)
      throttle = 0;
    if (abs(steering) <= JOYSTICK_DEADBAND)
      steering = 0;

    // mix throttle and steering inputs to obtain left & right motor speeds
    left_speed = ((long)throttle * MAX_SPEED / JOYSTICK_RANGE) - ((long)steering * MAX_SPEED / JOYSTICK_RANGE);
    right_speed = ((long)throttle * MAX_SPEED / JOYSTICK_RANGE) + ((long)steering * MAX_SPEED / JOYSTICK_RANGE);

    // cap speeds to max
    left_speed = min(max(left_speed, -MAX_SPEED), MAX_SPEED);
    right_speed = min(max(right_speed, -MAX_SPEED), MAX_SPEED);
  }
  else
  {
    // at least one RC signal is not good; turn off LED and stop motors
    digitalWrite(LED_PIN, LOW);

    left_speed = 0;
    right_speed = 0;
  }

  ZumoMotors::setSpeeds(left_speed, right_speed);
}
