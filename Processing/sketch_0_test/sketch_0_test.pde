JSONObject json;
String fileName = "datos-ejm.json";

Data data = new Data();

void printTime(){
  print(hour());
  print(":");
  print(minute());
  print(":");
  println(second());
}

void setup() {
  printTime();
  
  size(800,480);
  //fullScreen();
  //background(0);
  
  println("Setup Done");
}

void draw(){
  background(0);
  
  data.update();
  data.display();
  
  //rect(0,0, totalV, totalV);

  //text("Total Voltage detected: ", 10,20);
  //text(1.0/1000, 10, 40);
}
