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