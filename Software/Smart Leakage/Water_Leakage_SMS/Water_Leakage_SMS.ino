int waterdetector = 0;
int led=13;
int leak;

void setup(){
Serial.begin(9600);
  //fona.begin(9600);
  pinMode (waterdetector, INPUT);
  pinMode (led, OUTPUT);
}
void loop(){
detect();
}

void detect(){
  leak = analogRead(waterdetector);
  //Serial.println(leak);
  delay(2000);

  if (leak<=600){
digitalWrite(led,HIGH);
Serial.println("leaking");
Serial.print("\r");
delay(1000);
Serial.print("AT+CMGF=1\r");
delay(1000);
Serial.print("AT+CMGS=\"+91944xxxxxx8\"\r");
delay(1000);
Serial.print("Water leaking here\r");
delay(1000);
Serial.write(0x1A);
delay(1000);
}

  else{
    digitalWrite(led,LOW);
    //Serial.println("leaking");
  }}

