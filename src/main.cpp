#include <Arduino.h>

// Load Wi-Fi library
#include <WiFi.h>

// Replace with your network credentials
const char* ssid     = "NETGEAR74";
const char* password = "quickhill494";

// Set web server port number to 80
WiFiServer server(80);

// this line can be deleted
// so can this line

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String doorState = "closed";

// Assign output variables to GPIO pins

const int led = 25;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;


void setup() {
Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(led, OUTPUT);
  // Set outputs to LOW
  digitalWrite(led, LOW); //this turns led off


  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();         
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /open") >= 0) {
              Serial.println("Door has been opened");
              digitalWrite(led, HIGH);
              delay(1000);
              digitalWrite(led, LOW);
              doorState = "closed";
            } 
//                      
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>Who ate all the croissants?</h1>");
            
       
      
            // If the output5State is off, it displays the ON button       
           if (doorState=="closed") {
              //client.println("<p><a href=\"/open\"><button class=\"button\">CLICK HERE TO OPEN DOOR</button></a></p>");
              client.println("<p><img src='https://i.ibb.co/2spzL5c/hoddle.jpg' width='150' height = '150' style='padding-right: 20px; padding left: 20px; padding-top: 20px; padding-bottom:20px;'><img src='https://i.ibb.co/BqJ9WXk/file-20181129-170250-1gdqh15.jpg' width='150' height='150' style='padding-right: 20px; padding left: 20px; padding-top: 20px; padding-bottom:20px;'></p>");
              client.println("<p><a href=\"/open\"><img src='https://i.ibb.co/Wgz12wT/IMG-4553.jpg' width='150' height = '150' style='padding-right: 20px; padding left: 20px; padding-top: 20px; padding-bottom:20px;'></button></a><img src='https://i.ibb.co/vmQQ2Lz/26c56beddc7266337e0e6409fccec1ff.jpg' width='150' height='150' style='padding-right: 20px; padding left: 20px; padding-top: 20px; padding-bottom:20px;'></p>");
         } 
            
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}