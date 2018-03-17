#include <SoftwareSerial.h>
SoftwareSerial BT(10,9);
char cnal;  //Variable que guarda la instruccion enviada por el control
char comando;
int pinEncoder = 8;//Pin donde se conecta el sensor del econder
float conversionCm = 1.55;
float convercionGrados = 2.78;
boolean encoder;
boolean encoder1;
boolean encoder2;
boolean programacion = false;
int contador = 0;
char string[21];
char comandos[5];
int desplazamiento[5];
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
  cuentaPasos();
  
  //Revisa si hay una conexion BT establecida
  if (BT.available())
  {
    //Lee la instruccion enviada por el control
    cnal=BT.read();
    //Escribe en el puerto serial la instruccion recibida
    Serial.write(cnal);
    
    if(programacion){
      if(cnal!='.' && contador < 21){
        comandos[contador] = cnal;
      }else{
        programacion = false;
        parsear(string);
      }
    }
    
    //Determina que accion llevar acabo segun la instruccion recibida
    switch (cnal)
    {
      case 'F':        
        programacion = true;
        break;
      case 'D'://Activa el giro de ambos motores hacia la izquierda
        digitalWrite(pinizq,HIGH);
        digitalWrite(pinder,LOW);
        digitalWrite(pinizq2,LOW);
        digitalWrite(pinder2,HIGH);
        Serial.println(" Derecha ");
        break;
      case 'A'://Activa el giro de ambos motores hacia la derecha
        izquierda(0);
        Serial.println(" Izquierda ");
        break;
      case 'W'://Aumenta la velocidad de giro
        adelante(0);
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
        atras(0);
        break;
      case 'B':
        if (velocidad>=51)
        {
          velocidad=velocidad-51;
          analogWrite(pinvel,velocidad);
          Serial.print(velocidad);
        }
        break;
      case 'N':      
        izquierda(360);
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

void cuentaPasos(){
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
  }/*
  if((pasos%40==0) && (pasos != 0)){
    vueltas++;
    Serial.print("Vueltas: ");
    Serial.println(vueltas);
  }*/
}

int cmApasos(int cm)
{
  return (cm*conversionCm);
}

int gradosApasos(int grados){
  return (grados*convercionGrados);
}

void parsear (char string[21]){
  int j = 0;
  for(int i=0;i<contador;i++){
    comandos[i]=string[j];
  }
}

void adelante (int distancia){
  if(distancia != 0){
    digitalWrite(pinizq,HIGH);
    digitalWrite(pinder,LOW);
    digitalWrite(pinizq2,HIGH);
    digitalWrite(pinder2,LOW);
    pasos = 0;
    Serial.println(cmApasos(distancia));
    while(pasos <= cmApasos(distancia)){
      cuentaPasos();
      Serial.print("Pasos: ");
      Serial.println(pasos);
    }
    digitalWrite(pinizq,LOW);
    digitalWrite(pinder,LOW);
    digitalWrite(pinizq2,LOW);
    digitalWrite(pinder2,LOW);
  }else{
    digitalWrite(pinizq,HIGH);
    digitalWrite(pinder,LOW);
    digitalWrite(pinizq2,HIGH);
    digitalWrite(pinder2,LOW);
  }
}

void atras (int distancia){
  if(distancia != 0){
    digitalWrite(pinizq,LOW);
    digitalWrite(pinder,HIGH);
    digitalWrite(pinizq2,LOW);
    digitalWrite(pinder2,HIGH);
    pasos = 0;
    Serial.println(cmApasos(distancia));
    while(pasos <= cmApasos(distancia)){
      cuentaPasos();
      Serial.print("Pasos: ");
      Serial.println(pasos);
    }
    digitalWrite(pinizq,LOW);
    digitalWrite(pinder,LOW);
    digitalWrite(pinizq2,LOW);
    digitalWrite(pinder2,LOW);
  }else{
    digitalWrite(pinizq,LOW);
    digitalWrite(pinder,HIGH);
    digitalWrite(pinizq2,LOW);
    digitalWrite(pinder2,HIGH);
  }
}

void izquierda (int grados){
  if(grados != 0){
    digitalWrite(pinder,HIGH);
    digitalWrite(pinizq,LOW);
    digitalWrite(pinder2,LOW);
    digitalWrite(pinizq2,HIGH);
    pasos = 0;
    Serial.println(gradosApasos(grados));
    while(pasos <= gradosApasos(grados)){
      cuentaPasos();
      Serial.print("Pasos: ");
      Serial.println(pasos);
    }
    digitalWrite(pinizq,LOW);
    digitalWrite(pinder,LOW);
    digitalWrite(pinizq2,LOW);
    digitalWrite(pinder2,LOW);
  }else{
    digitalWrite(pinder,HIGH);
      digitalWrite(pinizq,LOW);
      digitalWrite(pinder2,LOW);
      digitalWrite(pinizq2,HIGH);
  }
}

void derecha (int grados){
  if(grados != 0){
    digitalWrite(pinizq,HIGH);
    digitalWrite(pinder,LOW);
    digitalWrite(pinizq2,LOW);
    digitalWrite(pinder2,HIGH);
    pasos = 0;
    Serial.println(gradosApasos(grados));
    while(pasos <= gradosApasos(grados)){
      cuentaPasos();
      Serial.print("Pasos: ");
      Serial.println(pasos);
    }
    digitalWrite(pinizq,LOW);
    digitalWrite(pinder,LOW);
    digitalWrite(pinizq2,LOW);
    digitalWrite(pinder2,LOW);
  }else{
    digitalWrite(pinizq,HIGH);
    digitalWrite(pinder,LOW);
    digitalWrite(pinizq2,LOW);
    digitalWrite(pinder2,HIGH);
  }
}


