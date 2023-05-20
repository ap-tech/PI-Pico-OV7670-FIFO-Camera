// Processing code for Robot1 moniter
 
 
 import processing.serial.*;
 
 
float  [] PD = new float[19200] ;
 int a = 0;
 int b = 0;
 int c = 0;
 int i = 0;
 int u = 0;
 Serial myPort;
 
 void setup() {
 size(160, 120);
 
 //(this,"/dev/tty.usbmodem1421",460800);
 // List all the available serial ports
 println(Serial.list());
 // I know that the first port in the serial list on my mac
 // is always my  Arduino, so I open Serial.list()[0].
 // Open whatever port is the one you're using.
 myPort = new Serial(this,"/dev/cu.usbmodem1421",9600);    //115200 460800
 // don't generate a serialEvent() unless you get a newline character:
 myPort.bufferUntil('\n');
 }
 
 void draw() {
 // set the background color with the color values:
  
 
 
 
  
 
 
  for(b = 0; b < 120; b ++){      
  
  for(a = 0; a < 160; a ++){         

  strokeCap(SQUARE);
  stroke(PD[c]);
  line(a, b, a, b+1);

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
 if (colors.length >=19200) {
 // map them to the range 0-255:
 
 
 //76800
 
 for(i = 0; i < 19200; i ++){

  PD[i] = map(colors[i], 0, 255, 0, 255);
 
 }
 
 
 
 
 
 
 }
 }
 }


 

 

 
 

 
 
 
 
 
 
 
 
 
 
 
 
 

