/* Cody Swain and JD */
/* Line-following robot */

// Defining arduino pin numbers for sensor pins 1-8
int sensorPins[] = {65, 48, 64, 47, 52, 68, 53, 69};

// sensorRead elements updated with each sensorRead()
int sensorRead[8]; // 1 or 0

int sensorError[] = {3, 2, 1, 0, 0, -1, -2, -3};

// Sensor sampling config
int t_on = 12; //microseconds (us)
int t_wait = 1000; //microseconds (us)

// Left Motor Pin Declarations
const int LEFT_PWM = 40;
const int LEFT_DIR = 14;
const int LEFT_SLP = 31;

// Right Motor Pin Declarations
const int RIGHT_PWM = 39;
const int RIGHT_DIR = 15;
const int RIGHT_SLP = 11; 

// Variable PWM Values
int rightPWM = 30;
int leftPWM = 30; 

float error = 0;

// Previous Error 
float previousError = 0;
float errorSlope = 0;
int scalar1 = 0;
int scalar2 = 0; 

// Pin Count 
int activePinCount = 0;

void setup(){
  // Set up serial monitor
  Serial.begin(9600);

  delay(15);
  pinMode(61, OUTPUT);
  digitalWrite(61, HIGH);

  // Set left motor pin modes
  pinMode(LEFT_DIR, OUTPUT); 
  pinMode(LEFT_PWM, OUTPUT); 
  pinMode(LEFT_SLP, OUTPUT);

  // Set right motor pin mode
  pinMode(RIGHT_DIR, OUTPUT);
  pinMode(RIGHT_PWM, OUTPUT);
  pinMode(RIGHT_PWM, OUTPUT);

  // LOW-Forward; HIGH-Reverse
  // HIGH Control; LOW-Outputs floating (coast)
  digitalWrite(LEFT_DIR, LOW);
  digitalWrite(LEFT_SLP, HIGH);
  digitalWrite(RIGHT_DIR, LOW);
  digitalWrite(RIGHT_SLP, HIGH);
}


void loop(){
  readSensors();

  // Do something based off the sensor reading
  previousError = error;
  error = calculateMotorCorrection(); 
  Serial.println(error);
  correctMotors(error);
}

//void calculateProportionalError(){
//  return 0;
//}
//
//void calculateDerivativeError(){
//  return 0;
//}


void readSensors(){
  /*
  How to read from sensors:
  ----------------------------------------------
  1. Set sensor channel pin to high
  2. After a time t_on, set channel pins inputs
  3. After a time t_wait, read value from sensor
     (Sensor value is digital 1 or 0)
  */

  // Set sensor pins to output, and activate
  for (int count = 0; count<8; count++) {
    pinMode(sensorPins[count], OUTPUT);
    digitalWrite(sensorPins[count], HIGH);
  }

  // Length of high output
    delayMicroseconds(t_on);

    // Set sensor pins to input 
  for (int count=0; count<8; count++) {
      pinMode(sensorPins[count], INPUT);
  }

  // Delay before reading input
  // Determined by findDelay(); function
  delayMicroseconds(t_wait);

  // Read sensor values and assign to global variables
  for (int count=0; count<8; count++){
    sensorRead[count] = digitalRead(sensorPins[count]);
    Serial.print(count);
    Serial.print(": ");
    Serial.print(sensorRead[count]); 
    Serial.print("   ");
  }
//  sensorRead[0] = 0;
//  sensorRead[7] = 0;
  Serial.println();

  //Delay for stability?
    delay(1);
}

float changeMapping(float input){
  float output = abs(input)/(2.5) * (150);
  return output; 
}

float invertedMapping(float input){
  float output = 2.5/abs(input) * (30);
  return output; 
}

float calculateMotorCorrection(){
  float error = 0;
  float sum = 0;
  activePinCount = 0;
  for (int count=0; count<8; count++){
    if (sensorRead[count] == 1){
      sum += sensorError[count];
      if (count != 3 && count != 4){
        activePinCount += 1; 
      }
    }
  }
  error = sum / activePinCount;
  return error; 
}

void correctMotors(float error){
//  errorSlope = (abs(error) - abs(previousError))/2;
  if (error > .5){
    leftPWM = changeMapping(error);
    rightPWM = invertedMapping(error); 
  } else if (error < -.5){
    leftPWM = invertedMapping(error);
    rightPWM = changeMapping(error);
  } else if (activePinCount == 6){
    leftPWM = 0;
    rightPWM = 0; 
  } else if (error == 0 && activePinCount != 0){
    leftPWM = 50;//changeMapping(error);
    rightPWM = 50;//changeMapping(error); 
  }
  
  Serial.print("Left PWM: ");
  Serial.print(leftPWM);
  Serial.println();
  Serial.print("Right PWM: ");
  Serial.print(rightPWM);
  Serial.println();
  analogWrite(LEFT_PWM, leftPWM);
  analogWrite(RIGHT_PWM, rightPWM);
  Serial.flush();
}




//void findDelay(){
//  /* Find minimum t_wait for sensor to 
//  distinguish between white and black */
//
//  int start_time = micros();
//  int end_time;
//
//  while (digitalRead(sensor1) == 1){
//    end_time = micros();
//  };
//
//  int time_elapsed = end_time - start_time;
//  Serial.println(time_elapsed);
//}
