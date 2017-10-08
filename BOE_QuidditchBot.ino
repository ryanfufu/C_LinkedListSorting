#include <Servo.h>

Servo servoLeft;                                              // Left servo
Servo servoRight;                                             // Right servo

#define enablePin  11                                         // Connects to the RFID's ENABLE pin
#define rxPin      19                                         // Serial input (connects to the RFID's SOUT pin)
#define txPin      18                                         // Serial output (unused)
int count2 = 0;                                               // Counter for black hashmarks
int threshold = 400;                                          // Threshold for QTI sensors
int go = 1;                                                   // Sets the loop for line following and RFID detecting
int score = 0;                                                // Initializes variable for the pair score
int finished = 0;                                             // Sets the loop for receiving from Chaser #1 and calculating the pair score
int seeker = 0;                                               // Prints pair score to LCD and sends to Chaser #1
int seeker2 = 0;                                              // Sets the loop to wait for a transmission from the Seeker
int finalScore = 0;                                           // Initialize the final score variable
String detect = "";                                           // Initializes the string in which tags are detected
String receive = "";                                          // Initializes the string in which transmissions from Chaser #1 are receiver
char toSend;                                                  // Initializes the character that represents the team score

void setup()                                                  // Built-in initialization block
{
  servoLeft.attach(13);                                       // Left servo attached to pin 13
  servoRight.attach(12);                                      // Right servo attached to pin 12
  pinMode(enablePin, OUTPUT);                                 // Sets up pin for RFID sensor
  pinMode(rxPin, INPUT);                                      // Sets up pin for input from RFID sensor
  pinMode(2, INPUT);                                          // Set pin 2 as input for the button
  pinMode(10, OUTPUT);                                        // Sets pin as output for LED
  pinMode(9, OUTPUT);                                         // Sets pin as output for LED
  digitalWrite(enablePin, LOW);                               // Enable the RFID Reader
  Serial.begin(9600);                                         // Set data rate to 9600 bps
  Serial1.begin(2400);                                        // Begins serial for RFID sensor
  Serial2.begin(9600);                                        // Begins serial for LCD screen
  Serial3.begin(9600);                                        // Begins serial for Xbee
  Serial2.write(12);                                          // Clears the LCD screen      
  delay(5);                                                   // Delay for LCD screen
}

void loop()                                                   // Main loop auto-repeats
{
  if (go == 1) {                                              // First loop include lines following and RFID tag detection
    long tLeft = rcTime(5);                                   // Finds time constant for left QTI sensor
    long tCenter = rcTime(6);                                 // Finds time constant for center QTI sensor
    long tRight = rcTime(7);                                  // Finds time constant for right QTI sensor

    // Case 1: black-black-black, go forward and check for RFID tags
    if (tLeft > threshold && tCenter > threshold && tRight > threshold) {
      Serial1.end();                                          // Ends RFID serial to get rid of old data
      Serial1.begin(2400);                                    // Starts RFID serial again
      if (count2 <= 4) {                                      // If there have been less than 5 hashmarks, go forward to sense for tag
        servoLeft.writeMicroseconds(1400);                    // Go forward
        servoRight.writeMicroseconds(1600);
        delay(500);                                             
        count2++;                                             // Increment hashmark count
        servoLeft.writeMicroseconds(1500);                    // Stop and pause
        servoRight.writeMicroseconds(1500);
        delay(500);
      }
      else if (count2 == 5) {                                 // If this is the last hashmark, stop and reverse
        servoLeft.writeMicroseconds(1500);                    // Go towards last line
        servoRight.writeMicroseconds(1500);
        delay(250);
        servoLeft.writeMicroseconds(1600);                    // Reverse
        servoRight.writeMicroseconds(1400);
        delay(1500);
        servoLeft.writeMicroseconds(1500);                    // Stop
        servoRight.writeMicroseconds(1500);
        delay(1000);  
        go = 0;                                               // Move onto next segment of code (receiving)
      }
      if (Serial1.available() && count2 <= 5) {               // If tag is detected and at a hashmark
        digitalWrite(9, HIGH);                                // Turn on LED
        delay(500);                                           // Pause
        digitalWrite(9, LOW);                                 // Turn off LED
        detect = detect + 'u';                                // Add a "u" to indicate detection in the string
        Serial1.end();                                        // Terrible solution to end stream of data from RFID sensor
        Serial1.begin(2400);
      }
      else if (count2 <= 5) {                                 // Tag not detected and at a hashmark
        detect = detect + 'v';                                // Add a "v" to indidcate no detection      
      }
    }

    // Case 2: white-black-white, go forward
    else if (tLeft <= threshold && tCenter > threshold && tRight <= threshold) {
      servoLeft.writeMicroseconds(1400);
      servoRight.writeMicroseconds(1600);
    }

    // Case 3: white-black-black, go right
    else if (tLeft <= threshold && tCenter > threshold && tRight > threshold) {
      servoLeft.writeMicroseconds(1450);
      servoRight.writeMicroseconds(1450);
      delay(50);
    }

    // Case 4: black-black-white, go left
    else if (tLeft > threshold && tCenter > threshold && tRight <= threshold) {
      servoLeft.writeMicroseconds(1550);
      servoRight.writeMicroseconds(1550);
      delay(50);
    }

    // Case 5: white-white-black, go right
    else if (tLeft <= threshold && tCenter <= threshold && tRight > threshold) {
      servoLeft.writeMicroseconds(1450);
      servoRight.writeMicroseconds(1450);
      delay(50);
    }

    // Case 6: black-white-white, go left
    else if (tLeft > threshold && tCenter <= threshold && tRight <= threshold) {
      servoLeft.writeMicroseconds(1550);
      servoRight.writeMicroseconds(1550);
      delay(50);
    }

    // Case 7: white-white-white, stop
    else if (tLeft <= threshold && tCenter <= threshold && tRight <= threshold) {
      servoLeft.writeMicroseconds(1500);
      servoRight.writeMicroseconds(1500);
    }

  }
  else if (finished != 1) {                                   // Line following and detection code is finished, send to Chaser #1 and calculate pair score
    if (Serial3.available() && receive.length() < 5) {        // If incoming transmission
      char incoming = Serial3.read();                         // Read the transmission
      if (incoming == 'u' || incoming == 'v') {               // Only add to the received string if it is a "u" or "v"
        receive += incoming;                                  // Add to the string
      }
    }

    if (receive.length() == 5) {                              // If 5 characters have been received
      for (int i = 0; i < 5; i++) {                           // Loop through each character
        // If characters at this position in both strings are the same
        if (detect.charAt(i) == 'u' && receive.charAt(i) == 'u') {
          score += 10;                                        // Add ten points
        }
      }
      finished = 1;                                           // Finished this segment of code, move onto sending
    }

  }
  else if (seeker != 1) {                                     // Move onto sending
    Serial2.write(12);                                        // Clear the LCD
    delay(5);
    Serial2.print(score);                                     // Write to the LED
    Serial2.write(13);
    delay(2000);                                              // Wait before sending
    toSend = 97 + (score / 10);                               // Calculates ASCII of character to send
    Serial3.print(toSend);                                    // Sends this character
    seeker = 1;                                               // Finish this segment of code, move onto receiving from Seeker
  }
  else if (seeker2 != 1) {                                    // Start receiving from Seeker
    if (Serial3.available()) {                                // If receiving a transmission
      char incoming = Serial3.read();                         // Read the transmission
      if (incoming == 'z') {                                  // If the Seeker sent a "z"
        Serial3.print(toSend);                                // Sends score to Seeker
        seeker2 = 1;                                          // Move onto next segment of code
      }
    }
  }
  else {                                                      // Last segment to receive from Seeker
    if (Serial3.available()) {                                // If receive a transmission
      char ascii = Serial3.read();                            // Converts transmission to ASCII
      if (ascii >= 65 && ascii <= 90) {                       // If ASCII is within range of possible scores
        finalScore = (ascii - 65) * 10;                       // Convert ASCII to integer
        Serial2.write(12);                                    // Clear the LCD
        delay(5);
        Serial2.print(finalScore);                            // Print final score to LCD
        Serial2.write(13);
        delay(100000000);                                     // Program is done!
      }
    }
  }

}

long rcTime(int pin)                                          // Returns decay time
{
  pinMode(pin, OUTPUT);                                       // Charge capacitor
  digitalWrite(pin, HIGH);                                    // ...by setting pin output to high
  delay(1);                                                   // ...for 1 ms
  pinMode(pin, INPUT);                                        // Set pin to input
  digitalWrite(pin, LOW);                                     // ...with no pullup
  long time  = micros();                                      // Mark the time
  while (digitalRead(pin));                                   // Wait for voltage < threshold
  time = micros() - time;                                     // Calculate decay time
  return time / 0.8675;                                       // Return decay time
}

