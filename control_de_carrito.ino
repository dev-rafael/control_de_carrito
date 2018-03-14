#include <SoftwareSerial.h>
SoftwareSerial BT(10,9);
char cnal;  //Variable que guarda la instruccion enviada por el control
int pinEncoder = 8;//Pin donde se conecta el sensor del econder
boolean encoder;
boolean encoder1;
boolean encoder2;
int pasos = 0;
int vueltas = 0;
int pinizq=5;//Pin de activacion izquierdo del primer puente H
int pinder=4;//Pin de activacion derecho del primer puente H
int pinvel=3;//Pin del enable, controla la velocidad
int velocidad=51;//Variable que guarda la velocidad
int pinder2=7;//Pin de activacion izquierdo del segundo puente H
int pinizq2=6;//Pin de activacion derecho del segundo puente H
void setup() {
  // put your setup code here, to run once:
BT.begin(9600);//Inicio de comunicacion Bluetooth
Serial.begin(9600);//Inicio de comunicacion serial
//Declaracion de modo de operacion de los pines
pinMode(pinder,OUTPUT);
pinMode(pinizq,OUTPUT);
pinMode(pinder2,OUTPUT);
pinMode(pinizq2,OUTPUT);
pinMode(pinEncoder, INPUT);
}

void loop() {
  if(digitalRead(pinEncoder) == LOW){
    encoder1 = true;
  }
  if(digitalRead(pinEncoder) == HIGH){
    encoder2 = true;
  }
  if(encoder1&&encoder2){
    pasos++;
    encoder1=false;
    encoder2=false;
    Serial.print("Pasos: ");
    Serial.println(pasos);
  }
  if(pasos>=40){
    pasos = 0;
    vueltas++;
    Serial.print("Vueltas: ");
    Serial.println(vueltas);
  }
  
  //Revisa si hay una conexion BT establecida
if (BT.available())
 {
    //Lee la instruccion enviada por el control
    cnal=BT.read();
    //Escribe en el puerto serial la instruccion recibida
    Serial.write(cnal);
    //Determina que accion llevar acabo segun la instruccion recibida
  switch (cnal)
  {
    case 'D'://Activa el giro de ambos motores hacia la izquierda
      digitalWrite(pinizq,HIGH);
      digitalWrite(pinder,LOW);
      digitalWrite(pinizq2,LOW);
      digitalWrite(pinder2,HIGH);
      Serial.println(" Izquierda ");
      break;
    case 'A'://Activa el giro de ambos motores hacia la derecha
      digitalWrite(pinder,HIGH);
      digitalWrite(pinizq,LOW);
      digitalWrite(pinder2,LOW);
      digitalWrite(pinizq2,HIGH);
      Serial.println(" Derecha ");
      break;
    case 'W'://Aumenta la velocidad de giro
      digitalWrite(pinizq,HIGH);
      digitalWrite(pinder,LOW);
      digitalWrite(pinizq2,HIGH);
      digitalWrite(pinder2,LOW);
      break;
    case 'V':
      if (velocidad<=204)
      {
        velocidad=velocidad+51;
        analogWrite(pinvel,velocidad);
        Serial.print(velocidad);
      }
      break;
    case 'S'://Disminuye la velocidad de giro
      digitalWrite(pinizq,LOW);
      digitalWrite(pinder,HIGH);
      digitalWrite(pinizq2,LOW);
      digitalWrite(pinder2,HIGH);
      break;
    case 'B':
      if (velocidad>=51)
      {
        velocidad=velocidad-51;
        analogWrite(pinvel,velocidad);
        Serial.print(velocidad);
      }
      break;
    case 'O'://Detiene los motores
      digitalWrite(pinizq,LOW);
      digitalWrite(pinder,LOW);
      digitalWrite(pinizq2,LOW);
      digitalWrite(pinder2,LOW);
      Serial.println(" Apagado ");
      break;
  }
  }
  if (Serial.available())
  {
    BT.write(Serial.read());
  }
}
