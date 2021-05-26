void SensInit(){
  // Initializing SHT21
  Wire.begin();
  Wire.beginTransmission(0x40);
  error = Wire.endTransmission();
  if(error == 0){
      SHT21present = 1;
  }else{
      SHT21present = 0;
  }
    if(ECHO){
      Serial.print(" SHT21 is present ? ");
      if(ECHO && SHT21present)Serial.print(" YES ");
      if(ECHO && !SHT21present){
        Serial.print(" NO; error = ");
        Serial.print(error);
      }
      Serial.println();
  }
}
