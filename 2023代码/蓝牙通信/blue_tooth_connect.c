#define LED RED_LED  

void setup() {
  Serial.begin(9600); 
  pinMode(LED, OUTPUT); 
}

void loop() {
    if (Serial.available()) 
    {
      char data_received;
      data_received = Serial.read(); 

      if (data_received == '1')  
      {
        digitalWrite(LED, HIGH); 
        Serial.write("LED turned ON"); 
      }
      else if (data_received == '2')  
      {
        digitalWrite(LED, LOW); 
        Serial.write("LED turned OFF"); 
      }
    }
}
