void go_forward(int distance_in_cm) {
  int total_steps = convert_cm_to_steps(distance_in_cm);
  digitalWrite(left_dir_pin, LEFT_FORWARD);
  digitalWrite(right_dir_pin, LEFT_FORWARD);
  for (int i = 0; i < total_steps; i++) {
    step_once_both();
    delay(5);
  }
}
void go_backward(int distance_in_cm) {
  int total_steps = convert_cm_to_steps(distance_in_cm);
  digitalWrite(left_dir_pin, LEFT_BACKWARD);
  digitalWrite(right_dir_pin, RIGHT_BACKWARD);
  for (int i = 0; i < total_steps; i++) {
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
  if (degrees_to_turn <= 0)
    degrees_to_turn = 90;

  double distance_to_travel = degrees_to_turn * PI * robot_motor_radius_of_turn / 180;
  Serial.println(distance_to_travel);
  int steps_to_turn = convert_cm_to_steps(distance_to_travel);
  Serial.println(steps_to_turn);
  step_many_times_slow(steps_to_turn);
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
  if (degrees_to_turn <= 0)
    degrees_to_turn = 90;
  double distance_to_travel = degrees_to_turn * PI * robot_motor_radius_of_turn / 180;
  int steps_to_turn = convert_cm_to_steps(distance_to_travel);
  Serial.println(steps_to_turn);
  step_many_times_slow(steps_to_turn);
}

void rotate_left_90() {
  rotate_left_degrees(90);
}

void rotate_right_90() {
  rotate_right_degrees(90);
}

void step_many_times(int number_steps) {
  for (int i = 0; i < number_steps; i++) {
    step_once_both();
    delay(5);
  }
}

void step_many_times_slow(int number_steps) {
  for (int i = 0; i < number_steps; i++) {
    step_once_both();
    delay(10);
  }
}

void go_forward_one_step() {
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
