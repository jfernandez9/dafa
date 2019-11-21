/*************************************************************
  This is a DEMO. You can use it only for development and testing.
  You should open Setting.h and modify General options.

  If you would like to add these features to your product,
  please contact Blynk for Businesses:

                   http://www.blynk.io/

 *************************************************************/

//#define USE_WROVER_BOARD
#define USE_CUSTOM_BOARD          // See "Custom board configuration" in Settings.h

#define APP_DEBUG        // Comment this out to disable debug prints

#define BLYNK_PRINT Serial

#include "BlynkProvisioning.h"

byte Valve = 32;
byte sensorPin       = 14;   //flujo de agua
// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
float calibrationFactor = 7.5;
volatile byte pulseCount;  
float flowRate;
unsigned int flowMilliLitres;
double totalMilliLitres;

 float factura;
double total;
float precio_mililitro=0.00012824;
unsigned long StartTime = 0;

int timeragua;
int timerflujo;



BlynkTimer timer;  
BlynkTimer timer1;


BLYNK_CONNECTED() {
    Blynk.syncVirtual(V0, V6);
    
}

//descargar los datos anteriores

BLYNK_WRITE(V0) {
  int value0 = param.asInt();
}

BLYNK_WRITE(V6) {
int value0 = param.asInt();
}

void pulseCounter()
{
  // Increment the pulse counter
  pulseCount++;
}


void alerta(){
Blynk.notify("Alerta: ¡Posible fuga!");
Serial.println("alertaaaaaaa");
}

void mostrar_lecturas() {
    
  Blynk.virtualWrite(V4, flowRate);
   Blynk.virtualWrite(V6, factura);
     Blynk.virtualWrite(V0, totalMilliLitres/1000);
Serial.println(total);


}

void calcular_flujo(){ // esta funcion calcula el caudal, litros consumidos y precio del recibo
  // Disable the interrupt while calculating flow rate and sending the value to
    // the host
    detachInterrupt(digitalPinToInterrupt(sensorPin));
      flowRate = (pulseCount ) / calibrationFactor;
      flowMilliLitres = (flowRate / 60) * 1000;
    factura=precio_mililitro*totalMilliLitres;
    // Add the millilitres passed in this second to the cumulative total
    totalMilliLitres += flowMilliLitres;
    // Reset the pulse counter so we can start incrementing again
    pulseCount = 0;
    
    // Enable the interrupt again now that we've finished sending output
    attachInterrupt(digitalPinToInterrupt(sensorPin), pulseCounter, FALLING);

}

void setup() {
  delay(500);
  Serial.begin(115200);

 pinMode(Valve, OUTPUT); //set up valve
    digitalWrite(Valve, HIGH);   
  pinMode(sensorPin, INPUT); //activar sensor 
  digitalWrite(sensorPin, HIGH);
  
  attachInterrupt(digitalPinToInterrupt(sensorPin), pulseCounter, FALLING);
  
   
   timer.setInterval(1000L, mostrar_lecturas);
   //imprime los datos en la app
   
   timeragua=timer1.setInterval(10000, alerta); 
   //este timer enviará una alerta cada vez que el tiempo 
   //especificado se detecte una lectura continua de agua
   
   timerflujo=timer1.setInterval(1000, calcular_flujo);
   //calcular caudal, factura, litros consumidos

   

  BlynkProvisioning.begin();

}

void loop() {
  // This handles the network and cloud connection
  BlynkProvisioning.run();

timer.run(); // Initiates BlynkTimer
timer1.run();
if(flowRate == 0){
  timer1.restartTimer(timeragua);
}
}
