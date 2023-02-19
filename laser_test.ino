#include <Servo.h>

int x_pos = 0;
const int x_serv_port = 9;
Servo x_servo; 
int y_pos = 0;
const int y_serv_port = 10;
Servo y_servo; 
int z_pos = 0;
 
const int trigPin = 12;
const int echoPin = 11;
const int laserPin = 2;
  
void setup() {
 
  x_servo.attach(x_serv_port);
  y_servo.attach(y_serv_port);
  pinMode(laserPin, OUTPUT);
  Serial.begin(9600);
  Serial1.begin(34800);
 
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  x_servo.write(0); // this will most likely need to be updated
  y_servo.write(0);
}
 
void loop() {
    // get z distance and wait until zero.
    float distance = getCmDistance();
    if (distance != 0) {
      return;
    }
    
    // now at zero, read in data
    float frames[500][3] = {{-1, -1, -1}};
    int frame_counter = 0;
    float frame[3] = {};
   // Collect incoming point data
    while (Serial.available() > 0 || frame_counter >= 500) {
      String point = Serial.readString();
      int index = 0;
      for (int i = 0; i < 3; i++) {
        int next_index = point.indexOf(',', index);
        Serial.println(String(next_index));
        Serial.println("substring: " + point.substring(index, next_index));
        Serial.println("Float: " + String(point.substring(index, next_index).toFloat()));
        float f = point.substring(index, next_index).toFloat();
        frame[i] = f;
        Serial.println("F: " + String(f));
        index = next_index + 1;
      }
    //  Serial.println("Frame: " + String(frame[0]) + " " + String(frame[1]) + " " + String(frame[2]));
      frame_counter++;
    }
 
 
    // move X and Y in Z order 0 to Max.
    float previous_z = -1;
    for (int i = 0; i < frame_counter; i++) {
        float* frame = frames[i];
        if (frame[0] == -1 && frame[1] == -1 && frame[2] == -1) {
          break;
        }

        float current_z = frame[2];
        drawPoint(frame[0], frame[1], current_z);
        if (previous_z > current_z) {
          Serial.println("Sorting is wrong. Current Z is: " + String(current_z) + " Previous Z is: " + String(previous_z));
        }
        previous_z = current_z;
        
    }

    Serial.println("Distance: " + String(getCmDistance()) + "cm");
 
 
  
}
 
void drawPoint(float x, float y, float z_distance) {
 
  float x_angle = atan(x/z_distance) * (180 / PI);
  float y_angle = atan(y/z_distance) * (180 / PI);
 
  x_servo.write(x_pos % 90);
  y_servo.write(y_pos % 90);
 
}
 
// TODO: Get proper units later
float parseZaxis(float raw_distance) {
  return raw_distance;
}
 
float getCmDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  float duration = pulseIn(echoPin, HIGH);
  float cm = (duration/2) / 29.1;
 
  return cm;
}
