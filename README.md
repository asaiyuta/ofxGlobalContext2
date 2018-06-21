# ofxGlobalContext2
Helper to use the context with singletons

### Description
If you use a class that has a specific function (update ...) as a public member via this helper, call that function by assigning that function to an arbitrary timing
#### Corresponding function
  * setup(only when calling setup)
  * update(timing changeable)
  * beforeUpdate(fixed timing)
  * afterUpdate(fixed timing)
  * beforeDraw(fixed timing)
  * afterDraw(fixed timing)
#### Example
##### use class
~~~
class Data{
  ...
  void setup(){
    hogehoge...
  }

void update(){
    hogehoge...
  }
  
  void afterUpdate(){
    hogehoge...
  }
  ...
};  
~~~
#### use main app
~~~
  void ofApp::setup(){
    ...
    context::setup<Data>(); //call Data::setup() 
    context::setup<Data>(ofEvents().draw, OF_EVENT_ORDER_AFTER_APP); //want to change the update timing
    //If you do not need to call setup and you do not need to change the timing of update, you do not have to write this
    ...
  }
~~~
#### use ather scene
~~~
  class Scene_A{
    Data& data{context::get<Data>()};
  };
~~~

