#define MOTOR_M2 7
#define MOTOR_E2 6
#define MOTOR_E1 5
#define MOTOR_M1 4

#define hall_r 18
#define hall_l 19

//old code
double Dl, Dr, dt, y_vl,  y_vr;
volatile unsigned int left_tick, right_tick;
unsigned int prev_l_tick, prev_r_tick;
unsigned long prev_time = 0;

//new variables
int currLeftSpeed = 255;
int currRightSpeed = 255;

void setup() {
  Serial.begin(9600);
  // initialize motor pins
  pinMode(MOTOR_M1, OUTPUT);
  pinMode(MOTOR_M2, OUTPUT);
  pinMode(MOTOR_E1, OUTPUT);
  pinMode(MOTOR_E2, OUTPUT);
  // Debug purpose:
  
  // put your setup code here, to run once:
  pinMode(hall_l, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(hall_l), CalcL, CHANGE);
  pinMode(hall_r, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(hall_r), CalcR, CHANGE);
}

void loop() {
  // put your main code here, to run repeatedly:
    goStragiht2(220, 220);
}


void writeToMotor(int M2_dir, int M1_dir, int M2_speed, int M1_speed) {
  /* function invariant
     M1 and M2 must always be between  0 and 255
  */
  digitalWrite  (MOTOR_M1, M1_dir);
  digitalWrite  (MOTOR_M2, M2_dir);
  analogWrite   (MOTOR_E1, M1_speed);   //PWM Speed Control
  analogWrite   (MOTOR_E2, M2_speed);   //PWM Speed Control
}

int left_D() {
  int delta_left_tick = left_tick - prev_l_tick;
  prev_l_tick = left_tick;
  return delta_left_tick; // cm

}
int right_D() {
  int delta_right_tick = right_tick - prev_r_tick;
  prev_r_tick = right_tick;
  return delta_right_tick; // cm
}

void CalcL() {
  left_tick++;
  Serial.print("left t");
  Serial.println(left_tick);
}

void CalcR() {
  right_tick++;
  Serial.print("right t");
  Serial.println(right_tick);
}

void goStraightProto(int maxSpeed){
  int delta_right_tick = right_tick - prev_r_tick;
  prev_r_tick = right_tick;

  int delta_left_tick = left_tick - prev_l_tick;
  prev_l_tick = left_tick;

  
  if(delta_right_tick - delta_left_tick > 2){
    
  } else if(delta_right_tick - delta_left_tick < 2){
    
  }
  writeToMotor(HIGH, HIGH, ,);
  
  
}


void goStragiht2(int speedR, int speedL){

  int L = left_D();
  int R = right_D();

  writeToMotor(HIGH,HIGH, speedR, speedL);

  while(true){
    if(L > R){
       speedL--;
      } 
    if(R > L){
      speedL++;
      }
    writeToMotor(HIGH,HIGH, speedR, speedL);
     delay(50);
    L = left_D();
    R = right_D();
    
   
   }
  }
