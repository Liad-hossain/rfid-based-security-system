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

