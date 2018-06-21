#include "ofMain.h"
#include "ofxGlobalContext2.h"
#include "test_context.h"
class ofApp : public ofBaseApp{
    
public:
    void setup(){
        ofSetFrameRate(2);
        context::setup<test_call>();
    }
    void update(){
        std::cout << "ofApp update" << std::endl;
    }
    void draw(){
        std::cout << "ofApp draw" << std::endl;
    }
};


int main( ){
	ofSetupOpenGL(100,100,OF_WINDOW);
	ofRunApp(new ofApp());

}
