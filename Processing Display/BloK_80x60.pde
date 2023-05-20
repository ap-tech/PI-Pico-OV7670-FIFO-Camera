
import processing.serial.*;
 
 
float  [] PD = new float[4800] ;
 int a = 0;
 int b = 0;
 int c = 0;
 int i = 0;
 int u = 0;
 int k = 0;
 Serial myPort;
 






final int rows = 80, columns = 60;
final int numRect = 10;

void setup(){
 size(800,600);
 
 
 
 
  // List all the available serial ports
 println(Serial.list());
 // I know that the first port in the serial list on my mac
 // is always my  Arduino, so I open Serial.list()[0].
 // Open whatever port is the one you're using.
 myPort = new Serial(this,"/dev/cu.usbmodem1421",9600);                //    "/dev/tty.usbmodem1421"
 // don't generate a serialEvent() unless you get a newline character:
 myPort.bufferUntil('\n');
 }
 
 
 
 
 
 


void draw() {
  
  for (int i = 0; i < 60; i++) {
    // Begin loop for rows
    for (int j = 0; j < 80; j++) {
      
       
      int x = i*numRect;
      int y = j*numRect;
      fill(PD[c]);
      stroke(PD[c]);
      rect(y,x,numRect,numRect);
      
     c++; 
      
      
    }
  }
  
    c = 0;
}

    

void serialEvent(Serial myPort) { 
 // get the ASCII string:
 String inString = myPort.readStringUntil('\n');
 
 if (inString != null) {
 // trim off any whitespace:
 inString = trim(inString);
 // split the string on the commas and convert the 
 // resulting substrings into an integer array:
 float[] colors = float(split(inString, ","));
 // if the array has at least three elements, you know
 // you got the whole thing.  Put the numbers in the
 // color variables:
 if (colors.length >=4800) {
 // map them to the range 0-255:
 
 
 
 
 for(i = 0; i < 4800; i ++){

  PD[i] = map(colors[i], 0, 255, 0, 255);
 
 }
 
 
 
 
 
 
 }
 }
 }


 

 

 
 





