// debug
#define TESTING 5

#define TRUE 1
#define FALSE 0

// for left motor, HIGH is forward, LOW is backward
#define LEFT_FORWARD 0
#define LEFT_BACKWARD 1
#define PI 3.1415926536
// for right motor, LOW is forward, HIGH is backward
#define RIGHT_FORWARD 0
#define RIGHT_BACKWARD 1

// movement types
#define MOVE_STRAIGHT 1
#define ROTATE  2
#define STOP    3
#define TURN    4

// direction of movement type
#define FORWARD 1
#define BACKWARD 0
#define RIGHT 1
#define LEFT 0


// defines pins numbers
const int left_step_pin = 6;
const int left_dir_pin = 5;
const int right_step_pin = 4;
const int right_dir_pin = 3;

// hardware specs
// all measurements in cm
const double wheel_diameter = 9.3;
const double wheel_circumference = wheel_diameter * PI; //29.2168
const double robot_motor_to_motor_distance = 17.7;
const double robot_motor_radius_of_turn = robot_motor_to_motor_distance / 2;

String command;
void setup() {
  // stepper motor
  // 2 pins to arduino per stepper motor
  pinMode(left_step_pin, OUTPUT);
  pinMode(left_dir_pin, OUTPUT);
  pinMode(right_step_pin, OUTPUT);
  pinMode(right_dir_pin, OUTPUT);

  Serial.begin(9600);
}
void loop() {
  if (TESTING == 1){
    go_forward(100);
    delay(5000);
  }
  else if (TESTING == 2) {
    rotate_right_degrees(90);
    delay(3000);
  }
  else if(TESTING == 3){
    rotate_left_degrees(90);
    delay(3000);
  }
  else if(TESTING == 4){
    delay(1000);

    
    go_forward(100);
    delay(5);
    rotate_left_degrees(90);
    delay(5);
    go_forward(50);
    delay(5);
    rotate_left_degrees(90);
    delay(5);
    go_forward(100);
    delay(5);
    rotate_left_degrees(90);
    delay(5);
    go_forward(50);
    
  }
  else if (TESTING == 5){
    Serial.println(convert_cm_to_steps(5));
  }
  
  else {
    if (Serial.available() > 0) {
      command = Serial.readString();
      process_command(command);
    }

  }
}
/*
   input: 6 digit decimal number (int)
   output: nothing
   action: decode the command and move the robot
*/
void process_command(String incoming_command) {
  /* 6 digit int
    most significant digit is movement type
    one of: (1) move straight (2) rotate (3) stop (4) turn
    second most significant digit is direction of the movment
    one of: (0) left or backward (1) right or forward
  */
  int command_tag = incoming_command.substring(0, 2).toInt();
  int data_value = incoming_command.substring(2, 6).toInt();
  int movement_type = command_tag / 10;
  int direction_of_movement = command_tag % 10;

  switch (movement_type) {
    case MOVE_STRAIGHT:
      if (direction_of_movement == FORWARD)
        go_forward(data_value);
      else if (direction_of_movement == BACKWARD)
        go_backward(data_value);
      break;
    case ROTATE:
      if (direction_of_movement == RIGHT)
        rotate_right_degrees(data_value);
      else if (direction_of_movement == LEFT)
        rotate_left_degrees(data_value);
      break;
    case STOP:
      dont_move();
      break;
    /*case TURN:
      if (direction_of_movement == RIGHT)
        turn_right_degrees(data_value);
      else if (direction_of_movement == LEFT)
        turn_left(data_value);*/
    default:
      dont_move();
      break;
  }
  Serial.println("done");
}


/* want to know how many times to turn the wheel to cover the requested distance
  200 step motor, so each revolution (turn) of the wheel is 200 steps
  degree turned per step = 1.8 degrees
  steps/revolution = 200 = 360 degrees / (1.8 degrees/step)
  distance/ circumference = number of revolutions
  number of steps = number of revolutions * steps/revolution
*/
 
int convert_cm_to_steps(double distance_in_cm) {
  Serial.println((int)( (distance_in_cm / wheel_circumference) * 200));
  return (int)( (distance_in_cm / wheel_circumference) * 200);
}

