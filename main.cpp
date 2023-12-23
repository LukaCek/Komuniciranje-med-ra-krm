#define LedYallow 13
#define dirPin 12
#define stepPin 11
#define GripperPin 15
int dt=300;
String GripperType = "Digital"; //Servo or Digital
String msg;
char msgType;
bool gripperStartus = 0; //0=Opend, 1=Closed;


int extractValue(String msg, char identifier){
  // Poišči pozicijo identifikatorja v sporočilu
  int index = msg.indexOf(identifier);

  // Izloči vrednost po identifikatorju
  String valueStr = msg.substring(index + 1, msg.indexOf(" ", index));
  
  // Pretvori niz v celo število
  return valueStr.toInt();
}
void premikMotorja(int koraki, bool smer, int d){
    d=d*2;
    int i=0;
    bool state=1;
    while(i!=koraki){
    i++;
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(d);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(d);
    }
}

TaskHandle_t Task0;
void codeForTask0( void * parameter)
{
  for (;;) {
    Serial.println(micros());
    
    // Toggle LED
    digitalWrite(LedYallow, !digitalRead(LedYallow));
    
    // Gripper control
    if (GripperType == "Digital") {
      digitalWrite(GripperPin, gripperStartus ? HIGH : LOW);
    } else if (GripperType == "Servo") {
      // Implement Servo control logic here
    }
    
    // Use vTaskDelay instead of delay to avoid blocking
    vTaskDelay(dt);
  }
}

void setup() {
  pinMode(GripperPin, OUTPUT);
  pinMode(LedYallow, OUTPUT);
  digitalWrite(LedYallow, LOW);

  Serial.begin(115200);
  Serial.println("Robot started (-=");

  xTaskCreatePinnedToCore(codeForTask0, "Task_0", 1000, NULL, 1, &Task0, 0);
}

void loop() {
  while (Serial.available() == 0) {
  }
  msg = Serial.readString();
  msg.trim();

  //preverino vrsto sporočila
  msgType = msg[0];

  if (msgType == 'M'){
    Serial.println("To je Move.");

    // Preveri, ali sporočilo ustreza formatu "Xn Ym"
    if (msg.indexOf("X") != -1 && msg.indexOf("Y") != -1){
    // Izloči vrednosti X in Y iz sporočila
    int pozX = extractValue(msg, 'X');
    int pozY = extractValue(msg, 'Y');

    // Uporabi ali shranjuj podatke po potrebi
    // Primer: Izpiši vrednosti na serijski vmesnik
    Serial.print("Pozicija X: ");
    Serial.println(pozX);
    Serial.print("Pozicija Y: ");
    Serial.println(pozY);
    } else {
      // Sporočilo ni v pravilnem formatu
      Serial.println("Napaka: Prišel si v M ampak tukaj ni M ali pa je nekaj narobe.");
    }
    
  }
  else if (msgType == 'S'){
    Serial.println("To so premik v stopinjah.");
    premikMotorja(200,1,600);
  }
  else if (msgType == 'D'){
    Serial.println("Komanda za dt (delay time)");

    // Izloči vrednosti X in Y iz sporočila
    dt = extractValue(msg, 'D');
    // Primer: Izpiši vrednosti na serijski vmesnik
    Serial.print("DT: ");
    Serial.println(dt);
    
  }
  else if (msgType == 'G'){
    Serial.print("Gripper: ");
    //preverimo če gripper odpremo "G O" ali zapremo "G C"
    if (msg[2] == 'O'){
      Serial.println("Opening");
      gripperStartus = 0;
    } else if (msg[2] == 'C') {
      Serial.println("Cloasing");
      gripperStartus = 1;
    } else {Serial.println("ERROR");}

  }
  else {Serial.println("Ta vrsta sporočila ni znana.");}
  msg = "";
}