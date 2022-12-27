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
