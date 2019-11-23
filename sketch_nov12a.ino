//#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <EEPROM.h>
// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "9e5faa99de7f4d35963c39e37c0e5e09";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "wifi chuy";
char pass[] = "huanying";


//valvula

byte Valve = 32;


/*
Arduino Water flow meter
YF- S201 Hall Effect Water Flow Sensor
Water Flow Sensor output processed to read in litres/hour
*/
byte sensorPin       = 14;    //interruptor en esp32
// The hall-effect flow sensor outputs approximately 4.5 pulses per second per
// litre/minute of flow.
float calibrationFactor = 7.5;
volatile byte pulseCount;  
float flowRate;
unsigned int flowMilliLitres;
double totalMilliLitres;
unsigned long oldTime;
unsigned long oldTime1;
unsigned long inicio1;
int i=0;



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


void setup()
{
 Serial.begin(9600);

  
  pinMode(Valve, OUTPUT); //set up valve
    digitalWrite(Valve, HIGH);   // turn the LED on (HIGH is the voltage level)

  
  // Set up the status LED line as an output
 
  pinMode(sensorPin, INPUT);
  digitalWrite(sensorPin, HIGH);

  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
 
  
  // The Hall-effect sensor is connected to pin 2 which uses interrupt 0.
  // Configured to trigger on a FALLING state change (transition from HIGH
  // state to LOW state)
  attachInterrupt(digitalPinToInterrupt(sensorPin), pulseCounter, FALLING);
  
   Blynk.begin(auth, ssid, pass);
   timer.setInterval(1000L, mostrar_lecturas);
   //imprime los datos en la app
   
   timeragua=timer1.setInterval(10000, alerta); 
   //este timer enviará una alerta cada vez que el tiempo 
   //especificado se detecte una lectura continua de agua
   
   timerflujo=timer1.setInterval(1000, calcular_flujo);
   //calcular caudal, factura, litros consumidos
}
   
   
void loop ()
{
   Blynk.run();
timer.run(); // Initiates BlynkTimer
timer1.run();
if(flowRate == 0){
  timer1.restartTimer(timeragua);
}
}
