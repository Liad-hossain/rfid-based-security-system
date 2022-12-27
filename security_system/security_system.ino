// Include required libraries
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SoftwareSerial.h>
#include <Servo.h>
#include <SPI.h>
#include <Adafruit_Fingerprint.h>

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(7, 6); // ------> need to change the pin here
//SoftwareSerial mySerial2(11,13);

#else
#define mySerial Serial1

#endif

MFRC522 mfrc522(10, 8); // MFRC522 mfrc522(SS_PIN, RST_PIN)
LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo sg90;
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t id;
constexpr uint8_t greenLed = 0;
//constexpr uint8_t redLed = 6;
constexpr uint8_t servoPin = 9;
constexpr uint8_t buzzerPin = 5;

char initial_password[4] = {'1', '4', '7', '7'};  // Variable to store initial password
String oldTagUID = "46 60 EC F9";  // String to store UID of tag. Change it with your tag's UID
String newTagUID=  "B3 42 08 92";
char password[4];   // Variable to store users password
boolean RFIDMode = true; // boolean to change modes
boolean NormalMode = true; // boolean to change modes
char key_pressed = 0; // Variable to store incoming keys
uint8_t i = 0;  // Variable used for counter

// defining how many rows and columns our keypad have
const byte rows = 4;
const byte columns = 4;

// Keypad pin map
char hexaKeys[rows][columns] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Initializing pins for keypad
byte row_pins[rows] = {A0, A1, A2, A3};
byte column_pins[columns] = {2, 3, 4, 1};

// Create instance for keypad
Keypad keypad_key = Keypad(makeKeymap(hexaKeys), row_pins, column_pins, rows, columns);

void setup() {
  Serial.begin(9600);
  // Arduino Pin configuration
  pinMode(buzzerPin, OUTPUT);

  sg90.attach(servoPin);  //Declare pin 8 for servo
  sg90.write(0); // Set initial position at 0 degrees

  lcd.init();   // LCD screen
  lcd.backlight();
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
  delay(100);
  fingerPrintSetup();
  delay(100);


  lcd.clear(); // Clear LCD screen
}

void loop() {
  {
    // System will first look for mode
    if (RFIDMode == true) {
      lcd.setCursor(0, 0);
      lcd.print("   Door Lock");
      delay(2000);
        lcd.clear();
        lcd.setCursor(0, 0);        
        lcd.print("Enter Pass or");
        lcd.setCursor(0, 1);
        lcd.print("Scan Tag|Finger");
        RFIDMode = false; // Make RFID mode false
    }

    // If RFID mode is false, it will look for keys from keypad
    if (RFIDMode == false) {
      key_pressed = keypad_key.getKey(); // Storing keys
      if (key_pressed)
      {
        lcd.clear();
        bool isKeyPressed=true;
        while(i<4){
            if(isKeyPressed)password[i++] = key_pressed; // Storing in password variable
            if(isKeyPressed)lcd.print("*");
            isKeyPressed=false;
            if(i==4)break;
            key_pressed = keypad_key.getKey();
            if(key_pressed)isKeyPressed=true;
        }
      }
      if (i == 4) // If 4 keys are completed
      {
        delay(200);
        if (!(strncmp(password, initial_password, 4))) // If password is matched
        {
          lcd.clear();
          lcd.print("Pass Accepted");
          sg90.write(90); // Door Opened
          digitalWrite(greenLed, HIGH);
          delay(10000);
          digitalWrite(greenLed, LOW);
          sg90.write(0); // Door Closed
          lcd.clear();
          i = 0;
          RFIDMode = true; // Make RFID mode true
        }
        else    // If password is not matched
        {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Wrong Password");
          lcd.setCursor(0, 1);
          lcd.print("Access Denied");
          digitalWrite(buzzerPin, HIGH);
          delay(3000);
          digitalWrite(buzzerPin, LOW);
          lcd.clear();
          i = 0;
          RFIDMode = true;  // Make RFID mode true
        }
      }
      getFingerprintIDez();
      int f=findTag();
      if(f==1){
          lcd.clear();
          lcd.print("Tag Accepted");
          sg90.write(90); // Door Opened
          digitalWrite(greenLed, HIGH);
          delay(10000);
          digitalWrite(greenLed, LOW);
          sg90.write(0); // Door Closed
          lcd.clear();
          i = 0;
          RFIDMode = true; // Make RFID mode true
      }
      else if(f==0){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Wrong Tag Shown");
        lcd.setCursor(0, 1);
        lcd.print("Access Denied");
        digitalWrite(buzzerPin, HIGH);
        delay(3000);
        digitalWrite(buzzerPin, LOW);
        lcd.clear();
        RFIDMode = true;
      }
    }
  }
}

int findTag(){
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
        return -1;
      }
    
      // Select one of the cards
    if (!mfrc522.PICC_ReadCardSerial()) {
      return -1;
    }

      //Reading from the card
    String tag = "";
    for (byte j = 0; j < mfrc522.uid.size; j++)
    {
      tag.concat(String(mfrc522.uid.uidByte[j] < 0x10 ? " 0" : " "));
      tag.concat(String(mfrc522.uid.uidByte[j], HEX));
    }
    tag.toUpperCase();
    //Serial.println(tag);
    if (tag.substring(1) == oldTagUID || tag.substring(1) == newTagUID)
      return 1;
    return 0;    
}

// setup function for fingerprint module
void fingerPrintSetup()
{
  Serial.begin(9600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  finger.begin(57600);
  if (finger.verifyPassword()){
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
   while (1) { delay(1); }
  }  
  
  delay(5);

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  else {
    Serial.println("Waiting for valid finger...");
    Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }  
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Wrong Finger:(");
      lcd.setCursor(0, 1);
      lcd.print("Access Denied");
      digitalWrite(buzzerPin, HIGH);
      delay(3000);
      digitalWrite(buzzerPin, LOW);
      lcd.clear();
      RFIDMode = true;                
      return -1;
  }

  if((finger.fingerID==1 || finger.fingerID==3) && finger.confidence>=50){
    adminMode();
  }
  else if (finger.confidence>=50) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(" Fingerprint");
      lcd.setCursor(0, 1);
      lcd.print("  Accepted");
      sg90.write(90); // Door Opened
      delay(10000);
      sg90.write(0); // Door Closed
      lcd.clear();
      i = 0;
      RFIDMode = true; // Make RFID mode true  
  }
  else{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Wrong Finger:(");
    lcd.setCursor(0, 1);
    lcd.print("Access Denied");
    digitalWrite(buzzerPin, HIGH);
    delay(3000);
    digitalWrite(buzzerPin, LOW);
    //digitalWrite(redLed, LOW);
    lcd.clear();
    i = 0;
    RFIDMode = true;  // Make RFID mode true    
  }
  return finger.fingerID;
}

uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}


void adminMode(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("1.Change 2.Add");
  lcd.setCursor(0,1);
  lcd.print("3.Delete 0.Exit"); 
  key_pressed = keypad_key.getKey();
  while(1){
    if(!key_pressed){
      key_pressed = keypad_key.getKey();
      continue;
    }
    if(key_pressed=='1'){
        changePassword();
        break;
    }
    else if(key_pressed=='2'){
        addFinger();   
        break;           
    }
    else if(key_pressed=='3'){
        deleteFinger();  
        break;    
    }  
    else if(key_pressed=='0')break; 
    else{
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Wrong Key");
        lcd.setCursor(0,1);
        lcd.print("Enter Valid Key");
        delay(10000);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("1.Change 2.Add");
        lcd.setCursor(0,1);
        lcd.print("3.Delete 0.Exit");  
        key_pressed = keypad_key.getKey();
    }
  }
    lcd.clear();
    lcd.setCursor(0, 0);        
    lcd.print("Enter Pass or");
    lcd.setCursor(0, 1);
    lcd.print("Scan Tag|Finger");
}

void changePassword(){    
  //Serial.println("Changing password");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Enter New");
    lcd.setCursor(0,1);
    lcd.print(" Password");
    i=0;
    key_pressed = keypad_key.getKey(); // Storing keys
    while(!key_pressed)
    {
        key_pressed = keypad_key.getKey();
        if(key_pressed){
          lcd.clear();
          bool isKeyPressed=true;
          while(i<4){
              if(isKeyPressed)initial_password[i++] = key_pressed; // Storing in password variable
              if(isKeyPressed)lcd.print("*");
              isKeyPressed=false;
              if(i==4)break;
              key_pressed = keypad_key.getKey();
              if(key_pressed)isKeyPressed=true;
          }
        }
    }  
    i=0;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Password Changed");
    lcd.setCursor(0,1);
    lcd.print("Successfully!!"); 
    delay(5000);   
}


void addFinger()                     // run over and over again
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" Place Your");
  lcd.setCursor(0,1);
  lcd.print("   Finger");
  uint8_t id2 = 2;
  for(;;id2++){
    uint8_t p = finger.loadModel(id2);
    if(p!=FINGERPRINT_OK)break;
  }
  int added=getFingerprintEnroll(id2);
  while(1){
    if(added==3)return;     //For Admin's Finger
    if(added==1)break;
    added=getFingerprintEnroll(id2);
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Finger Added");
  lcd.setCursor(0, 1);
  lcd.print("Successfully!!");
  delay(4000);
}


uint8_t getFingerprintEnroll(uint8_t id2) {
  int p = -1;
  //Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id2);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      //Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      //Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      //Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      //Serial.println("Imaging error");
      break;
    default:
      //Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      //Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      //Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      //Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      ////Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      //Serial.println("Could not find fingerprint features");
      return p;
    default:
      //Serial.println("Unknown error");
      return p;
  }
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK){  
      if(finger.fingerID==1 || finger.fingerID==3)  //Admin's Finger ID
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("   Exited");
        delay(2000);
        return 3;
      }
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Finger Already");
      lcd.setCursor(0,1);
      lcd.print("   Stored");
      delay(2000);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(" Place Your");
      lcd.setCursor(0,1);
      lcd.print("   Finger");
      return -1;
  }
  //Serial.println("Remove finger");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" Remove Your");
  lcd.setCursor(0, 1);
  lcd.print("   Finger");
  delay(2000);

  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id2);
  p = -1;
  Serial.println("Place same finger again");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" Place Same");
  lcd.setCursor(0, 1);
  lcd.print("   Finger");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      //Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      //Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      //Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      //Serial.println("Imaging error");
      break;
    default:
      //Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      //Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      //Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      //Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      //Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      //Serial.println("Could not find fingerprint features");
      return p;
    default:
      //Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id2);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    //Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    //Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    //Serial.println("Fingerprints did not match");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" Not Same");
    lcd.setCursor(0,1);
    lcd.print("   Finger");    
    return p;
  }
  else{
    //Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID "); Serial.println(id2);
  p = finger.storeModel(id2);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    //Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    //Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    //Serial.println("Error writing to flash");
    return p;
  } else {
    //Serial.println("Unknown error");
    return p;
  }

  return true;
}


void deleteFinger(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Place Finger");
  lcd.setCursor(0, 1);
  lcd.print("To Delete");  
  
  int found=0;
  while(found<1){
    found=toDelete();    
  }
  if(found==1){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Finger Deleted");
    lcd.setCursor(0, 1);
    lcd.print("Successfully!!"); 
  }
  else if(found==3){   //For Admmin's Finger
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("   Exited");
      delay(2000);
    return;
  }
  else{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Finger is not");
    lcd.setCursor(0, 1);
    lcd.print("  Found!!");     
  }
  delay(3000);
}


int toDelete(){
  uint8_t p = finger.getImage();
  while (p != FINGERPRINT_OK){
      return -1;    
  }
  //Serial.println("Getting finger");  
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK){
      return 2;
  }

  uint8_t id3 = finger.fingerID;
  if (id3 == 0) {// ID #0 not allowed, try again!
     return 2;
  }

  if(id3==1 || id3==3){    //For Admin's ID
     return 3;
  }
  Serial.print("Deleting ID #");
  Serial.println(id3);

  deleteFingerprint(id3);
  return 1;
}

void deleteFingerprint(uint8_t id3) {
  uint8_t p = -1;

  p = finger.deleteModel(id3);


  // return p;
}

