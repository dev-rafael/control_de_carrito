#include <SoftwareSerial.h>
SoftwareSerial BT(10,9);
char cnal;  //Variable que guarda la instruccion enviada por el control
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
}

void loop() {
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
    case 'A'://Activa el giro de ambos motores hacia la izquierda
      digitalWrite(pinizq,HIGH);
      digitalWrite(pinder,LOW);
      digitalWrite(pinizq2,LOW);
      digitalWrite(pinder2,HIGH);
      Serial.println(" Izquierda ");
      break;
    case 'D'://Activa el giro de ambos motores hacia la derecha
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
      /*
      if (velocidad<=204)
      {
        velocidad=velocidad+51;
        analogWrite(pinvel,velocidad);
        Serial.print(velocidad);
      }
      */
      break;
    case 'S'://Disminuye la velocidad de giro
      digitalWrite(pinizq,LOW);
      digitalWrite(pinder,HIGH);
      digitalWrite(pinizq2,LOW);
      digitalWrite(pinder2,HIGH);
      /*
      if (velocidad>=51)
      {
        velocidad=velocidad-51;
        analogWrite(pinvel,velocidad);
        Serial.print(velocidad);
        
      }
      */
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
