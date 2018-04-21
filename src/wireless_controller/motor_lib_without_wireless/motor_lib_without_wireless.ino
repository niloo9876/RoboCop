/*     Simple Stepper Motor Control Exaple Code
 *      
 *  by Dejan Nedelkovski, www.HowToMechatronics.com
 *  
 */
// for left motor, HIGH is forward, LOW is backward
#define LEFT_FORWARD 1
#define LEFT_BACKWARD 0

// for right motor, LOW is forward, HIGH is backward
#define RIGHT_FORWARD 0
#define RIGHT_BACKWARD 1
// defines pins numbers
const int left_step_pin = 4; 
const int left_dir_pin = 3; 
const int right_step_pin = 6; 
const int right_dir_pin = 5; 

int turn_90_degree_steps = 200;
int quarter_wheel_step_number = 50;
int whole_wheel_step_number = 200;

void setup() {
  pinMode(left_step_pin,OUTPUT);
  pinMode(left_dir_pin,OUTPUT);
  //pinMode(4, OUTPUT);
  pinMode(right_step_pin,OUTPUT); 
  //pinMode(right_dir_pin,OUTPUT);
  
  digitalWrite(left_dir_pin, LOW);// LOW is forward, HIGH is backward
  digitalWrite(right_dir_pin, HIGH);// HIGH is forward, LOW is backward
}
void loop() {
  //step_once_both();
  //forward_quarter_wheel();
  go_straight();
  /*
  turn_right_90();
  turn_right_90();
  turn_right_90();
  turn_right_90();*/
  delay(500); // One second delay
  
}

void turn_right_45(){
  
}

void turn_left_45(){
  
}


void turn_left_90(){
  
}
void go_straight(){
  digitalWrite(left_dir_pin, LEFT_FORWARD); 
  digitalWrite(right_dir_pin, RIGHT_FORWARD);
  for (int i = 0; i< 100000; i++){
    step_once_both();
    delay(5);
  }
}

void forward_quarter_wheel(){
  
  digitalWrite(left_dir_pin, LEFT_FORWARD); 
  digitalWrite(right_dir_pin, RIGHT_FORWARD);
  step_many_times(200);
}
void turn_right_90(){
  digitalWrite(left_dir_pin, LEFT_FORWARD);
  digitalWrite(right_dir_pin, RIGHT_BACKWARD);
  step_many_times(turn_90_degree_steps);
}
void step_many_times(int number_steps){
  for (int i=0; i< number_steps; i++){
    step_once_both();
    delay(5);
  }
}
void step_once_both(){
    digitalWrite(right_step_pin,HIGH); 
    digitalWrite(left_step_pin,HIGH); 
    delayMicroseconds(700); 
    digitalWrite(right_step_pin,LOW); 
    digitalWrite(left_step_pin,LOW); 
    delayMicroseconds(700);
}

