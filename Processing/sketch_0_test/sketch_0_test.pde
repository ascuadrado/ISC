JSONObject json;
String fileName = "datos.json";

void printTime(){
  print(hour());
  print(":");
  print(minute());
  print(":");
  println(second());
}

void setup() {
  printTime();
  
  size(500,500);
  //fullScreen();
  //background(0);
  
  println("Setup Done");
}

void draw(){
  background(0);
  
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
  
  //rect(0,0, totalV, totalV);

  text("Total Voltage detected: ", 10,20);
  text(1.0*totalV/1000, 10, 40);
}
