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
  else if(found==3){   //For Admin's Finger
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

  if(id3==1 || id3==3){  //For Admin's ID;
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

