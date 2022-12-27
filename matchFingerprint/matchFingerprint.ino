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
