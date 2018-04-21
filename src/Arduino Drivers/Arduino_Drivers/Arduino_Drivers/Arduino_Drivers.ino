/**
 * Prototyping Arduino final Implementation of hardware control;
 * Idea: Different states to control the robot while constantly
 * outputting sensor data.
 */


 /*
  * testing thus far; seems to think right is slower or left is faster. 
  * Regardless, it seems to make the robot turn left because it speeds up the right wheel too much.
  */

//Motor Pins
 #define MOTOR_M2 7
 #define MOTOR_E2 6
 #define MOTOR_E1 5
 #define MOTOR_M1 4

 //wheel encoder pins
 #define leftWheelEnc 3
 #define rightWheelEnc 2

//ticks which represents how much the wheels has turned by.
volatile int leftTickCount = 0;
volatile int rightTickCount = 0;
int pastLeftTicks[5] = {0, 0, 0, 0, 0};
int pastRightTicks[5]= {0, 0, 0, 0, 0};
int leftArrayPos = 0;
int rightArrayPos = 0;

/**
 * Constants for Wheel Controls
 * implementing a master/slave wheel power relationship;
 * slave wheel will attempt to self-correct while master maintains
 * the same power.
 */
double masterWheelPower = 150; //Left Wheel speed
double slaveWheelPower = 150;  //Right Wheel speed

int lastRightTick = 0;
int lastLeftTick = 0;

// Difference between left and right wheel; used for correction
double error = 0;

// correction magic number;
double const correctionConstant = 1;

//circumference of the wheel; avg of measured wheel & 2piR
const long circumference = 20.4;

void setup() {
    Serial.begin(9600);

    // initialize motor pins
    pinMode(MOTOR_M1, OUTPUT);
    pinMode(MOTOR_M2, OUTPUT);
    pinMode(MOTOR_E1, OUTPUT);
    pinMode(MOTOR_E2, OUTPUT);

    // initalizes encoder pins.
    pinMode(leftWheelEnc, INPUT_PULLUP);
    pinMode(rightWheelEnc, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(leftWheelEnc), leftTick, FALLING);
    attachInterrupt(digitalPinToInterrupt(rightWheelEnc), rightTick, FALLING);
}

void loop() {
    
    writeToMotor(HIGH, HIGH, (int)masterWheelPower, (int)slaveWheelPower);
    //writeToMotor(HIGH, HIGH, 200, 200);
    
    int leftTicks = deltaLeftTicks();
    int rightTicks = deltaRightTicks();
    Serial.print(leftTicks);
    Serial.print("  ");
    Serial.print(rightTicks);
    Serial.print("  ");
    Serial.print(masterWheelPower);
    Serial.print("  ");
    Serial.print(slaveWheelPower);
    Serial.print("  ");
    error = leftTicks - rightTicks;
    
    
    //error = deltaLeftTicks() - deltaRightTicks();
    if(slaveWheelPower <= 240 && slaveWheelPower >= 10){
      slaveWheelPower += (error / correctionConstant);
    } else if (slaveWheelPower > 240){
      slaveWheelPower = 239;
    } else if (slaveWheelPower < 10){
      slaveWheelPower = 11;
    }
    

    Serial.println(error / correctionConstant);
    delay(200);
    
}







/**
 * Used to return how much ticks the wheel has turned by
 * @return the change in Ticks for the specific wheel
 */
int deltaLeftTicks(){

    int LtickCount = leftTickCount;
    int retVal = LtickCount - lastLeftTick;
    lastLeftTick = LtickCount;
    return retVal;
  /*
    int avgLeftTick = 0;
    
    pastLeftTicks[leftArrayPos%5] = leftTickCount;
    leftTickCount = 0;
    leftArrayPos++;
    if(leftArrayPos == 5){
      leftArrayPos = 0;
    }
    for(int i = 0; i<5; i++){
      avgLeftTick += pastLeftTicks[i];
    }
    
    
    return avgLeftTick/5;
    */
}

int deltaRightTicks(){
    
    int RtickCount = rightTickCount;
    int retVal = RtickCount - lastRightTick;
    lastRightTick = RtickCount;
    return retVal;
    
  /*
    int avgRightTick = 0;
    
    pastRightTicks[rightArrayPos%5] = rightTickCount;
    rightTickCount = 0;
    rightArrayPos++;
    if(rightArrayPos == 5){
      rightArrayPos = 0;
    }
    for(int i = 0; i<5; i++){
      avgRightTick += pastRightTicks[i];
    }
    
    
    return avgRightTick/5;
  */
}

/*
    Interrupt function which increments the left or right wheel tick.
    Each tick represents an amount of wheel turn.
    1/20th of a wheel turn.
 */
void leftTick() {
    leftTickCount++;
    
    //Serial.print("left ");
    //Serial.println(leftTickCount);
    
}

void rightTick() {
    rightTickCount++;
    
    //Serial.print("right ");
    //Serial.println(rightTickCount);
    
}


/*
    Copied from Project 1
   Directly interacts with the motors
   Lowest level of code in terms of abstraction
   Motor 2 (M2) is left wheel
   Motor 1 (M1) is right wheel
*/
void writeToMotor(int M2_dir, int M1_dir, int M2_speed, int M1_speed) {
  /* function invariant
     M1 and M2 must always be between  0 and 255
  */
  digitalWrite  (MOTOR_M1, M1_dir);
  digitalWrite  (MOTOR_M2, M2_dir);
  analogWrite   (MOTOR_E1, M1_speed);   //PWM Speed Control
  analogWrite   (MOTOR_E2, M2_speed);   //PWM Speed Control
}
