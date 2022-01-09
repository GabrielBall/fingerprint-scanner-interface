#include <FPS_GT511C3.h>
#include <SoftwareSerial.h>
FPS_GT511C3 fps(4, 5);
SoftwareSerial ss(4, 5);

const int enrollPin = 9;
const int buttonPin = 10;
const int redLed = 11;
const int greenLed = 12;
const int solenoidPin = 13;

void setup() {
  pinMode ( enrollPin , INPUT );
  pinMode ( buttonPin , INPUT );
  pinMode ( redLed , OUTPUT );
  pinMode ( greenLed , OUTPUT );
  pinMode ( solenoidPin , OUTPUT );

  Serial.begin(9600); //set up Arduino's hardware serial UART
  fps.begin(9600);
  delay(100);
  fps.UseSerialDebug = false; // set to true for debug
  fps.Open(); //send serial command to initialize fps
  fps.SetLed(True);
}
void loop()
{
  if (digitalRead(solenoidPin) == Low){
    digitalWrite(redLed,HIGH);
  }
  IDFinger();
  if (digitalRead(enrollPin) == HIGH){
    Enroll();
  }
  if (digitalRead(buttonPin) == HIGH){
    reset();
    Serial.println("RESET");
  }
}
void SerialPassthrough()
{
  if (Serial.available())
  { // If data comes in from serial monitor, send it out to FPS
    ss.write(Serial.read());
  }
  if (ss.available())
  { // If data comes in from FPS, send it out to serial monitor
    Serial.write(ss.read());
  }
}
void IDFinger() {
  // Identify fingerprint test
  if (fps.IsPressFinger())
  {
    fps.CaptureFinger(false);
    int id = fps.Identify1_N();
    if (id < 200) {
      //if the fingerprint matches, provide the matching template ID
      Serial.print("Verified ID:");
      Serial.println(id);
      digitalWrite(solenoidPin , HIGH );
      Serial.println("Lock open");
      digitalWrite(greenLed , HIGH );
      digitalWrite(redLed , LOW );
    }
    else{
      //if unable to recognize
      Serial.println("Finger not found");
      digitalWrite (redLed , HIGH );
    }
  else {
    Serial.println("Please press finger");
  }
  delay(100);
}

void Enroll()
{
  // Find open enroll id
  int enrollid = 0;
  bool usedid = true;
  while (usedid == true)
  {
    usedid = fps.CheckEnrolled(enrollid);
    if (usedid == true) enrollid++;
  }
  fps.EnrollStart(enrollid);
  
  // enroll
  Serial.print("Press finger to Enroll #");
  Serial.println(enrollid);
  
  while(fps.IsPressFinger() == false) delay(100);
  bool bret = fps.CaptureFinger(true);
  int iret = 0;
  if (bret != false)
  {
    Serial.println("Remove Finger");
    fps.Enroll1();
    while (fps.IsPressFinger() == true) delay(100);
    Serial.println("Press same finger again");
    while (fps.IsPressFinger() == false) delay(100);
    bret = fps.CaptureFinger(true);
    if (bret != false)
    {
      Serial.println("Remove finger");
      fps.Enroll2();
      while (fps.IsPressFinger() == true) delay(100);
      Serial.println("Press same finger again");
      while (fps.IsPressFinger() == false) delay(100);
      bret = fps.CaptureFinger(true);
      if (bret != false)
      {
        Serial.println("Remove finger");
        iret = fps.Enroll3();
        if (iret == 0)
        {
          Serial.println("Enrolling successful");
        }
        else {
          Serial.print("Enrolling failed with error code:");
          Serial.println(iret);
        }
      }
      else Serial.println("Failed to capture third finger"); 
    }
    else Serial.println("Failed to capture second finger");
  }
  else Serial.println("Failed to capture first finger");
}

void reset()
{
  digitalWrite (greenLed , LOW);
  digitalWrite (redLed , HIGH);
  digitalWrite (solenoidPin , LOW);
}

void Clear()
{
  fps.DeleteAll();
}
