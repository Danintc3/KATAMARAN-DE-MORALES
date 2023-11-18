#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>
// declaro los pines ce y csn del nrf24
const int pinCE = 9;
const int pinCSN = 10;
RF24 radio(pinCE,pinCSN); 
int joyX = 0;
int joyY = 0;
int mot1 = 0;
int mot2 = 0;
int estado = 0; 
int ModAut = 0;
int Bloqueo = 0;
int Direccion = 0;
int JoysPul = 8;
int PulA = 2;
int PulB = 3;
int PulC = 4;
int PulD = 5;
// contraseña del canal que se usará
 // tamaño de los datos enviados
byte DatosOut[5];  // "Datos enviados"
float DatosIn[7];
byte  direccion1[5] = { 'O','R', 'I','O','N'};
byte  direccion2[5] = { 'D','A', 'N','I','F'};
//***************************************************************
//              Distribucion del control
//***************************************************************
/*
BOTON J --> Pin 8
BOTON E --> Pin 6
BOTON F --> PIN 7
BOTON A --> Pin2
BOTON B --> Pin3
BOTON C --> Pin4
BOTON D --> Pin5
//***************************************************************/
void setup() {
  Serial.begin(115200);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(8, INPUT);
  pinMode(7, INPUT);
  pinMode(6, INPUT);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);
  pinMode(5, INPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.setRetries(15,15);      // Maximos reintentos
  //radio.setPayloadSize(8);    // Reduce el payload de 32 si tienes problemas
  radio.openWritingPipe(direccion1);
  radio.openReadingPipe(1,direccion2);
}

void loop() {
  if (!ModAut){  //Leer joysTicks

    //para de escuchar
    radio.stopListening(); 

    DatosOut[0] = mot1;
    DatosOut[1] = mot2;
    joyX = analogRead(A0); //paso 1 leer joystick en X
    joyY = analogRead(A1); //paso 2 leer joystick en Y
    joyX = map(joyX,0,1023,-180,180); // paso 3 mapeamos en X para poderlo tener en 180 a -180
    joyY = map(joyY,0,1023,-180,180); // paso 4 mapeamos en Y para poderlo tener en 180 a -180

    
    //aqui se preguntan los estados dependiendo del joystick
    if (joyX > -100 and joyX < 100 ){
      estado = 1;
    }
    else if (joyX < -100){
      estado = 2;
    }
    else if (joyX > 100){
      estado = 3;
    }
    else if (joyX == 0 and joyY < 5 ){
      estado = 4;
    }

    if (estado == 1){

      if(joyY < 0){

        joyY = 0;
      }
      mot1 = joyY;
      mot2 = joyY;
    }
    else if(estado == 2){
      if(joyY < 0){

        joyY = 0;
      }
      mot1 = joyY;
      mot2 = joyX *-1;
    }
    else if(estado == 3){
      if(joyY < 0){

        joyY = 0;
      }
      mot1 = joyX;
      mot2 = joyY;
    }
    else if(estado == 4){
      if(joyY < 0){
        joyY = 0;
      }
      mot1 = 0;
      mot2 = 0; 
    }

    //Leer los botones A,B,C Y D ADELANTE,ATRAS,DERECHA E IZQUIERDA 
    if (digitalRead(2) == 0){
      Direccion = 0;
    } 
    else if (digitalRead(4) == 0){
      Direccion = 1;
    }
    else if (digitalRead(5) == 0){
      Direccion = 2;
    }
    else if (digitalRead(3) == 0){
      Direccion = 3;
    }
    Serial.print("mot1: "); Serial.print(mot1);Serial.print(" mot2: "); Serial.print(mot2); // imprimimos datos del joystick en X y Y
    Serial.print("  Bloqueo: "); Serial.print(Bloqueo);Serial.print(" Direccion: "); Serial.print(Direccion); Serial.print(" Modo aut: "); Serial.println(ModAut);
  }

  else if (ModAut){
    digitalWrite(A3, HIGH);
    //iniciar la escucha
    radio.startListening();
    bool timeout = false;
    
    unsigned long started_waiting_at = millis();
    while ( ! radio.available() && ! timeout )       // Esperamos 200ms
          if (millis() - started_waiting_at > 200 )
              timeout = true;
    if ( timeout )
        Serial.println("Error, No ha habido respuesta a tiempo");
    else
    {   //Leemos los datos y los guardamos en la variable datos[]
        radio.read(DatosIn,sizeof(DatosIn));
        
        //reportamos por el puerto serial los datos recibidos

        Serial.print(" ModAut");
        Serial.print("Acceleration X: ");
        Serial.print(DatosIn[0]);
        Serial.print(", Y: ");
        Serial.print(DatosIn[1]);
        Serial.print(", Z: ");
        Serial.print(DatosIn[2]);
        Serial.println(" m/s^2");

        Serial.print("Rotation X: ");
        Serial.print(DatosIn[3]);
        Serial.print(", Y: ");
        Serial.print(DatosIn[4]);
        Serial.print(", Z: ");
        Serial.print(DatosIn[5]);
        Serial.println(" rad/s");

        Serial.print("Temperature: ");
        Serial.print(DatosIn[6]);
        Serial.println(" degC");

        Serial.println("");
    }
    
    //se lee el serial y se recibi lo que python envia
    if (Serial.available() > 0 ){
      String data = Serial.readStringUntil('!');
      int separador1 = data.indexOf('*');
      int separador2 = data.indexOf('#');
      int separador3 = data.indexOf('%');
      int dato1= data.substring(0, separador1).toInt();
      int dato2= data.substring(separador1 + 1 , separador2).toInt();
      int dato3= data.substring(separador2 + 1 , separador3).toInt();

      mot1=  dato1;
      mot2=  dato2;
      Direccion = dato3;
    
    }
  }
  
  //Leer Tanto Bloqueo como desbloqueo que son el mismo
  if (digitalRead(7) == 0){
    ModAut = 0;
  }
  else if (digitalRead(6) == 0){
    ModAut = 1;
  }

  //Preguntamos sobre el estado de bloque se lee debido a que se necesita saber si anteriormente se presiono
  if (digitalRead(8) == 0 and Bloqueo == 1){
    Bloqueo = 0;
    delay(300);
  }
  else if (digitalRead(8) == 0 and Bloqueo == 0){
    Bloqueo = 1;
    delay(300); 
  }

  // preguntamos por los estados de bloque modo automatico y si hay coneccion o no para los testigos
  if (Bloqueo == 1){
    digitalWrite(A2, LOW);
  }
  else if (Bloqueo == 0){
    digitalWrite(A2, HIGH);
  }

  radio.stopListening(); // paro de escuchar


  //Cargar los datos
  DatosOut[0]= mot1;
  DatosOut[1]= mot2;
  DatosOut[2]= Direccion;
  DatosOut[3]= Bloqueo;
  DatosOut[4]= ModAut;

  //finalmente enviamos todos los datos
  bool ok = radio.write(DatosOut, sizeof(DatosOut));

  if(!ok){
    Serial.println("no se ha podido enviar");


      digitalWrite(A4, LOW);

  }
  else{
    digitalWrite(A4, HIGH);
  }


  





}
