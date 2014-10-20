/*
 * Copyright (c) 2011 Dan Wilcox <danomatika@gmail.com>
 *
 * BSD Simplified License.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 *
 * See https://github.com/danomatika/ofxAppUtils for documentation
 *
 */
#pragma once

#include <ofxAppUtils.h>

class Square : public ofxScene {
    
public:
    
    // set the scene name through the base class initializer
    Square() : ofxScene("Square"){
        alpha = 255;
        
        // we want setup to be called each time the scene is loaded
        setSingleSetup(false);
        
        ofxApp *app = ofxGetAppPtr();
        width = app->getRenderWidth();
        height = app->getRenderHeight();
        
        ofSetRectMode(OF_RECTMODE_CENTER);
    }
    
    // scene setup
    void setup() {
        timer.set();
        pos.set(width,height/2);
        
    }
    
    // called when scene is entering
    void updateEnter() {
		
        // called on first enter update
        if(isEnteringFirst()) {
            setup();
            timer.setAlarm(2000);
            alpha = 0;
            ofLogNotice("LineScene") << "update enter";
        }
		
        // calc alpha amount based on alarm time diff
        alpha = 255*timer.getDiffN();
        
        update();
		
        // call finishedEntering() to indicate scene is done entering
        if(timer.alarm()) {
            finishedEntering();
            alpha = 255;
            ofLogNotice("LineScene") << "update enter done";
            timer2.set();
        }
    }
    
    // normal update
    void update() {
        if(!isEntering() && !isExiting()){
            pos.set((sin((float)timer2.getDiff()/1000 + PI/2)+1)/2*width, height/2);
        }
    }
    
    // called when scene is exiting
    void updateExit() {
		
        // called on first exit update
        if(isExitingFirst()) {
            timer.setAlarm(2000);
            alpha = 0;
            ofLogNotice("LineScene") << "update exit";
        }
        
        // calc alpha amount based on alarm time diff
        alpha = 255*abs(timer.getDiffN()-1.0);
        
        update();
		
        // call finishedExiting() to indicate scene is done exiting
        if(timer.alarm()) {
            finishedExiting();
            alpha = 0;
            ofLogNotice("LineScene") << "update exit done";
        }
    }
    
    // draw
    void draw() {
        ofEnableAlphaBlending();
        ofSetColor(255, 255, 0, alpha);	// alpha for fade in/out
        ofRect(pos,80,80);
        ofDisableAlphaBlending();
    }
    
    // cleanup
    void exit() {
        
    }
    
    // used for fade in and out
    ofxTimer timer;
    ofxTimer timer2;
    int alpha;
    ofPoint pos;
    
    float width;
    float height;
    
};
