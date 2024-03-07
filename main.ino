// +++        +++       ___ \  \     ++
// +           ++        \-____|     +
// +         +++                    ++
// +         +                      +
// +        ++                      +
// +        ++                      +
// ++       +                      +
//  +      ++       +  +        +  +
//  ++    +++++++++ +  +        +  +
//   ++   +       + +  +    +   +  +
//    ++  +       + +  +    +   +  +
//    +++++       +++  +    +   +  +
//       ++       +++  ++++++   +  +
//        +++++++++++  +++  +++++  +++
//                  ++++++      ++++++
#include <ESP32Servo.h> 
#include <ArtronShop_LineNotify.h>
#include <WiFi.h>
#define TRIG_PIN 13 // TRIG pin
#define ECHO_PIN 19 // ECHO pin
#define SERVO_PIN 18 // ECHO pin
//env
#define WIFI_SSID "" // WiFi Name
#define WIFI_PASSWORD "" // WiFi Password
#define LINE_TOKEN "" // LINE Token

Servo servoMotor;
float filterArray[20]; // array to store data samples from sensor
float distance; // store the distance from sensor
bool is_fristtime=true;
void setup() {
  // begin serial port
  Serial.begin (9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID,WIFI_PASSWORD);
   while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
servoMotor.attach(SERVO_PIN); 
  // configure the trigger and echo pins to output mode
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
    LINE.begin(LINE_TOKEN);
}

void loop() {
  // 1. TAKING MULTIPLE MEASUREMENTS AND STORE IN AN ARRAY
  for (int sample = 0; sample < 20; sample++) {
    filterArray[sample] = ultrasonicMeasure();
    delay(30); // to avoid untrasonic interfering
  }

  // 2. SORTING THE ARRAY IN ASCENDING ORDER
  for (int i = 0; i < 19; i++) {
    for (int j = i + 1; j < 20; j++) {
      if (filterArray[i] > filterArray[j]) {
        float swap = filterArray[i];
        filterArray[i] = filterArray[j];
        filterArray[j] = swap;
      }
    }
  }

  // 3. FILTERING NOISE
  // + the five smallest samples are considered as noise -> ignore it
  // + the five biggest  samples are considered as noise -> ignore it
  // ----------------------------------------------------------------
  // => get average of the 10 middle samples (from 5th to 14th)
  double sum = 0;
  for (int sample = 5; sample < 15; sample++) {
    sum += filterArray[sample];
  }

  distance = sum / 10;

  // print the value to Serial Monitor
  Serial.print("distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  if (distance <= 10){
    open();
    if ( is_fristtime)
    LINE.send("มีจดหมายมาส่ง");
    is_fristtime= false;
  }else{
    close();
    is_fristtime=true;
  }
}
void open(){
    servoMotor.write(90);
}
void close(){
    servoMotor.write(0);
}

float ultrasonicMeasure() {
  // generate 10-microsecond pulse to TRIG pin
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // measure duration of pulse from ECHO pin
  float duration_us = pulseIn(ECHO_PIN, HIGH);

  // calculate the distance
  float distance_cm = 0.017 * duration_us;

  return distance_cm;
}
