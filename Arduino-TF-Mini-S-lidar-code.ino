/*
Website: https://sonalabs.org
YouTube: https://youtube.com/@sonalabs

Project Title: Interfacing TF Mini-S Lidar (time-of-flight)distance sensor module with Arduino UNO R3....

Read more here: 

This sketch communicates to the TFMini using a SoftwareSerial port at a baud rate of 115200, 
while communicating the distance results through the default Arduino hardware
Serial debug port. 

The (UNO) circuit:
 * Uno RX is digital pin 10 (connect to TX of TF Mini)
 * Uno TX is digital pin 11 (connect to RX of TF Mini)

 */  

 // library needed for communicating with the TF Mini-S Lidar Sensor.
#include <SoftwareSerial.h> 

// The TF Mini Header file contains the definitions of the TF Mini class
#include "TFMini.h" 

// Creates a TF Mini object called tfmini
TFMini tfmini;      

// Setup software serial port
// Uno Rx (TF Mini Tx), Uno Tx (TF Mini Rx)
 
SoftwareSerial SerialTFMini(10, 11);         


// Function that gets the distance and strength data from the TF Mini sensor.
// The first parameter is a pointer to an integer that will store the distance data &
// The second parameter is a pointer to an integer that will store the strength data
// This function serves to extract data from the TFMini sensor & update the values pointed to
// by 'distance' & 'strength'.

void getTFminiData(int* distance, int* strength)
{
  // Declare & initialize several variables used within the 'getTFMiniData' function.
  // 'i' is a counter used to keep track of the index for received data.
  static char i = 0;

  // 'j' is a counter used in the checksum calculation loop
  char j = 0;

  // 'checksum' is a variable used to calculate the checksum value
  int checksum = 0;

  // 'rec' is an array of size 9 used to store the received data bytes
  static int rec[9];

  // The while loop ensures that all the available bytes are read from the SerialTFMini object.
  while (SerialTFMini.available())
  {
    // Read a byte from the 'SerialTFMini' object & store it in the 'rec' array at the current index 'i'.
    rec[i] = SerialTFMini.read();
    
    // This if statement checks if 'i' is 0 and the received data at index 0 is not equal to '0x59'
    // '0x59' is an hexadecimal value that represents the start of a data frame.
    if (i == 0 && rec[0] != 0x59)
    {
      // if the condition is true, it resets 'i' to 0.
      i = 0;
    }

    // This 'else if' statement checks if 'i' is 1 and the received data at index 1 is not equal to '0x59'
    else if (i == 1 && rec[1] != 0x59)
    {
      // if the condition is true, it resets 'i' to 0.
      i = 0;
    }

    // This checks if 'i' is 8 - meaning all the 9 bytes of the data frame have been received.
    else if (i == 8)
    {
      // It then calculates the checksum by summing up the first 8 bytes in the 'rec' array.
      for (j = 0; j < 8; j++)
      {
        checksum += rec[j];
      }

      // If the received checksum at 'rec[8]' matches the calculated checksum, the 'distance' & 'strength'
      // values from the received data are extracted and the pointers are updated.
      if (rec[8] == (checksum % 256))
      {
        *distance = rec[2] + rec[3] * 256;
        *strength = rec[4] + rec[5] * 256;
      }

      // Resets 'i' to 0 to prepare for the next data frame
      i = 0;
    }

    // This is executed when none of the previous conditions are met.
    // It increments 'i' to move on to the next byte in the data frame.
    else
    {
      i++;
    }
  }
}


void setup()
{
  //Initialize hardware serial port (serial debug port)
  Serial.begin(115200);  
  
   // wait for serial port to connect. Needed for native USB port only     
  while (!Serial);           
  Serial.println ("Initializing...");

   //Initialize the data rate for the SoftwareSerial port
  SerialTFMini.begin(TFMINI_BAUDRATE);   

  //Initialize the TF Mini sensor
  tfmini.begin(&SerialTFMini);            
}
 
void loop()
{
  // Declare & initialize two variables (distance & strenght) to zero.
  int distance = 0;
  int strength = 0;

  // Call the 'getTFMiniData' function to retreive the latest distance & strength values from the TF Mini Sensor 
  getTFminiData(&distance, &strength);

  // The while loop is executed as long as the 'distance' value is zero (false)
  // It ensures that valid distance data is obtained from the sensor.
  while (!distance)
  {
    // Within the while loop, the 'getTFMiniData' function is called again to update the ('d & s') values
    getTFminiData(&distance, &strength);

    // The 'if' statement checks if the 'distance' value is non-zero (true)
    // Indicating that valid data has been received from the sensor.
    if (distance)
    {
       // If the condition is true, display the measurement
       // The lines below output the distance and strength values to the serial monitor.
      Serial.print(distance);
      Serial.print("cm\t");
      Serial.print("strength: ");
      Serial.println(strength);
    }
  }

   // Wait some short time (100ms) before taking the next measurement
   // This wait time ensures that the sensor or the serial communication is not overwhelmed.
  delay(100);
}
