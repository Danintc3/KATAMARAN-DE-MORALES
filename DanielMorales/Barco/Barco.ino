#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <SPI.h>
#include <ESP32Servo.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;
Servo mot1;
Servo mot2;
//Declaremos los pines CE y el CSN
const int pinCE = 4;
const int pinCSN = 5;
RF24 radio(pinCE,pinCSN);
int Giroscopio = 0;
 
//Variable con la dirección del canal que se va a leer
byte  direccion1[5] = { 'O','R', 'I','O','N'};
byte  direccion2[5] = { 'D','A', 'N','I','F'};
byte DatosIn[5];
float DatosOut[7];
byte DatosInModAut[3];

// declaramos las funciones adelante, atras, derecha e izquierda
void Adelante(){
  digitalWrite(27, 1);
  digitalWrite(26, 0);
  digitalWrite(25, 0);
  digitalWrite(33, 1);
  //delay(2000);
}
void Atras(){
  digitalWrite(27, 0);
  digitalWrite(26, 1);
  digitalWrite(25, 1);
  digitalWrite(33, 0);
  //delay(2000);
}
void Derecha(){
  digitalWrite(27, 0);
  digitalWrite(26, 1);
  digitalWrite(25, 0);
  digitalWrite(33, 1);
  //delay(2000);
}
void Izquierda(){
  digitalWrite(27, 1);
  digitalWrite(26, 0);
  digitalWrite(25, 1);
  digitalWrite(33, 0);
  //delay(2000);
}



void setup(void) {
ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
  mot1.setPeriodHertz(50);
  mot2.setPeriodHertz(50);

  mot1.attach(12, 1000, 2000);
  mot2.attach(14, 1000, 2000);

  pinMode(27, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(25, OUTPUT);
  pinMode(33, OUTPUT);
 
 
 // configurampos para 115200 baudios 
 Serial.begin(115200); 

  
    //empezamos a escuchar por el canal
  radio.startListening();

 while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);
  // se configura el radio
  radio.setRetries(15,15);      // Maximos reintentos
  //radio.setPayloadSize(8);    // Reduce el payload de 32 si tienes problemas
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();
  radio.openWritingPipe(direccion2);
  radio.openReadingPipe(1,direccion1);

}

void loop() {
  if ( radio.available() )
 {    
     //Leemos los datos y los guardamos en la variable datos[]
     radio.read(DatosIn,sizeof(DatosIn));
     
     //reportamos por el puerto serial los datos recibidos
     Serial.print(" mot1= " );
     Serial.print(DatosIn[0]);
     Serial.print(" mot2= ");
     Serial.print(DatosIn[1]);
     Serial.print(" Direccion= " );
     Serial.print(DatosIn[2]);
     Serial.print(" Bloqueo");
     Serial.print(DatosIn[3]);
     Serial.print(" ModAut");
     Serial.println(DatosIn[4]);
     
 }
 else
 {
     Serial.println("No hay datos de radio disponibles");
 }
 if(DatosIn[2] == 0){
    Adelante();
  }
  if(DatosIn[2] == 1){
    Atras();
  }
  if(DatosIn[2] == 2){
    Izquierda();
  }
  if(DatosIn[2] == 3){
    Derecha();
  }

  
  if(DatosIn[3] == 1 ){
    mot1.write(0);
    mot2.write(0); 
  }
  else{
    mot1.write(DatosIn[0]);
    mot2.write(DatosIn[1]);    
  }
  if(DatosIn[4] == 1){
    
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    /* Print out the values */
    Serial.print("Acceleration X: ");
    Serial.print(a.acceleration.x);
    Serial.print(", Y: ");
    Serial.print(a.acceleration.y);
    Serial.print(", Z: ");
    Serial.print(a.acceleration.z);
    Serial.println(" m/s^2");

    Serial.print("Rotation X: ");
    Serial.print(g.gyro.x);
    Serial.print(", Y: ");
    Serial.print(g.gyro.y);
    Serial.print(", Z: ");
    Serial.print(g.gyro.z);
    Serial.println(" rad/s");

    Serial.print("Temperature: ");
    Serial.print(temp.temperature);
    Serial.println(" degC");

    Serial.println("");
    Serial.println("Enviando datos 99");
    DatosOut[0]= a.acceleration.x;
    DatosOut[1]= a.acceleration.y;
    DatosOut[2]= a.acceleration.z;
    DatosOut[3]= g.gyro.x;
    DatosOut[4]= g.gyro.y;
    DatosOut[5]= g.gyro.z;
    DatosOut[6]= temp.temperature;
    radio.stopListening();
    // envia la respuesta
    radio.write(DatosOut, sizeof(DatosOut));
    radio.startListening();
  }






}
