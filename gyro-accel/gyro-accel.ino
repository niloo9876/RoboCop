#include <SparkFunLSM9DS1.h>
#include <SPI.h>
#include <Wire.h>
#include <math.h>


float previuseAccelX=0,previuseAccelY=0,previuseAccelZ=0;
double previuseTime;
double prevVelocityX=0,prevVelocityY=0,prevVelocityZ=0;
float prevCoordinateX=0,prevCoordinateY=0,prevCoordinateZ=0;
int i=1;
#define PI 3.14159265

//MY Additionals
/* This driver uses the Adafruit unified sensor library (Adafruit_Sensor),
   which provides a common 'type' for sensor data and some helper functions.
   
   To use this driver you will also need to download the Adafruit_Sensor
   library and include it in your libraries folder.

   You should also assign a unique ID to this sensor for use with
   the Adafruit Sensor API so that you can identify this particular
   sensor in any data logs, etc.  To assign a unique ID, simply
   provide an appropriate value in the constructor below (12345
   is used by default in this example).
   
   Connections (For default I2C)
   ===========
   Connect SCL to analog 5
   Connect SDA to analog 4
   Connect VDD to 5V DC
   Connect GROUND to common ground

   History
   =======
   2014/JULY/25  - First version (KTOWN)
*/
   
/* Assign a unique base ID for this sensor */   
LSM9DS1 lsm;  // Use I2C, ID #1000


/* Or, use Hardware SPI:
  SCK -> SPI CLK
  SDA -> SPI MOSI
  G_SDO + XM_SDO -> tied together to SPI MISO
  then select any two pins for the two CS lines:
*/

#define LSM9DS0_XM_CS 10
#define LSM9DS0_GYRO_CS 9

#define LSM9DS0_SCLK 13
#define LSM9DS0_MISO 12
#define LSM9DS0_MOSI 11

void configureSensor(void)
{
  // 1.) Set the accelerometer range
  lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_2G);
  //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_4G);
  //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_6G);
  //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_8G);
  //lsm.setupAccel(lsm.LSM9DS0_ACCELRANGE_16G);
  
  // 2.) Set the magnetometer sensitivity
  //lsm.setupMag(lsm.LSM9DS0_MAGGAIN_2GAUSS);
  //lsm.setupMag(lsm.LSM9DS0_MAGGAIN_4GAUSS);
  lsm.setupMag(lsm.LSM9DS1_MAGGAIN_8GAUSS);
  //lsm.setupMag(lsm.LSM9DS0_MAGGAIN_12GAUSS);

  // 3.) Setup the gyroscope
  //lsm.setupGyro(lsm.LSM9DS0_GYROSCALE_245DPS);
  lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_500DPS);
}

void setup(void) 
{
#ifndef ESP8266
  while (!Serial);     // will pause Zero, Leonardo, etc until serial console opens
#endif
  Serial.begin(250000);
  Serial.println(F("LSM9DS0 9DOF Sensor Test")); Serial.println("");
  
  /* Initialise the sensor */
  if(!lsm.begin())
  {
    /* There was a problem detecting the LSM9DS0 ... check your connections */
    Serial.print(F("Ooops, no LSM9DS0 detected ... Check your wiring or I2C ADDR!"));
    while(1);
  } 
  /* Setup the sensor gain and integration time */
  configureSensor();
  
  /* We're ready to go! */
  Serial.println("");

  /*restart millis timer*/
}

/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/
void loop(void) 
{  
  /* Get a new sensor event */
  float AccelX, AccelY,AccelZ;
  float DegreesX,DegreesY,DegreesZ;
  double CoordinateX,CoordinateY,CoordinateZ;
  sensors_event_t accel, mag, gyro, temp;
  lsm.getEvent(&accel, &mag, &gyro, &temp); 
  convertAcceleration (&AccelX,&AccelY,&AccelZ,accel.acceleration.x,accel.acceleration.y,accel.acceleration.z); 
  DegreesX=convertMagentic (mag.magnetic.x,accel.acceleration.x,AccelX);
  DegreesY=convertMagentic (mag.magnetic.y,accel.acceleration.y,AccelY);
  DegreesZ=convertMagentic (mag.magnetic.z,accel.acceleration.z,AccelZ);
  correctDegrees(&DegreesX,&DegreesY,&DegreesZ);

  
  // print out accelleration data
  Serial.print("Accel X: "); Serial.print(AccelX); Serial.print(" ");
  Serial.print("  \tY: "); Serial.print(AccelY);       Serial.print(" ");
  Serial.print("  \tZ: "); Serial.print(AccelZ);     Serial.println("  \tm/s^2");
  
  // print out Degrees data
  Serial.print("Degrees X: "); Serial.print(DegreesX); Serial.print(" ");
  Serial.print("  \tY: "); Serial.print(DegreesY);       Serial.print(" ");
  Serial.print("  \tZ: "); Serial.print(DegreesZ);     Serial.println("  \tdegrees");
  getCoordinate(&CoordinateX,&CoordinateY,&CoordinateZ,AccelX,AccelY,AccelZ);
  //print out Coordinates data
  Serial.print("Coordinate X: "); Serial.print(CoordinateX); Serial.print(" ");
  Serial.print("  \tY: "); Serial.print(CoordinateY);       Serial.print(" ");
  Serial.print("  \tZ: "); Serial.print(CoordinateZ);     Serial.println("  \tmeter");


  Serial.println("**********************\n");
}


void fixAccelaration(float *currAccel){
  if ((*currAccel>-0.07)&&(*currAccel<0.07))
    *currAccel=0;
}
void convertAcceleration(float *AccelX,float *AccelY,float *AccelZ,float currAccelX,float currAccelY,float currAccelZ){
 
  while (i){
    previuseAccelX=currAccelX;
    previuseAccelY=currAccelY;
    previuseAccelZ=currAccelZ;
    i=0;
  }
  *AccelX= currAccelX-previuseAccelX;
  previuseAccelX=currAccelX;
  *AccelY= currAccelY-previuseAccelY;
  previuseAccelY=currAccelY;
  *AccelZ= currAccelZ-previuseAccelZ;
  previuseAccelZ=currAccelZ;
  fixAccelaration(AccelX);  fixAccelaration(AccelY);  fixAccelaration(AccelZ);
}

float convertMagentic(float currMagnet,float currAccelVector,float Accel){
  float param;
  param=(currAccelVector-Accel)/9.8;
  currMagnet= acos(param)*180.0/PI;
  if (isnan(currMagnet)){
    if (param<0)
      currMagnet=180;
    else
      currMagnet=0;
  }
  return currMagnet;
}


void correctDegrees (float *DegreesX,float *DegreesY,float *DegreesZ){
  float uncorrDegreesX, uncorrDegreesY, uncorrDegreesZ;
  uncorrDegreesX=*DegreesX;
  uncorrDegreesY=*DegreesY;
  uncorrDegreesZ=*DegreesZ;
  if ((uncorrDegreesY>=90)&&(uncorrDegreesY<=180))
    *DegreesZ=360-uncorrDegreesZ;
  if ((uncorrDegreesX>=90)&&(uncorrDegreesX<=180))
    *DegreesY=360-uncorrDegreesY;
  if ((uncorrDegreesY>=90)&&(uncorrDegreesY<=180))
    *DegreesX=360-uncorrDegreesX;
}

void getCoordinate(double *currCoordinateX,double *currCoordinateY,double *currCoordinateZ,float AccelX,float AccelY,float AccelZ){
  double currentTime;
  currentTime=(millis()-previuseTime)/1000;
  previuseTime=millis();
  *currCoordinateX=prevCoordinateX+(prevVelocityX*currentTime)+(AccelX*currentTime*currentTime*0.5);
  prevVelocityX=prevVelocityX+(AccelX*currentTime);
  prevCoordinateX=*currCoordinateX;
  *currCoordinateY=prevCoordinateY+(prevVelocityY*currentTime)+(AccelY*currentTime*currentTime*0.5);
  prevVelocityY=prevVelocityY+(AccelY*currentTime);
  prevCoordinateY=*currCoordinateY;
  //Calculate Z coodinate
  *currCoordinateZ=prevCoordinateZ+(prevVelocityZ*currentTime)+(AccelZ*currentTime*currentTime*0.5);
  prevVelocityZ=prevVelocityZ+(AccelZ*currentTime);
  prevCoordinateZ=*currCoordinateZ;
}
