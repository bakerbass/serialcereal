// const int FSR_PIN = A0; // FSR and 100K resistor connected to A0
// int fsrReading;         // the analog reading from the FSR resistor divider

class Sensor {
  /*
  Sensor class has a few key parameters:
   * the pin parameter indicates which physical pin should be read to read the sensor, EG A0, A1, etc
   * the MNN parameter is a bit of a misnomer, because it can be used as a MNN for note-sending sensors, 
     but can also be used to select the CC number if the sensor is being used for CC values.
  */
public:
  void setup(int setPin, uint8_t setMNN, int setThreshold = 10)
  {
    PIN = setPin;
    MNN = setMNN;
    isOn = false;
    prevReading = 0;
    threshold = setThreshold;
  }
  bool readSensorMN() {
    reading = analogRead(PIN);  // Read the value from the FSR

    if(reading > threshold && !isOn) {  
      if (MNN == 60)
      {
        Serial.println("ass on");
      }   
      // Serial.println("isOn: ");
      // Serial.println(isOn);  
      //velocity curve and shift from 10 bits
      velocity = log10((reading >> 4) + 8)*60; 
      // bit shift from 10 bits to 7 bits
      // velocity = reading >> 3;
      //send note on with velocity
      usbMIDI.sendNoteOn(MNN, velocity, 0); 
      // serial monitoring
      Serial.printf("current pin: %d\n", PIN);
      Serial.println("reading: ");
      Serial.println(reading);
      Serial.println("velocity: ");
      Serial.println(velocity);
      isOn = true;
    }
    // if, while the note is on, the sensor falls below the threshold, send a note off and change isOn to false
    else if (reading < threshold && isOn){ 
      if (MNN == 60){ //if this is the ass sensor off
        //send all notes off
        usbMIDI.sendControlChange(123,127, 0);
        Serial.println("ass off");
      }
      else{
        usbMIDI.sendNoteOff(MNN, 0, 0);
        // Serial.println("Off");
        isOn = false;
    }
    }
    /* some pseudo MPE logic: 
    else if (reading > (lastreading + or - mpeThreshold) && isOn) //basically, if the reading is different than the other reading by some amount and the note is still on, we should send MPE messege
        usbMIDI.sendPolyPressure(uint8_t note, uint8_t pressure, uint8_t channel) //perhaps this should also be sent with a note on
    */
    return isOn;
  }
  void readSensorCC() {
    reading = analogRead(PIN);
    int thresh = 64;
    // Serial.println("reading: ");
    // Serial.println(reading);
    if (reading > prevReading + thresh || reading < prevReading - thresh){
      uint8_t cc = reading >> 3;
      usbMIDI.sendControlChange (MNN, cc, 0);
      prevReading = reading;
    }
  }
  void readSensorANO(){
    // Serial.println(reading);
    if (reading > threshold){
      usbMIDI.sendControlChange(123,127, 0);
      Serial.println("Panic!!");
    }
  }
private:
  int PIN;
  uint8_t MNN;
  bool isOn = false;
  uint8_t velocity;
  int reading;
  int prevReading;
  int threshold = 10;
};


Sensor ASS; // Force Sensitive Resistor
Sensor Bowl1; 
Sensor Bowl2;
Sensor Bowl3;  // piezo
void setup() {
  // set serial monitor baud rate
  Serial.begin(3600);
  // set up sensors (Pin, Midi note number)
  ASS.setup(A0, 60);
  Bowl1.setup(A1, 76, 1);
  Bowl2.setup(A2, 80, 1);
  Bowl3.  setup(A3, 53, 1);
}

void loop() {
  // ASS.isOn = readSensorMN(ASS);
  ASS.readSensorMN();
  Bowl1.readSensorMN();
  Bowl2.readSensorMN();
  Bowl3.  readSensorMN();
  while (usbMIDI.read()) {} //ignore input value
  delay(100); // Delay between readings for stability
  // should avoid using delay, can cause MIDI issues
}
// template <size_t max_sensors>
// class SensorManager
// {

// }
// struct fsr {
//     const int PIN;
//     int reading;
//     int MNN;
//     bool isOn;
// };
// struct fsr ASS = {A0, 0, 60};