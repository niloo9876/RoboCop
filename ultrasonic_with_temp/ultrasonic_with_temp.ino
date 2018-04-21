const int temp_pin = A0;
const int echo_pin = 12;
const int trig_pin = 13;

void setup() {
  
pinMode(trig_pin, OUTPUT); // Sets the trigPin as an Output
pinMode(echo_pin, INPUT); // Sets the echoPin as an Input
 Serial.begin(9600);
}

void loop() {
  
  //Serial.print(getTemp());
  //Serial.println(" degrees C");
  Serial.print(getAvgDist());
  Serial.println(" cm");
  delay(500);
}
double getTemp(){
  return analogRead(temp_pin)/1024.0*5000/10;
}
double getAvgDist(){
  int x = 10;
  double sum=0;
  for (int i=0; i<10;i++){
    sum+=getSensorDistanceCM();
    delay(50);
  }
  return sum/10;
}
double getSensorDistanceCM() {

  double duration, distance;
  // required initialization every time want to measure distance using ultrasonic sensor
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  // Sets the trig_pin on HIGH state for 10 micro seconds
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig_pin, LOW);
  // initialization ends

  duration = pulseIn(echo_pin, HIGH); // Reads the echo_pin, returns the sound wave travel time in microseconds
  // explanation for magic numbers
  // - 1024:  analog read has value between 0 and 1024
  // - 5000:  map this value to between 0V and 5V (hence 5000 mV)
  // - 10:    divide by 10 to become celcius, or another number to convert to fahrenheit
  double temp = getTemp();
  // speed of sound in meter/second
  double speed_of_sound = 311.5 + 0.6 * temp; 
  //Serial.println(speed_of_sound);
  // convert meter/second to centimeter/second
  // 10^6 microseconds in a second
  // 100 cm in a meter
  speed_of_sound = 1 / speed_of_sound / 100 * 1000000; 
  //Serial.print("Speed of sound is: ");
  //Serial.print(speed_of_sound);
  //Serial.println(" cm/s");
  distance = duration / speed_of_sound ;
  //Serial.print("duration is: ");
  //Serial.println(duration);
  //Serial.println(distance);
  distance /= 2;
  //Serial.println(distance);
  return distance;
}


