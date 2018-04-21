const int leftStep = 5;
const int leftDir = 4;
const int rightStep = 7;
const int rightDir = 6;

const int backward = 1;
const int forward = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(leftStep, OUTPUT);
  pinMode(leftDir, OUTPUT);
  pinMode(rightStep, OUTPUT);
  pinMode(rightDir, OUTPUT);
}

void loop() {
  //LOW=forward HIGH=backward
  digitalWrite(leftDir, backward);
  digitalWrite(rightDir, forward); 
  for(int x = 0; x < 2000; x++) {
    stepOnce();
    delay(5);
  }

  delay(2000);
}

void runStraight(int dir, int cycles){
  if(dir == forward){
    digitalWrite(leftDir, forward);
    digitalWrite(rightDir, backward);  
  }
  if(dir == backward){
    digitalWrite(leftDir, backward);
    digitalWrite(rightDir, forward); 
  }

  for(int i = 0; i<cycles; i++){
    stepOnce();  
   }
}

void turnRight(){
  digitalWrite(leftDir, forward);
  digitalWrite(rightDir, forward);
  for(int i = 0; i<50; i++){
    stepOnce();  
   }
  }

void turnLeft(){
  digitalWrite(leftDir, backward);
  digitalWrite(rightDir, backward);
  for(int i = 0; i<50; i++){
    stepOnce();  
   }
  }

void stepOnce(){
    digitalWrite(rightStep,HIGH); 
    digitalWrite(leftStep,HIGH); 
    delayMicroseconds(500);
    
    digitalWrite(rightStep,LOW); 
    digitalWrite(leftStep,LOW); 
    delayMicroseconds(500); 
}
  
