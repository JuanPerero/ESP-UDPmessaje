#include <ESP8266WiFi.h>
//#include <ESP8266WebServer.h>
//#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WiFiUDP.h>
#include <EEPROM.h>
#include "webconfig.h"
#include "UDP.hpp"
#include "TimerInterrupt_Generic.h"

/// POST IT


//- Posible Reconeccion de puerto UDP
//- Rutina de configuracion hacia AP con pin de entrada
//- No comprobas mala configuracion de contraseña
//- Eventos de reconeccion de wifi wifi



#if !defined( ESP8266 )
  #error This code is intended to run on the ESP32 platform! Please check your Tools->Board setting.
  // Por ahora la incompatibilidad solo se daría en el timer
  // hay que cambiar el servidor web de 8266
#endif


//############ Configuraciones y set de variables para el Timer
// These define's must be placed at the beginning before #include "TimerInterrupt_Generic.h"
// _TIMERINTERRUPT_LOGLEVEL_ from 0 to 4
// Don't define _TIMERINTERRUPT_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
#define _TIMERINTERRUPT_LOGLEVEL_ 0
#include "TimerInterrupt_Generic.h"
#define TIMER_INTERVAL_MS        10000
// Init ESP8266 timer 1
ESP8266Timer ITimer;

/*
//- ESP32
// These define's must be placed at the beginning before #include "_TIMERINTERRUPT_LOGLEVEL_.h"
// _TIMERINTERRUPT_LOGLEVEL_ from 0 to 4
#define _TIMERINTERRUPT_LOGLEVEL_ 0
#include "TimerInterrupt_Generic.h"
// Init ESP32 timer 0 and 1
#define TIMER1_INTERVAL_MS        5000
ESP32Timer ITimer0(0);
*/






//Definicion de sectores de memoria
#define MEMWIFINAME 300
#define MEMWIFIPASS 428
#define MEMCOUNTIPS 100
#define MEMINITIPS 512

#define NOMBRERED "esp-shutdown"

//##############  Uso de UDP
boolean ConnectUDP();
void SendUDP_ACK();
void SendUDP_Packet(String content);
void GetUDP_Packet(bool sendACK = true);


//##############  MEMORIA EEPROM
void writeString(char add,String data);
String read_String(char add);
int readInt(char add);
void writeInt(char add,int data);
void writeString(char add,String data);


//#############  Rutinas de inicio
void loaddata();
bool conectowifi();
void initAPwebserver();
bool initudp();
void initdetection();

//############# AP y serv. Web Conf. y Handlers
void handleRoot(AsyncWebServerRequest *request);
void iprefresh(AsyncWebServerRequest *request);
void handlewificonf(AsyncWebServerRequest *request);
void handlelimpiarips(AsyncWebServerRequest *request);

//#############  VARIABLES GLOBALES
//ESP8266WebServer server(80);
AsyncWebServer server(80);
String ssid;
String password;
bool reconfwifi=false;
int Nips=0;
byte* listaips;


void borrarACTSERVER();
WiFiEventHandler gotIpEventHandler;
//=======================================================================
//  SETUP Y LOOP
//=======================================================================

void setup(){
  EEPROM.begin(1024);
  Serial.begin(115200);
  // Leer datos de memoria
  loaddata();
  gotIpEventHandler = WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP& event){      
              Serial.print("\nNueva IP: ");
              Serial.print(WiFi.localIP());}); 
              
  // Iniciar conectividad a la red
  if(conectowifi()){
      // Rutina de inicializacion de captura de evento
      initdetection();
  
      borrarACTSERVER();
      // Rutina de inicializacion de UDP
      while(!initudp()){    }
    }
  // De no haber station Abrir web para configuracion
  else { //Evaluacion de configuracion frente a activacion de pin
      //Rutina de inicializacion WEB
      //hadles para saves de lso datos cargados
      //Mantenimiento hasta reboot del sistema

      Serial.println("Uso por AP");
      initAPwebserver();
     
    }
}


bool flagaux=false;
void loop() {
    // CONECTOWIFI NO FUNCIONA CON EL HANDLER DE LA WEB
    if(flagaux==true){
        Serial.println("Conection loop");
        conectowifi();
        Serial.println("Salida");
        flagaux=false;
      }
}


//=======================================================================
//  Rutinas de inicio
//=======================================================================


void loaddata(){
  // Lectura de Datos de wifi
  ssid = read_String(MEMWIFINAME);
  password = read_String(MEMWIFIPASS);
  // Lectuda de cuantas IP se tienen que leer
  int N = readInt(MEMCOUNTIPS);
  Nips = N;
  // Lectura de datos de IP
  byte byte1,byte2,byte3,byte4;
  listaips = (byte*) malloc(sizeof(byte)*4*N); 
  for(int i=0;i<N;i++){
    EEPROM.get(MEMINITIPS+i*4,byte1);
    EEPROM.get(MEMINITIPS+i*4+1,byte2);
    EEPROM.get(MEMINITIPS+i*4+2,byte3);
    EEPROM.get(MEMINITIPS+i*4+3,byte4);
    listaips[i*4] = byte1;
    listaips[i*4+1] = byte2;
    listaips[i*4+2] = byte3;
    listaips[i*4+3] = byte4;
    Serial.println("Byte:");
    Serial.print(byte1);
    Serial.print(byte2);
    Serial.print(byte3);
    Serial.print(byte4);
    
    }
}

bool conectowifi(){
  WiFi.mode(WIFI_STA);
  int n = WiFi.scanNetworks(false);
  bool flagexist=false; 
  if(n>0){
    
      for(int i=0;i<n;i++){
        if(WiFi.SSID(i)==ssid){
          i=n;
          flagexist=true;
          WiFi.begin(ssid,password,false);
          while (WiFi.status() != WL_CONNECTED){
              delay(500);
              Serial.print(".");
            }
          if(reconfwifi){
            writeString(MEMWIFINAME,ssid);
            writeString(MEMWIFIPASS,password);
          }
        }
      }
      if(flagexist==false){
        WiFi.disconnect();    
        Serial.print("Fail reboot el aparato");
        }
   }
  WiFi.scanDelete();
  return flagexist;
}



void IRAM_ATTR TimerHandler()
{  
  SendUDP_Packet("SHUT");
  //SendUDP_ACK();// Cambiar el ip destino si se quiere usar
}


void initdetection(){
  if (ITimer.attachInterruptInterval(TIMER_INTERVAL_MS * 1000, TimerHandler))
  {
   Serial.print(F("Starting ITimer OK, millis() = "));
  }
  else
    Serial.println(F("Can't set ITimer correctly. Select another freq. or interval"));
}



//=======================================================================
//  Funciones de uso para UDP
//=======================================================================
bool initudp(){
  // Adaptacion de connectUDP
   while(UDP.begin(localPort)!= 1) 
   {
      Serial.println("Connection udp failed");
      delay(5000);
   }
   Serial.println("UDP successful");
   return true;
}

boolean ConnectUDP() {
   Serial.println();
   Serial.println("Starting UDP");

   // in UDP error, block execution
   if (UDP.begin(localPort) != 1) 
   {
      Serial.println("Connection failed");
      while (true) { delay(1000); } 
   }

   Serial.println("UDP successful");
}

void SendUDP_ACK()
{  //UDP.remoteIP()  es para responder
   UDP.beginPacket(UDP.remoteIP(), remotePort);
   UDP.write("ACK");
   UDP.endPacket();
}

void SendUDP_Packet(String content)
{
  for(int i=0;i<Nips;i++){
   //UDP.beginPacket(UDP.remoteIP(), remotePort);
   IPAddress aux(listaips[i*4],listaips[i*4+1],listaips[i*4+2],listaips[i*4+3]);
   UDP.beginPacket(aux, remotePort);
   UDP.write(content.c_str());
   UDP.endPacket();
  }
}

//void GetUDP_Packet(bool sendACK = true)
void GetUDP_Packet(bool sendACK)
{
   int packetSize = UDP.parsePacket();
   if (packetSize)
   {
      // read the packet into packetBufffer
      UDP.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);

      Serial.println();
      Serial.print("Received packet of size ");
      Serial.print(packetSize);
      Serial.print(" from ");
      Serial.print(UDP.remoteIP());
      Serial.print(":");
      Serial.println(UDP.remotePort());
      Serial.print("Payload: ");
      Serial.write((uint8_t*)packetBuffer, (size_t)packetSize);
      Serial.println();
      ProcessPacket(String(packetBuffer));

      //// send a reply, to the IP address and port that sent us the packet we received
      if(sendACK) SendUDP_ACK();
   }
   //delay(10);
}






//=======================================================================
//                    MEMORIA EEPROM
//=======================================================================
void writeInt(char add,int data)
{
  //int _size = sizeof(data);
  //int i;
  //for(i=0;i<_size;i++)
  //{  EEPROM.write(add+i,data[i]);  }
  
  EEPROM.write(add,byte(data));
  EEPROM.commit();
 //EEPROM.update();


 // EEPROM.put(add,data);
 //EEPROM.commit();
}
int readInt(char add)
{
  int dat;
  EEPROM.get(add,dat);
  return dat;
}

void writeString(char add,String data)
{
  int _size = data.length();
  int i;
  for(i=0;i<_size;i++)
  {
    EEPROM.write(add+i,data[i]);
  }
  EEPROM.write(add+_size,'\0');   //Add termination null character for String Data
  EEPROM.commit();
}


String read_String(char add)
{
  int i;
  char data[128]; //Max 127 Bytes
  int len=0;
  unsigned char k;
  k=EEPROM.read(add);
  while(k != '\0' && len<127)   //Read until null character
  {    
    k=EEPROM.read(add+len);
    data[len]=k;
    len++;
  }
  data[len]='\0';
  return String(data);
}


void saveipslist(){
  writeInt(MEMCOUNTIPS,Nips);
  for(int i=0;i<Nips;i++){
     EEPROM.write(MEMINITIPS+i*4,  listaips[i*4]);
     EEPROM.write(MEMINITIPS+i*4+1,listaips[i*4+1]);
     EEPROM.write(MEMINITIPS+i*4+2,listaips[i*4+2]);
     EEPROM.write(MEMINITIPS+i*4+3,listaips[i*4+3]);
     EEPROM.commit();
    }
 }






//=======================================================================
//              Funciones de AP y servidor Web para Configuracion
//=======================================================================

void initAPwebserver(){
   WiFi.mode(WIFI_AP);
   //WiFi.softAPConfig(direccionAP,direccionAP,SUBNET);
   WiFi.softAP(NOMBRERED);

   //server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

   server.on("/", HTTP_GET, handleRoot);
   server.on("/refresh", iprefresh);
   server.on("/WCONF", handlewificonf);
   server.on("/getdata", handlegetdata);
   server.on("/clear", handlelimpiarips);
   server.onNotFound(handleRoot);
   
   //server.onNotFound(handleRoot)
   //server.on("/", handleRoot);  //Associate handler function to path
   server.begin();  
   Serial.println("HTTP server started");
}



void borrarACTSERVER(){
   //server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

   server.on("/", HTTP_GET, handleRoot);
   server.on("/refresh", iprefresh);
   server.on("/WCONF", handlewificonf);
   server.on("/getdata", handlegetdata);
   server.on("/clear", handlelimpiarips);
   server.onNotFound(handleRoot);
   
   //server.onNotFound(handleRoot)
   //server.on("/", handleRoot);  //Associate handler function to path
   server.begin();  
   Serial.println("HTTP server started");
}



//=======================================================================
//                    handles de paginas
//=======================================================================
void handleRoot(AsyncWebServerRequest *request) {
  request->send(200, "text/html", pagename); //pagename nombre de la variable donde se guarda la pagina
}

void iprefresh(AsyncWebServerRequest *request) {
  int params = request->params();
  free(listaips);
  listaips = (byte*) malloc(sizeof(byte)*params*4);
  IPAddress ip;
  for (int i = 0; i < params; i++)
  {
    AsyncWebParameter* p = request->getParam(i);
    const char * str =(p->value().c_str());
    ip.fromString(p->value());
    //uint8_t ip[4];
    //sscanf(str, "%u.%u.%u.%u", &ip[0], &ip[1], &ip[2], &ip[3]);   
    listaips[i*4]=ip[0];
    listaips[1+i*4]=ip[1];
    listaips[2+i*4]=ip[2];
    listaips[3+i*4]=ip[3];
  } 
  Nips = params;
  request->send(200, "text/html");
  saveipslist();
}

void handlewificonf(AsyncWebServerRequest *request){
    int np = request->params();
    if(request->hasParam("ssid") && request->hasParam("pass")){
        AsyncWebParameter* pss = request->getParam("ssid");
        AsyncWebParameter* pps = request->getParam("pass");
        ssid = pss->value().c_str();
        password = pps->value().c_str();
        String texto = "\n<!DOCTYPE html>\n<html>\n <p> Configurando ssid: "+ ssid +" pass: "+ password +"</p>\n</html>\n\n\n";
        request->send(200, "text/html",texto);   
        reconfwifi=true;
        Serial.printf("Ejecucion hadd\n");
        //server.end();

        flagaux=true;
        //conectowifi();
        
      }
}
void handlegetdata(AsyncWebServerRequest *request){
  // Relacion entre puntero y vector
  // - Un puntero es igual al nombre del vector pe=vec
  // byte* pe == byte[] vec
  // por eso lo de restar un numero mas para apuntar al nombre y no al elemento[0]     
  if(Nips>0){
    String texto = "";
    for(int i=0;i<Nips-1;i++){
      Serial.print(listaips[i*4]);
      Serial.print(listaips[i*4+1]);
      Serial.print(listaips[i*4+2]);
      Serial.println(listaips[i*4+3]);
      texto+= "\""+String(i)+"\":\""+IpAddress2String(generarIP(listaips,i*4))+"\",";
      Serial.println(texto);
    }
    texto+="\""+String(Nips-1)+"\":\""+IpAddress2String(generarIP(listaips,(Nips-1)*4))+"\"";
    

    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", "{"+texto+"}");
    response->addHeader("Test-Header", "get ips");
    request->send(response);

    //request->send(200,"multipart/form-data",texto);
    Serial.printf("Send data:\n");
    Serial.println(texto);  
  } 
}

 void handlelimpiarips(AsyncWebServerRequest *request){
    free(listaips);
    Nips=0;
    writeInt(MEMCOUNTIPS,Nips);
    listaips = (byte*) malloc(sizeof(byte)*1);
    listaips[0]=0; 
    request->send(200, "text/html","OK"); 
  }


/* Desuso
void handlegetdata(AsyncWebServerRequest *request){
  // Relacion entre puntero y vector
  // - Un puntero es igual al nombre del vector pe=vec
  // byte* pe == byte[] vec
  // por eso lo de restar un numero mas para apuntar al nombre y no al elemento[0]     
  if(Nips>0){
    String texto = "";
    for(int i=0;i<Nips-1;i++){
      texto+=String(i)+"="+IpAddress2String(generarIP(listaips,i*4));
    }
    texto+=String(Nips-1)+"="+IpAddress2String(generarIP(listaips,(Nips-1)*4));
    //texto+=String(Nips-1)+"="+IpAddress2String(generarIP(&listaips[(Nips-1)*4-1]));

    AsyncWebServerResponse *response = request->beginResponse(200, "multipart/form-data", texto);
    response->addHeader("Test-Header", "My header value");
    request->send(response);

    //request->send(200,"multipart/form-data",texto);
    Serial.printf("Send data:\n");
    Serial.println(texto);  
  } 
}
*/
void handlegetdata2(AsyncWebServerRequest *request){
  // Relacion entre puntero y vector
  // - Un puntero es igual al nombre del vector pe=vec
  // byte* pe == byte[] vec
  // por eso lo de restar un numero mas para apuntar al nombre y no al elemento[0]     
  if(Nips>0){
    String texto = "";
    for(int i=0;i<Nips-1;i++){
      texto+=String(i)+"="+IpAddress2String(generarIP(listaips,i*4))+"&";
    }
    texto+=String(Nips-1)+"="+IpAddress2String(generarIP(listaips,(Nips-1)*4));
    //texto+=String(Nips-1)+"="+IpAddress2String(generarIP(&listaips[(Nips-1)*4-1]));
    request->send(200,"application/x-www-form-urlencoded",texto);
    Serial.printf("Send data:\n");
    Serial.println(texto);  
  } 
}
//=======================================================================
//                    Funciones Generales
//=======================================================================

//== IP en byte to IPAddress
IPAddress generarIP(byte* bytes,int palote){
  return IPAddress(bytes[0+palote], bytes[1+palote], bytes[2+palote], bytes[3+palote]);
  }

String IpAddress2String(const IPAddress& ipAddress)
{
//  String  aux1 = String(ipAddress[0]) + String(".") + String(ipAddress[1]) + String(".") +\
//  String(ipAddress[2]) + String(".") + String(ipAddress[3]); 
//  String aux2 = String() + ipAddress[0] + "." + ipAddress[1] + "." + ipAddress[2] + "." + ipAddress[3];
  return String(ipAddress[0]) + String(".") + String(ipAddress[1]) + String(".") +\
         String(ipAddress[2]) + String(".") + String(ipAddress[3]); 
}
