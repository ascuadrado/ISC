class Data {
    boolean testing = false;
    
    // Important data
    float vTotal = 119;
    float speed        = 225;
    float torque = 0.5;
    float throttle     = 0.2;
    float stateOfCharge = 0.45;
    String opMode = "Testing";
    
    // Testing data
    float maxCellV = 3.251;
    float minCellV = 3.345;
    float maxTemp = 21;
    float capacitorV = 85;
    float velocity = 3150;
    float current = 0;

    void update()
    {        
        c.write("request Data\n");
        
        while(c.available()==0){
          delay(1);
        }
        
        String data = c.readString();
        data = data.substring(0, data.indexOf("\n"));
        print(data);
        
        try{
          json = parseJSONObject(data);
          
          // Important data
          vTotal = json.getFloat("vTotal");
          speed = json.getFloat("speed");
          torque = json.getFloat("torque");
          throttle = json.getFloat("throttle");
          stateOfCharge = json.getFloat("stateOfCharge");
          opMode = json.getString("opMode");
          
          // Testing data
          maxCellV = json.getFloat("maxCellV");
          minCellV = json.getFloat("minCellV");
          maxTemp = json.getFloat("maxTemp");
          capacitorV = json.getFloat("capacitorV");
          velocity = json.getFloat("velocity");
          current = json.getFloat("current");
          
        } catch (Exception e){
          println(e);
        }
    }
}
