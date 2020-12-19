import processing.core.*; 
import processing.data.*; 
import processing.event.*; 
import processing.opengl.*; 

import java.util.HashMap; 
import java.util.ArrayList; 
import java.io.File; 
import java.io.BufferedReader; 
import java.io.PrintWriter; 
import java.io.InputStream; 
import java.io.OutputStream; 
import java.io.IOException; 

public class sketch_0_test extends PApplet {

JSONObject json;
String fileName = "datos-ejm.json";

Data data = new Data();

public void printTime(){
  print(hour());
  print(":");
  print(minute());
  print(":");
  println(second());
}

public void setup() {
  printTime();
  
  
  //fullScreen();
  //background(0);
  
  println("Setup Done");
}

public void draw(){
  background(0);
  
  data.update();
  data.display();
  
  //rect(0,0, totalV, totalV);

  //text("Total Voltage detected: ", 10,20);
  //text(1.0/1000, 10, 40);
}
float maxVoltage = 135.0f;
float minVoltage = 80.0f;

class Data {
    float totalVoltage = 0;
    float current = 0;
    float speed = 0;
    
    public void update(){
      json = loadJSONObject(fileName);
      JSONArray BMS = json.getJSONArray("BMS");
      JSONObject BMS1 = BMS.getJSONObject(0);
      JSONObject BMS2 = BMS.getJSONObject(1);  
      JSONObject BMS3 = BMS.getJSONObject(2);
      JSONArray BMS1_v_json = BMS1.getJSONArray("cellVoltagemV");
      JSONArray BMS2_v_json = BMS2.getJSONArray("cellVoltagemV");
      JSONArray BMS3_v_json = BMS3.getJSONArray("cellVoltagemV");
      int[] BMS1_v = BMS1_v_json.getIntArray();
      int[] BMS2_v = BMS2_v_json.getIntArray();
      int[] BMS3_v = BMS3_v_json.getIntArray();
    
      int totalV = 0;
      for(int i=0; i<12; i++){
        totalV += BMS1_v[i];
      }
      for(int i=0; i<12; i++){
        totalV += BMS2_v[i];
      }
      for(int i=0; i<12; i++){
        totalV += BMS3_v[i];
      }
      
      totalVoltage = totalV / 1000;
    }
    
    public void display(){
      int margen = 15;
      float batW = 1.0f * 20 / 100; // Width of battery indicator in %
      float batPercent = (totalVoltage - minVoltage)/(maxVoltage - minVoltage);
      float speedW = 1.0f * 25 / 100;
      
      // Battery
      stroke(255);
      strokeWeight(4);
      noFill();
      rect(margen-2, margen-2, width*batW+2, height - 2*margen+2);
      fill(32, 195, 26);
      noStroke();
      rect(margen, height - margen, width*batW, -(height - 2*margen)*batPercent);
      
      // Text
      fill(255);
      textSize(16);
      textAlign(CENTER);
      text(String.format("%.2f",batPercent*100)+"%", margen+width*batW/2, margen+20);
      
      // Speedometer
      noFill();
      stroke(255);
      strokeWeight(4);
      //float d = 2*(width-4*margen-batW*width);
      float d = (height - 2*margen);
      arc(width-margen, height-margen, 2*d, 2*d, PI, PI+HALF_PI);
      arc(width-margen, height-margen, 2*(d-height*speedW), 2*(d-height*speedW), PI, PI+HALF_PI);
      line(width-margen, margen, width-margen, margen+speedW*height);
      line(width-margen-d, height-margen, width-margen-d+speedW*height , height-margen);
      
      
    }
}
  public void settings() {  size(800,480); }
  static public void main(String[] passedArgs) {
    String[] appletArgs = new String[] { "sketch_0_test" };
    if (passedArgs != null) {
      PApplet.main(concat(appletArgs, passedArgs));
    } else {
      PApplet.main(appletArgs);
    }
  }
}
