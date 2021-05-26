void ReadBattery(){
  int out = analogRead(A0);
  delay(10);
  vBat = 0.00511 * out;
}
