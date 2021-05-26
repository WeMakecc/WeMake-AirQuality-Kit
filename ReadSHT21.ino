void ReadSHT21(){
    Humi = SHT2x.GetHumidity();  // read sensor
    Temp = SHT2x.GetTemperature();  // read sensor
    delay(1000);
}
