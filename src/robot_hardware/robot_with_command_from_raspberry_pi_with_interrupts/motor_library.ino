void go_forward(int distance_in_cm) {


  int total_steps = convert_cm_to_steps(distance_in_cm);
  digitalWrite(left_dir_pin, LEFT_FORWARD);
  digitalWrite(right_dir_pin, RIGHT_FORWARD);
  for (int i = 0; i < total_steps; i++) {
    if (after_isr)
      break;
    step_once_both();
    delay(5);
  }
}
void turn_left() {
  digitalWrite(right_dir_pin, RIGHT_FORWARD);
  step_single_right();
  delay(5);
}

void turn_right() {
  digitalWrite(left_dir_pin, LEFT_FORWARD);
  step_single_left();
  delay(5);
}
void rotate_left() {
  digitalWrite(left_dir_pin, LEFT_BACKWARD);
  digitalWrite(right_dir_pin, RIGHT_FORWARD);
  step_once_both();
  delay(5);
}

void rotate_right_degrees(int degrees_to_turn) {
  digitalWrite(left_dir_pin, LEFT_FORWARD);
  digitalWrite(right_dir_pin, RIGHT_BACKWARD);
  int steps_to_turn = degrees_to_turn * PI * robot_motor_radius_of_turn / 180;
  step_many_times(steps_to_turn);
}
void rotate_right() {
  digitalWrite(left_dir_pin, LEFT_FORWARD);
  digitalWrite(right_dir_pin, RIGHT_BACKWARD);
  step_once_both();
  delay(5);
}

void rotate_left_degrees(int degrees_to_turn) {
  digitalWrite(left_dir_pin, LEFT_BACKWARD);
  digitalWrite(right_dir_pin, RIGHT_FORWARD);
  int steps_to_turn = degrees_to_turn * PI * robot_motor_radius_of_turn / 180;
  step_many_times(steps_to_turn);
}

void rotate_right_90() {
  digitalWrite(left_dir_pin, LEFT_FORWARD);
  digitalWrite(right_dir_pin, RIGHT_BACKWARD);
  step_many_times(turn_90_degree_steps);
}

void step_many_times(int number_steps) {
  for (int i = 0; i < number_steps; i++) {
    if (after_isr)
      break;
    step_once_both();
    delay(5);
  }
}

void go_forward() {
  digitalWrite(left_dir_pin, LEFT_FORWARD);
  digitalWrite(right_dir_pin, RIGHT_FORWARD);
  step_once_both();
  delay(5);
}

void go_backward() {
  digitalWrite(left_dir_pin, LEFT_BACKWARD);
  digitalWrite(right_dir_pin, RIGHT_BACKWARD);
  step_once_both();
  delay(5);
}

void step_once_both() {
  digitalWrite(right_step_pin, HIGH);
  digitalWrite(left_step_pin, HIGH);
  delayMicroseconds(500);
  digitalWrite(right_step_pin, LOW);
  digitalWrite(left_step_pin, LOW);
  delayMicroseconds(500);
}

void step_single_left() {
  digitalWrite(left_step_pin, HIGH);
  delayMicroseconds(500);
  digitalWrite(left_step_pin, LOW);
  delayMicroseconds(500);
}

void step_single_right() {
  digitalWrite(right_step_pin, HIGH);
  delayMicroseconds(500);
  digitalWrite(right_step_pin, LOW);
  delayMicroseconds(500);
}
void dont_move() {
  ;
}
