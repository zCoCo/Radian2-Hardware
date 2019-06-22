#define VM_SENS A1
#define VINL_SENS A0
#define VLM_BRIDGE 9
#define VINL_BRIDGE 10

float read_vmVoltage(){
  return ((float) analogRead(VM_SENS)) * (14.99f / 4.99f) * 5.0f / 1024.0f;
}

float read_vinlVoltage(){
  return ((float) analogRead(VINL_SENS)) * (12.0f / 2.0f) * 5.0f / 1024.0f;
}

void setup(){
  Serial.begin(9600);
  pinMode(VM_SENS, INPUT);
  pinMode(VINL_SENS, INPUT);
} // #setup

void loop(){
  // Plot:
  Serial.print(read_vmVoltage()); Serial.print(','); Serial.println(read_vinlVoltage());
  delay(50);
} // #loop
