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
