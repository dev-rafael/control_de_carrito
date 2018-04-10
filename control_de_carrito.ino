#include <SoftwareSerial.h>
SoftwareSerial BT(10,9);
char cnal;                                        //Variable que guarda la instruccion enviada por el control
int pinEncoder = 8;                               //Pin donde se conecta el sensor del econder
float conversionCm = 0.5;                         //Factor de conversion de pasos a centimetros
float convercionGrados = 0.3;                     //Factor de conversion de pasos a grados
boolean encoder;                                  //Variable que acciona un incremento en los pasos

/* Variables que detectan un flanco */
boolean encoder1; 
boolean encoder2;
/* * * * * * * * * * * * * * * * *  */

boolean programacion = false;                     //Variable que activa la programacion de comandos
int contador = 0;                                 //Contador para clase de apoyo
char linea[23];                                   //Variable que almacena el string de comandos enviados por BT
char comandos[5];                                 //Arreglo que almacena los comandos a ejecutarse
int desplazamiento[5];                            //Arreglo que almacena las distancias a recorrer por cada comando
int numComand;                                    //Contador que guarda el numero de comandos a programar
int pasos = 0;                                    //Variable que almacena la cantidad de pasos detectados
int vueltas = 0;                                  //Variable que almacena la cantidad de vueltas detectadas
int pinizq=5;                                     //Pin de activacion izquierdo del primer puente H
int pinder=4;                                     //Pin de activacion derecho del primer puente H
int pinvel=3;                                     //Pin del enable, controla la velocidad
int velocidad=51;                                 //Variable que guarda la velocidad
int pinder2=7;                                    //Pin de activacion izquierdo del segundo puente H
int pinizq2=6;                                    //Pin de activacion derecho del segundo puente H

void setup() {

BT.begin(9600);                                   //Inicio de comunicacion Bluetooth
Serial.begin(9600);                               //Inicio de comunicacion serial


/* Declaracion de modo de operacion de los pines */
pinMode(pinder,OUTPUT);
pinMode(pinizq,OUTPUT);
pinMode(pinder2,OUTPUT);
pinMode(pinizq2,OUTPUT);
pinMode(pinEncoder, INPUT);
/* * * * * * * * * * * * * * * * * * * * * * */
}

void loop() {  
  cuentaPasos();
  
  if (BT.available())                             //Revisa si hay una conexion BT establecida
  {    
    cnal=BT.read();                               //Lee la instruccion enviada por el control
    
    /* Escribe en el puerto serial la instruccion recibida */
    Serial.print("Comando inicial: ");
    Serial.write(cnal);
    Serial.println(" fin.");
    /* * * * * * * * * * * * * * * * * * * * * * */

    /*
     * Revisa si el modo programacion está activado, de ser el caso almacena una serie de caracteres enviados
     * via BT en un arreglo de caracteres hasta recibir el caracter de finalizacion o quedarse sin espacio.
     * 
     * Una vez almacenadas todas las instrucciones las divide de manera util para que el comando de 
     * ejecucion las pueda leer
     */
    if(programacion){
      if(cnal!='.' && contador < 21){
        linea[contador] = cnal;
        contador++;
      }else{
        programacion = false;
        parsear();
      }
    }

    /* * * * * * * * * * * * * * * * * * * * * * */
    
    /*
     * Parte principal del modo control remote del carrito, revisa que el modo programacion esté desactivado, y
     * de ser el caso revisa el comando recibido para ejecutar una accion determinada. Las acciones disponibles
     * son:
     *  > Adelante
     *  > Atrás
     *  > Izquierda
     *  > Derecha
     *  > Aumentar velocidad
     *  > Reducir velocidad
     *  > Detenerse
     *  > Iniciar modo programación
     *  > Ejecutar secuencia guardada
     *  > Comando de prueba
     */
    if(!programacion){
      switch (cnal)
      {
        case 'E':
          /*
           * Comando para ejecutar secuencia guardada
           * 
           * Itera entre cada posicion de los arreglos de comandos y distancias para determinar la accion a
           * realizar y cuando detenerla. Los movimientos son:
           *      < en centimetros >
           *  > Adelante
           *  > Atras
           *      <   en  grados   >
           *  > Izquierda
           *  > Derecha
           */
          Serial.println("Modo ejecucion");
          for(int contExe=0; contExe<numComand; contExe++){
            Serial.print("Comando: ");
            Serial.println(comandos[contExe]);
            switch(comandos[contExe]){
              case 'W':
                adelante(desplazamiento[contExe]);
                break;
              case 'D':
                izquierda(desplazamiento[contExe]);
                break;
              case 'S':
                atras(desplazamiento[contExe]);
                break;
              case 'A':
                derecha(desplazamiento[contExe]);
                break;
              default:
                if(Serial.available()){
                  BT.write("Comando no reconocido");
                }
                break;
            }
            delay(100);
          }
          break;
        case 'F':                                 //Activar modo de programacion
          programacion = true;
          break;
        case 'A':                                 //Activa el giro de ambos motores hacia la izquierda
          derecha(0);
          Serial.println(" Derecha ");
          break;
        case 'D':                                 //Activa el giro de ambos motores hacia la derecha
          izquierda(0);
          Serial.println(" Izquierda ");
          break;
        case 'W':                                 //Activar el giro de ambos motores hacia adelante
          adelante(0);
          break;
        case 'V':                                 //Aumenta la velocidad de giro
          if (velocidad<=204)
          {
            velocidad=velocidad+51;
            analogWrite(pinvel,velocidad);
            Serial.print(velocidad);
          }
          break;
        case 'S':                                 //Activar el giro de ambos motores hacia atras
          atras(0);
          break;
        case 'B':                                 //Disminuye la velocidad de giro
          if (velocidad>=51)
          {
            velocidad=velocidad-51;
            analogWrite(pinvel,velocidad);
            Serial.print(velocidad);
          }
          break;
        case 'N':                                 //Comando de prueba
          izquierda(360);
          break;
        case 'O':                                 //Detiene los motores
          digitalWrite(pinizq,LOW);
          digitalWrite(pinder,LOW);
          digitalWrite(pinizq2,LOW);
          digitalWrite(pinder2,LOW);
          Serial.println(" Apagado ");
          break;
      }
    }
    /* * * * * * * * * * * * * * * * * * * * * * */
  }

  /* Escribe en la terminal BT lo que recibe del puerto serial */
  if (Serial.available())
  {
    BT.write(Serial.read());
  }
  /* * * * * * * * * * * * * * * * * * * * * * */
}

/* Función de apoyo que registra la cantidad de pulsos detectados por el encoder */
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
/* * * * * * * * * * * * * * * * * * * * * * */


/* 
 *  Función de apoyo que recibes un numero entero igual a una distancia en centimetros o grados 
 *  y regresan su equivalente en pasos
 */
int cmApasos(int cm)
{
  return (cm*conversionCm);
}

int gradosApasos(int grados){
  return (grados*convercionGrados);
}
/* * * * * * * * * * * * * * * * * * * * * * */

/*
 * Método de apoyo que distribuye los comandos recibidos via BT en dos arreglos utilizables por
 * el programa para leer cada comando registrado y asociado a una distancia.
 * 
 * Primero el método registra el primer caracter recibido como comando, los siguientes caracteres
 * hasta detectar una coma (,) son guardados en un arreglo temporal y luego transformados en un
 * numero entero, el cual a su vez es guardado en la misma posición que el comando previamente
 * registrado, pero en otro arreglo.
 * 
 * El ciclo continua hasta llegar al ultimo caracter recibido.
 */
void parsear (){
  int i;
  int j=0;
  Serial.print(contador);
  Serial.println("Entra en parseo");
  for(i=0;i<contador;i++){
    Serial.print(linea[i]);
  }
  Serial.println(" fin de string de comandos.");
  i=0;
  while(j<contador){
    comandos[i]=linea[j];
    char numero [6]={};
    int contNum = 0;
    while(linea[j+1] != ',' && (j+1)<contador){
      j++;
      /*
      Serial.print(contNum);
      Serial.print(linea[j]);
      */
      numero[contNum] = linea[j];
      //Serial.println(numero[contNum]);
      contNum++;
    }
    //numero[contNum]='\0';
    j=j+2;
    desplazamiento[i]=atoi(numero);
    i++;
    
  }

  numComand=i;
  contador=0;
  
  
  int contDebug;
  Serial.print("Comandos: ");
  for(contDebug = 0; contDebug < i; contDebug++){
    Serial.print(comandos[contDebug]);
    Serial.print(',');
  }
  Serial.println(".");
  Serial.print("Desplazamientos: ");
  for(contDebug = 0; contDebug < i; contDebug++){
    Serial.print(desplazamiento[contDebug]);
    Serial.print(',');
  }
  Serial.println(".");
}
/* * * * * * * * * * * * * * * * * * * * * * */

/*
 * Funciones que activan los motores hacia una de las 4 direcciones 
 * 
 *  > Adelante
 *  > Atras
 *  > Izquierda
 *  > Derecha
 * 
 *  Si el argumento es un numero positivo entre 1 y 9999 los motores se desactivarán
 *  una vez que el encoder haya detectado que pasó la distancia especificada
 *  
 *  Si el argumento es 0, los motores no se detienen automaticamente
 * 
 */
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
/* * * * * * * * * * * * * * * * * * * * * * */

