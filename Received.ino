byte Received(){
    byte R = 0;
    while(1){
      if(mySerial.available() > 0){
         ch = mySerial.read();
         if(ECHO){
           Serial.print(ch,HEX);Serial.print(" ");
         }
         if(flag){
            received[i] = ch;
            i++;
            if(i > 8){
               i = 0;
            }
         }
         if(ch == 0xAA && !flag){
               flag = 1;
               i = 0;
         }
         if(ch == 0xAB && flag){
               flag = 0;  
               if(ECHO){
                   Serial.println();
                   for(i = 0;i < PAYLOAD_LEN; i++){
                          Serial.print(received[i],HEX);Serial.print(" ");
                   }
                   Serial.println();
               }
               int check = received[1]+received[2]+received[3]+received[4]+received[5]+received[6];
               check = check % 256;
               if(check != received[7]){
                  if(ECHO)Serial.println(" ERROR: message rejected");
                  err = 1;
               }else{
                  if(ECHO)Serial.println(" GOOD : message accepted");
                    err = 0;
                    R = 1;
                    break;  // a full message is read
               }
         }
      }
    }
    return R;
}
