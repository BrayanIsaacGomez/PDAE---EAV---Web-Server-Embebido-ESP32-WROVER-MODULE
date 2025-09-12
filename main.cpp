#include <WiFi.h>
#include <Adafruit_NeoPixel.h>

//#define SERIAL_MONITORING


const char* ssid     = "galileo";       //Comentar estos datos al configurar su propia red
const char* password = "";  //Comentar estos datos al configurar su propia red

WiFiServer server(80);

const int botonPin1 = 19; // botón físico azul
const int botonPin2 = 18; // botón físico amarillo
const int botonPin3 = 5; // botón físico verde

String mensaje = "Esperando...";
char buffer[20];
float_t contador = 0.0f;
uint64_t tiempo_inicio = 0;
uint64_t tiempo_actual = 0;

//Definicion de pines para LEDS
#define PIN 25 //PIN en el cual esta conectado el NEO PIXEL

#define NUMPIXELS 3 // Numero de NEO PIXEL conectados

// Creamos un objeto de tipo NEO PIXEL
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void playNeopixels()
{
  // Animacion de leds con un color especifico
  for(int i=0;i<NUMPIXELS;i++){
    int red = random(0, 255);
    int blue = random(0, 255);
    int green = random(0, 255);
    pixels.setPixelColor(i, pixels.Color(red,green,blue));  // Color en leds turquesa
    pixels.show();                                      //Refrescamos los pixeles
    delay(500);                                         //Retardo de encendido en cada pixel;
  }
}

void playNeopixelsNonBlocking() {
  for(int i=0;i<NUMPIXELS;i++){
      int r = random(0,255);
      int g = random(0,255);
      int b = random(0,255);
      pixels.setPixelColor(i, pixels.Color(r,g,b));
  }
  pixels.show();

}

void setup() {
  #ifdef SERIAL_MONITORING
    Serial.begin(115200);
  #endif

  pinMode(botonPin1, INPUT_PULLUP); // botón con resistencia interna
  pinMode(botonPin2, INPUT_PULLUP); // botón con resistencia interna
  pinMode(botonPin3, INPUT_PULLUP); // botón con resistencia interna

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    #ifdef SERIAL_MONITORING
      Serial.print(".");
    #endif
  }

  #ifdef SERIAL_MONITORING
    Serial.println("\nConectado a WiFi");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  #endif

  server.begin();
  tiempo_inicio = esp_timer_get_time();
}

void loop() {
  
  tiempo_actual = esp_timer_get_time();
  uint64_t tiempo_transcurrido = tiempo_actual - tiempo_inicio;
  if( tiempo_transcurrido >= 1000000 ){
    // tiempo_transcurrido estará en microsegundos
    contador = contador + 0.5f;
    tiempo_inicio = tiempo_actual;

  }
  


  WiFiClient client = server.available();
  if (!client) return;

  String currentLine = "";
  String header = "";



  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      header += c;
      
      if (c == '\n') {
        /*
        Serial.println(" :o   //////////////////");
        Serial.println(header);
        Serial.println(client.remoteIP());
        Serial.println(" ////////////////////////");
        */
        // --- Si la petición fue /estado ---
        if (header.indexOf("GET /estado1") >= 0) {
          if (digitalRead(botonPin1) == LOW) {
            mensaje = "PRESIONADO";
          } else {
            mensaje = "SUELTO";
          }
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/plain");
          client.println();
          client.print(mensaje);
          break;
        }

        if (header.indexOf("GET /estado2") >= 0) {
          if (digitalRead(botonPin2) == LOW) {
            mensaje = "PRESIONADO";
          } else {
            mensaje = "SUELTO";
          }
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/plain");
          client.println();
          client.print(mensaje);
          break;
        }

        if (header.indexOf("GET /estado3") >= 0) {
          if (digitalRead(botonPin3) == LOW) {
            mensaje = "PRESIONADO";
          } else {
            mensaje = "SUELTO";
          }
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/plain");
          client.println();
          client.print(mensaje);
          break;
        }

        

        // --- Si la petición fue /estado ---
        if (header.indexOf("GET /sensor") >= 0) {
          //sprintf(buffer, "%.2f", contador);
          mensaje = String(contador, 2);
          
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/plain");
          client.println();
          client.print(mensaje);
          break;
        }

        if (header.indexOf("GET /H") >= 0) {
          playNeopixelsNonBlocking();
          client.println("HTTP/1.1 200 OK");       // Status
          client.println("Content-type:text/plain");// Header
          client.println();                        // Línea vacía
          client.print("NEOPIXEL_ON");    // Contenido

        }

        if (header.indexOf("GET /L") >= 0) {
          for(int i=0;i<NUMPIXELS;i++){
            pixels.setPixelColor(i, pixels.Color(0,0,0));       //Apagamos los LEDs
            pixels.show();
            //delay(100);
          }
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/plain");
          client.println();
          client.print("NEOPIXEL_OFF");    // Contenido
        }

        // --- Página principal ---
        if (header.indexOf("GET / ") >= 0) {
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println();

          
          client.println("<!DOCTYPE html>");
          client.println("<html>");
          client.println("<head>");
          client.println("<meta charset='UTF-8'>");
          client.println("<title>Control NeoPixels</title>");
          client.println("<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.5.1/css/all.min.css'>");


          // CSS embebido
          client.println("<style>");
          client.println("body { font-family: Arial; text-align:center; background:#f4f4f4; }");
          client.println(".switch { position: relative; display: inline-block; width: 80px; height: 40px; margin: 20px; }");
          client.println(".switch input { opacity: 0; width: 0; height: 0; }");
          client.println(".slider { position: absolute; cursor: pointer; top:0; left:0; right:0; bottom:0; background-color:#ccc; transition:0.4s; border-radius:40px; }");
          client.println(".slider:before { position:absolute; content:''; height:32px; width:32px; left:4px; bottom:4px; background:white; transition:0.4s; border-radius:50%; }");
          client.println("input:checked + .slider { background-color:#2196F3; }");
          client.println("input:checked + .slider:before { transform:translateX(40px); }");
          client.println("</style>");

          client.println("</head>");
          client.println("<body>");

          client.println("<div style='display:flex; justify-content:center; align-items:center; gap:50px; margin-top:100px;'>");

          client.println("<div style='display:flex; flex-direction:column; align-items:center;'>");
          client.println("<h2 style='margin:0;'>Botón 1</h2>");
          client.println("<i id='estado1' class='fa-regular fa-circle' style='color:red;font-size:50px;'></i>");
          client.println("</div>");

          client.println("<div style='display:flex; flex-direction:column; align-items:center;'>");
          client.println("<h2 style='margin:0;'>Botón 2</h2>");
          client.println("<i id='estado2' class='fa-regular fa-circle' style='color:orange;font-size:50px;'></i>");
          client.println("</div>");

          client.println("<div style='display:flex; flex-direction:column; align-items:center;'>");
          client.println("<h2 style='margin:0;'>Botón 3</h2>");
          client.println("<i id='estado3' class='fa-regular fa-circle' style='color:green;font-size:50px;'></i>");
          client.println("</div>");

          client.println("</div>");


          
          // Lectura del sensor
          client.println("<div style='text-align:center; margin-top:40px; margin-bottom:40px;'>");
          //client.println("<h1'>Sensor</h1>");
          client.println("<h2 style='margin:0;'>Sensor</h2>");
          client.println("<div style='display:flex; flex-direction:row; justify-content:center; align-items:center;'>");
          client.println("<h1 id='sensor'>Cargando...</h1>");
          client.println("<h2 style='margin:0;'> °C</h2>");
          client.println("</div>");
          
          client.println("</div>");

          // Slide toggle
          client.println("<div style='text-align:center; margin-top:40px; margin-bottom:40px;'>");

          client.println("<h2 style='margin-bottom:15px;'>Neopixels</h2>");  // texto arriba
          client.println("<label class='switch'>");
          client.println("  <input type='checkbox' id='neopixelToggle'>");
          client.println("  <span class='slider'></span>");
          client.println("</label>");

          client.println("</div>");


          // JS para AJAX y toggle
          client.println("<script>");
          client.println("document.getElementById('neopixelToggle').addEventListener('change', function() {");
          client.println("if(this.checked){ fetch('/H'); } else { fetch('/L'); } });");

          // AJAX para actualizar estado y sensor
          client.println("setInterval(()=>{");
          client.println("fetch('/estado1').then(r=>r.text()).then(t=>{");
          client.println("  const icon = document.getElementById('estado1');");
          client.println("  if(t=='PRESIONADO'){ icon.className='fa-solid fa-circle'; icon.style.color='blue'; }");
          client.println("  else { icon.className='fa-regular fa-circle'; icon.style.color='grey'; }");
          client.println("});");
          client.println("fetch('/estado2').then(r=>r.text()).then(t=>{");
          client.println("  const icon = document.getElementById('estado2');");
          client.println("  if(t=='PRESIONADO'){ icon.className='fa-solid fa-circle'; icon.style.color='yellow'; }");
          client.println("  else { icon.className='fa-regular fa-circle'; icon.style.color='grey'; }");
          client.println("});");
          client.println("fetch('/estado3').then(r=>r.text()).then(t=>{");
          client.println("  const icon = document.getElementById('estado3');");
          client.println("  if(t=='PRESIONADO'){ icon.className='fa-solid fa-circle'; icon.style.color='green'; }");
          client.println("  else { icon.className='fa-regular fa-circle'; icon.style.color='grey'; }");
          client.println("});");
          client.println("fetch('/sensor').then(r=>r.text()).then(t=>{document.getElementById('sensor').innerText=t;});");
          client.println("},300);");
          client.println("</script>");



          client.println("</body>");
          client.println("</html>");


          client.println();
          break;
        }

        if (c == '\n') break; // fin de petición
      }
    }
  }

  client.stop();
}
