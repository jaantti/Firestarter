String aa;

void setup(){
  Serial.begin(9600);
}

void loop(){
  while(Serial.available > 0){
    delay(3);
    if (Serial.available > 0){
      char a = Serial.read();
      aa += a;
    }
  }
  if (aa.length() > 0){
    Serial.println(aa);
  }
  aa = "";
  
  
}

