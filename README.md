# ofxGlobalContext2
ofxGlobalContextのちょっと違うやつ

~~~
  void setup(){
    ...
    context.setup<FOO>(); //if have setup call setup and ofAddlistner(ofEvent().draw, ...) add FOO.update 
    context.setup<HOGEHOGE>(ofEvent().update); // update call update
    ...
  }
  
  void update(){
    const auto& foo = context.get<FOO>();
  }
~~~
