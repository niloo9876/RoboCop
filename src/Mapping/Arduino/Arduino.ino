/*
  CPEN 291 - 2017 Winter Term 2
  Demo: April 4th, 2018
  Group 13 9:00am

  code for arduino on robot car
  
  members:
  - Erick
  - Ming
  - Nancy
  - Niloo
  - Timothy
  - Yiyi
*/
#define TESTING 0

// Radio control global varibles
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
RF24 radio(7, 8); // CE, CSN
const byte addresses[][6] = {"10001", "10002"};

// remote -> remote car data packet
typedef struct data_packet //Data Structure
{
  int controller_state = 0;
  bool sw_state;
};

// global data pack variable
struct data_packet payload_received;

// for left motor, LOW is forward, HIGH is backward
// for a4988 stepper motor driver
#define LEFT_FORWARD 0
#define LEFT_BACKWARD 1

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

// joystick position states
#define DONT_MOVE 0
#define MOVE_FORWARD 1
#define MOVE_BACKWARD 2
#define ROTATE_LEFT 3
#define ROTATE_RIGHT 4
//Defining 2 possible states
#define InputMode 0
#define RemoteMode 1

// math
#define PI 3.1415926536
int currentMode = InputMode;

// defines pins numbers

// stepper motor driver pins
const int left_step_pin = 6;
const int left_dir_pin = 5;
const int right_step_pin = 4;
const int right_dir_pin = 3;

// feedback pins
const int collision_pin = 12; // sense collision and sends to remote control
const int piezo_pin = 21; // if remote control sw is on, piezo goes off
const int headlightPins = 9; // when in mapping mode and moving (i.e. joystick not in center), 5 green led lights turn on

// whether the robot has collided with the wall
bool collision_state = 0;

// hardware specs
// all measurements in cm
const double wheel_diameter = 9.3;
const double wheel_circumference = wheel_diameter * PI; //29.2168
const double robot_motor_to_motor_distance = 17.7;
const double robot_motor_radius_of_turn = robot_motor_to_motor_distance / 2;

// delay variables
const int delay_normal = 5;
const int delay_slow = 10;

/*
    Global Variables required for main loops;
    Helps calculate the termination condition for each state.
*/
bool processed = 1;
bool isRunning = 0;
bool isSetup = 0;
// terminating counters:
int stepsToTake = 0;
// Tracking current number of steps;
int currentSteps = 0;
int trackedSteps = 0;

int wheelToTrack = 0;
#define fwdTrack 0
#define bckTrack 1
#define leftTurnTrack 2
#define rightTurnTrack 3

int pastWheelState = 0;

//
String command;




/*
  input: double [distance in centimeter]
  output: int [number of steps to take to travel the distance in cm]

  note:
  want to know how many times to turn the wheel to cover the requested distance
  200 step motor, so each revolution (turn) of the wheel is 200 steps
  degree turned per step = 1.8 degrees
  steps/revolution = 200 = 360 degrees / (1.8 degrees/step)
  distance/ circumference = number of revolutions
  number of steps = number of revolutions * steps/revolution
*/

int convert_cm_to_steps(double distance_in_cm) {
  return (int)( (distance_in_cm / wheel_circumference) * 200);
}

/*
  intput: degrees to turn the robot 
  output:
*/
int convert_degree_to_steps(double degrees_to_turn) {
  double distance_to_travel = degrees_to_turn * PI * robot_motor_radius_of_turn / 180;
  int steps_to_turn = convert_cm_to_steps(distance_to_travel);

  return steps_to_turn;
}

// Helper function to set both wheels to go forward
void go_forward() {
  digitalWrite(left_dir_pin, LEFT_FORWARD);
  digitalWrite(right_dir_pin, RIGHT_FORWARD);
}

// Helper function to set both wheels to go backwards
void go_backward() {
  digitalWrite(left_dir_pin, LEFT_BACKWARD);
  digitalWrite(right_dir_pin, RIGHT_BACKWARD);
}

// Helper function to set the wheels so the robot rotates right
void rotate_right_degrees() {
  digitalWrite(left_dir_pin, LEFT_FORWARD);
  digitalWrite(right_dir_pin, RIGHT_BACKWARD);
}

// Helper function to set the wheels so the robot rotates left
void rotate_left_degrees() {
  digitalWrite(left_dir_pin, LEFT_BACKWARD);
  digitalWrite(right_dir_pin, RIGHT_FORWARD);
}

// Helper function to get the robot to go a set amount
void step_many_times(int number_steps) {
  for (int i = 0; i < number_steps; i++) {
    step_once_both();
    delay(5);
  }
}

// Helper function to make the wheels go one step
// normal speed: delay 5ms
void step_once_both() {
  digitalWrite(right_step_pin, HIGH);
  digitalWrite(left_step_pin, HIGH);
  delayMicroseconds(500);
  digitalWrite(right_step_pin, LOW);
  digitalWrite(left_step_pin, LOW);
  delayMicroseconds(500);
  trackedSteps++;
  currentSteps++;
  delay(5);
}

// slow speed: delay 10ms
void step_once_both_slow() {
  digitalWrite(right_step_pin, HIGH);
  digitalWrite(left_step_pin, HIGH);
  delayMicroseconds(500);
  digitalWrite(right_step_pin, LOW);
  digitalWrite(left_step_pin, LOW);
  delayMicroseconds(500);
  trackedSteps++;
  currentSteps++;
  delay(delay_slow); // wait 10 milliseconds before triggering the next step
}


// Helper function for the remote to consistently turn left
void rotate_left() {
  rotate_left_degrees();
  step_once_both();
  delay(delay_slow);
}

// Helper function for the remote to consistently turn right
void rotate_right() {
  rotate_right_degrees();
  step_once_both();
  delay(delay_slow);
}

// Helper function for the remote to consistantly go foward
void move_forward() {
  go_forward();
  step_once_both();
  delay(delay_normal);
}

// Helper function for the remote to consistantly go backwards
void move_backward() {
  go_backward();
  step_once_both();
  delay(delay_normal);
}

// does nothing 
// when stepper motors are not trigger, it doesn't move
void dont_move() {
  ;
}


void testingPrintOuts() {
  if (TESTING) {
    String toTest = "";
    toTest += "Processing: " + String(processed);
    toTest += " stepsToTake: " + String(stepsToTake);
    toTest += " currentSteps: " + String(currentSteps);
    toTest += " wheelToTrack: " +  String(wheelToTrack);
    toTest += " currentMode: " + String(currentMode);
    Serial.println(toTest);
  }
}


// Helper function to calculate change in distance.
double calculateDistance() {
  double distance = ((double)trackedSteps) / 200 * wheel_circumference;
  if (wheelToTrack == fwdTrack) {
    return distance;
  } else if (wheelToTrack == bckTrack) {
    return (-1) * distance;
  }
  return 0.0;
}




// Helper function to calculate change in angle
double calculateAngle() {
  double distance = ((double)trackedSteps) / 200 * wheel_circumference;
  double angle = distance * 180 / (PI * robot_motor_radius_of_turn);

  if (wheelToTrack == leftTurnTrack) {
    return angle;
  } else if (wheelToTrack == rightTurnTrack) {
    return (-1) * angle;
  }
  return 0.0;
}

// function which sends the server the changes in distance and angle
void printChanges() { //
  String toPrint = "Distance: ";

  String distance = String(calculateDistance());
  String angle = String(calculateAngle());
  toPrint += distance + " Angle: " + angle;

  testingPrintOuts();
  //toPrint += " Angle: " + calculateAngle();
  Serial.println(toPrint);
  trackedSteps = 0;
}





// void function which the remote control utilizes
void remoteControl() {
  if (radio.available()) {
    radio.read(&payload_received, sizeof(payload_received));

    // if the joystick switch is HIGH, piezo buzzer sounds
    // acts as a honk
    if (payload_received.sw_state)
      tone(piezo_pin, 250);
    else
      noTone(piezo_pin);

    // if the robot collided with an object in the front
    collision_state = digitalRead(collision_pin);
    if (collision_state) {
      // two-way communication
      radio.stopListening(); // make the rf module writing mode
      radio.write(&collision_state, sizeof(collision_state)); // send the information
      collision_state = 0; // set control state to 0, essentiall pulses the peizo buzzer
      radio.write(&collision_state, sizeof(collision_state));
      radio.startListening(); // put the rf module back to reading mode
    }
  }
  int currentState = payload_received.controller_state;
  if (pastWheelState != currentState) {
    if (pastWheelState != 0) {
      printChanges();
    }
    pastWheelState = currentState;
  }

  // when joystick is not in center position
  if (payload_received.controller_state != 0) {
    // turn on the 5 green led headlights
    digitalWrite(headlightPins, HIGH);
  } else { // if in center position, the 5 green led headlights are off
    digitalWrite(headlightPins, LOW);
  }

  // Checks what direction to go based on controller signals.
  switch (payload_received.controller_state) {
    case DONT_MOVE:
      dont_move();
      break;
    case MOVE_FORWARD:
      wheelToTrack = fwdTrack;
      move_forward();
      break;
    case MOVE_BACKWARD:
      wheelToTrack = bckTrack;
      move_backward();
      break;
    case ROTATE_LEFT:
      wheelToTrack = leftTurnTrack;
      rotate_left();
      break;
    case ROTATE_RIGHT:
      wheelToTrack = rightTurnTrack;
      rotate_right();
      break;
    default:
      dont_move();
  }
}

/*
   input: 7 digit decimal number (int)
   output: nothing
   action: decode the command and move the robot
*/
void process_setup(String incoming_command) {
  /* 6 digit int
    most significant digit is movement type
    one of: (1) move straight (2) rotate (3) stop (4) turn
    second most significant digit is direction of the movment
    one of: (0) left or backward (1) right or forward
  */
  int mode = incoming_command.substring(0, 1).toInt();
  int command_tag = incoming_command.substring(1, 3).toInt();
  int data_value = incoming_command.substring(3, 7).toInt();
  int movement_type = command_tag / 10;
  int direction_of_movement = command_tag % 10;

  if (TESTING) {
    Serial.println(mode);
    Serial.println(command_tag);
    Serial.println(movement_type);
    Serial.println(direction_of_movement);
    Serial.println(data_value);
  }

  //Sets up the current Mode
  currentMode = mode;
  if (mode == InputMode) {
    currentSteps = 0;
    trackedSteps = 0;
    // Resets currentSteps and initializes new Steps needed;
    // Switch case which initializes the modes of the motor and steps needed;
    switch (movement_type) {
      case MOVE_STRAIGHT:
        stepsToTake = convert_cm_to_steps(data_value);

        if (direction_of_movement == FORWARD) {
          go_forward();
          wheelToTrack = fwdTrack;
        }
        else if (direction_of_movement == BACKWARD) {
          go_backward();
          wheelToTrack = bckTrack;
        }
        break;

      case ROTATE:
        stepsToTake = convert_degree_to_steps(data_value);
        if (direction_of_movement == RIGHT) {
          rotate_right_degrees();
          wheelToTrack = rightTurnTrack;
        }
        else if (direction_of_movement == LEFT) {
          rotate_left_degrees();
          wheelToTrack = leftTurnTrack;
        }
        break;
      case STOP:
        dont_move();
        return;
      // shouldn't be reached
      default:
        //nothing to default
        dont_move();
        break;
    }
  }
}

void setup() {
  // stepper motor
  // 2 pins to arduino per stepper motor
  pinMode(left_step_pin, OUTPUT);
  pinMode(left_dir_pin, OUTPUT);
  pinMode(right_step_pin, OUTPUT);
  pinMode(right_dir_pin, OUTPUT);

  Serial.begin(9600);

  pinMode(collision_pin, INPUT);
  pinMode(headlightPins, OUTPUT);

  // radio set up
  radio.begin();
  radio.openWritingPipe(addresses[0]); // 10002
  radio.openReadingPipe(1, addresses[1]); // 10001
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}



void loop() {
  // if there is no commands yet
  if (Serial.available() > 0) {
    command = Serial.readString();
    // Checks if interrupting
    if (isRunning == 1) {
      printChanges();
    }
    processed = 0;
    isSetup = 0;
  } else {
    // If not setup yet; setup all global variables required
    if (!isSetup) {
      process_setup(command);
      isSetup = 1;
      isRunning = 1;
      if (currentMode == RemoteMode) {
        processed = 1;
      }
    }
    // Checks if something needs to be processed
    // Should be in InputMode
    if (!processed && currentMode == InputMode) {
      // Since it knows if it's turning or moving a certain distance;
      // we just need to keep stepping until the goal is reached.
      if (currentSteps == stepsToTake) {
        printChanges();
        Serial.println("done");
        processed = 1;
        isRunning = 0;
      } else {
        //Checks if it's apx 5cm; if so, print an update.
        if (wheelToTrack == fwdTrack || wheelToTrack == bckTrack) {
          if (trackedSteps >= 35) {
            printChanges();
          }
          step_once_both();
        } else {
          step_once_both_slow();
        }
      }
      //Else if it is in mapping/remote mode.
    } else if (currentMode == RemoteMode) {
      remoteControl();
    }
  }
}
