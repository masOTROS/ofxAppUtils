/*
 * Copyright (c) 2011-2012 Dan Wilcox <danomatika@gmail.com>
 *
 * BSD Simplified License.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 *
 * See https://github.com/danomatika/ofxAppUtils for documentation
 *
 */
#include "ofxApp.h"

#include "ofGraphics.h"
#include "ofAppRunner.h"

#include "ofxSceneManager.h"

/// APP

//--------------------------------------------------------------
ofxApp::ofxApp() : _currentWarpPoint(-1) {

	bDebug = false;
	
	_bAutoTransforms = true;
	
	_bEditingWarpPoints = false;
	
	_bDrawFramerate = true;
	_framerateColor.set(255);	// white
	
	_sceneManager = NULL;
	_bSceneManagerUpdate = true;
	_bSceneManagerDraw = true;

#ifdef OFX_APP_UTILS_USE_CONTROL_PANEL
	_bTransformControls = false;
	_bDrawControlPanel = true;
#endif
}

//--------------------------------------------------------------
void ofxApp::setMirror(bool mirrorX, bool mirrorY) {
	setMirrorX(mirrorX);
	setMirrorY(mirrorY);
}

void ofxApp::setMirrorX(bool mirrorX) {
	_bMirrorX = mirrorX;
#ifdef OFX_APP_UTILS_USE_CONTROL_PANEL
	if(_bTransformControls)
		controlPanel.setValueB("transformMirrorX", _bMirrorX);
#endif
}

void ofxApp::setMirrorY(bool mirrorY) {
	_bMirrorY = mirrorY;
#ifdef OFX_APP_UTILS_USE_CONTROL_PANEL
	if(_bTransformControls)
		controlPanel.setValueB("transformMirrorY", _bMirrorY);
#endif
}

void ofxApp::setOrigin(float x, float y, float z)	{
	_origin.set(x, y, z);
#ifdef OFX_APP_UTILS_USE_CONTROL_PANEL
	if(_bTransformControls) {
		controlPanel.setValueF("transformPos", x, 0);
		controlPanel.setValueF("transformPos", y, 1);
		controlPanel.setValueF("transformZ", x);
	}
#endif
}

void ofxApp::setAspect(bool aspect) {
	_bHandleAspect = aspect;
#ifdef OFX_APP_UTILS_USE_CONTROL_PANEL
	if(_bTransformControls)
		controlPanel.setValueB("transformAspect", _bHandleAspect);
#endif
}
		
void ofxApp::setCentering(bool center) {
	_bCenter = center;
#ifdef OFX_APP_UTILS_USE_CONTROL_PANEL
	if(_bTransformControls)
		controlPanel.setValueB("transformCenter", _bCenter);
#endif
}

void ofxApp::setWarp(bool warp) {
	_bWarp = warp;
#ifdef OFX_APP_UTILS_USE_CONTROL_PANEL
	if(_bTransformControls)
		controlPanel.setValueB("transformEnableQuadWarper", _bWarp);
#endif
}

//--------------------------------------------------------------
#ifdef OFX_APP_UTILS_USE_CONTROL_PANEL

void ofxApp::addTransformControls(int panelNum, int panelCol) {
	if(_bTransformControls)
		return;
	if(panelNum < 0) {
		controlPanel.addPanel("Transformer", 1, false);
		controlPanel.setWhichPanel(controlPanel.panels.size()-1);
	}
	else {
		controlPanel.setWhichPanel(panelNum);
	}
	controlPanel.setWhichColumn(panelCol);
	controlPanel.addSlider2D("position", "transformPosition", getOriginX(), getOriginY(),
							 -getRenderWidth(), getRenderWidth(),
							 -getRenderHeight(), getRenderHeight(), false);
	controlPanel.addSlider("z", "transformZ", getOriginZ(), -1000, 200, false);
	controlPanel.addToggle("keep aspect", "transformAspect", getOriginTranslate());
	controlPanel.addToggle("center rendering", "transformCenter", getCentering());
	controlPanel.addToggle("mirror x", "transformMirrorX", getMirrorX());
	controlPanel.addToggle("mirror y", "transformMirrorY", getMirrorY());
	controlPanel.addToggle("enable quad warper", "transformEnableQuadWarper", getWarp());
	controlPanel.addToggle("edit quad warper", "transformEditQuadWarper", false);
	controlPanel.addToggle("save quad warper", "transformSaveQuadWarper", false);
	_bTransformControls = true;
}

void ofxApp::loadControlSettings(const string xmlFile) {
	controlPanel.loadSettings(ofToDataPath(xmlFile));
}

void ofxApp::saveControlSettings(const string xmlFile) {
	controlPanel.saveSettings(ofToDataPath(xmlFile));
}

void ofxApp::setDrawControlPanel(bool draw) {
	_bDrawControlPanel = draw;
}

bool ofxApp::getDrawControlPanel() {
	return _bDrawControlPanel;
}

void ofxApp::drawControlPanel() {
	ofFill();
	ofSetColor(255);
	ofSetRectMode(OF_RECTMODE_CORNER);
	controlPanel.draw();
}

#endif

//--------------------------------------------------------------
void ofxApp::drawFramerate(float x, float y) {
	ofSetColor(_framerateColor);
	ofDrawBitmapString("fps: "+ofToString(ofGetFrameRate()), x, y);
}

//--------------------------------------------------------------
void ofxApp::setSceneManager(ofxSceneManager* manager) {
	if(manager == NULL) {
		ofLogWarning("ofxApp") << "cannot add NULL scene manager";
		return;
	}
	_sceneManager = manager;
}

//--------------------------------------------------------------
ofxSceneManager* ofxApp::getSceneManager() {
	return _sceneManager;
}
		
//--------------------------------------------------------------
void ofxApp::clearSceneManager() {
	_sceneManager = NULL;
}

/// RUNNER APP

//--------------------------------------------------------------
ofxApp::RunnerApp::RunnerApp(ofxApp* app) {
	this->app = app;
}

//--------------------------------------------------------------
ofxApp::RunnerApp::~RunnerApp() {
	delete app;
}

//--------------------------------------------------------------
void ofxApp::RunnerApp::setup() {
	
	// set transform sizes here, since width/height aren't set yet in main.cpp
	app->clearTransforms();
	app->setRenderSize(ofGetWidth(), ofGetHeight());
	
#ifdef OFX_APP_UTILS_USE_CONTROL_PANEL
	app->controlPanel.setup("App Controls", 1, 0, 275, app->getRenderHeight()-40);
#endif
	
	app->setup();
}

//--------------------------------------------------------------
void ofxApp::RunnerApp::update() {

	app->mouseX = mouseX;
	app->mouseY = mouseY;

#ifdef OFX_APP_UTILS_USE_CONTROL_PANEL
	ofxControlPanel& controlPanel = app->controlPanel;

	if(app->_bTransformControls) {
	
		// grab control panel variables
		app->_origin.set(controlPanel.getValueF("transformPosition", 0),	// x
						 controlPanel.getValueF("transformPosition", 1),	// y
						 controlPanel.getValueF("transformZ"));             // z
		
		// keep aspect?
		app->_bHandleAspect = controlPanel.getValueB("transformAspect");
		app->_bCenter = controlPanel.getValueB("transformCenter");
		
		// mirror x/y?
		app->_bMirrorX = controlPanel.getValueB("transformMirrorX");
		app->_bMirrorY = controlPanel.getValueB("transformMirrorY");
		
		// enable quad warper?
		app->_bWarp = controlPanel.getValueB("transformEnableQuadWarper");
		
		// edit quad warper?
		if(controlPanel.getValueB("transformEditQuadWarper")) {
			app->setEditWarp(true);
			controlPanel.setValueB("transformEditQuadWarper", false);
		}
		
		// save quad warper?
		if(controlPanel.getValueB("transformSaveQuadWarper")) {
			//app->saveWarpSettings();
			controlPanel.setValueB("transformSaveQuadWarper", false);
		}
	}

	controlPanel.update();
#endif

	if(app->_sceneManager && app->_bSceneManagerUpdate)
		app->_sceneManager->update();
	app->update();
}

//--------------------------------------------------------------
// TODO: changing _bAutoTransforms in the user draw function may result a missing
// transform push/pop
void ofxApp::RunnerApp::draw() {

	if(app->_bAutoTransforms)
		app->pushTransforms(app->_bEditingWarpPoints);
		
	if(app->_sceneManager && app->_bSceneManagerDraw)
		app->_sceneManager->draw();
		
	// do the user callback
	app->draw();		
	
	if(app->_bWarpPushed) {
		ofPopMatrix();
		app->_bWarpPushed = false;
	}
	
	// draw the quad warper editor
	if(app->_bEditingWarpPoints && app->bDebug) {
	
		// push transforms if needed (for manual mode)
		bool forceTransform = !app->_bTransformsPushed;
		if(forceTransform) {
			app->pushTransforms(true);
			ofPopMatrix();
		}
		
		// draw projection warping bounding box
		ofNoFill();
		ofSetRectMode(OF_RECTMODE_CORNER);
		ofSetHexColor(0x00FF00);
		ofRect(0.35, 0.35, app->_renderWidth-1.35, app->_renderHeight-1.35);
		ofSetRectMode(OF_RECTMODE_CORNER);
		ofFill();
		
		if(forceTransform) {
			ofPopMatrix();
			app->_bTransformsPushed = false;
		}
	}
	
	if(app->_bAutoTransforms && app->_bTransformsPushed) {
		ofPopMatrix();
		app->_bTransformsPushed = false;
	}
	
	if(app->bDebug) {
		stringstream text;
		
		// draw the quad warper editor
		if(app->_bEditingWarpPoints) {
		
			ofSetHexColor(0x00FF00);
			text << "Quad Warper Edit Mode" << endl
				 << "Drag from the corners of the screen" << endl
				 << "Click center rectangle to exit";
			ofDrawBitmapString(text.str(), 28, 28);
			text.str("");
				
			// draw center exit box
			ofNoFill();
			ofSetRectMode(OF_RECTMODE_CENTER);
			ofRect(ofGetWidth()/2, ofGetHeight()/2, 100, 100);
			ofSetRectMode(OF_RECTMODE_CORNER);
			ofFill();
		}
#ifdef OFX_APP_UTILS_USE_CONTROL_PANEL
		else if(app->_bDrawControlPanel) {
			app->drawControlPanel();
		}
#endif
		if(app->_bDrawFramerate)
			app->drawFramerate(ofGetWidth()-100, ofGetHeight()-6);
	}
}

//--------------------------------------------------------------
void ofxApp::RunnerApp::exit() {
	app->exit();
	if(app->_sceneManager)
		app->_sceneManager->clear();
}

//--------------------------------------------------------------
void ofxApp::RunnerApp::keyPressed(int key) {
	if(app->_sceneManager)
		app->_sceneManager->keyPressed(key);
	app->keyPressed(key);

#ifdef OFX_APP_UTILS_USE_CONTROL_PANEL
	if(app->bDebug) {
		app->controlPanel.keyPressed(key);
	}
#endif
}

//--------------------------------------------------------------
void ofxApp::RunnerApp::keyReleased(int key) {
	if(app->_sceneManager)
		app->_sceneManager->keyReleased(key);
	app->keyReleased(key);
}

//--------------------------------------------------------------
void ofxApp::RunnerApp::mouseMoved(int x, int y) {
	if(app->_sceneManager)
		app->_sceneManager->mouseMoved(x, y);
	app->mouseMoved(x, y);
}

//--------------------------------------------------------------
void ofxApp::RunnerApp::mouseDragged(int x, int y, int button) {
	if(app->_sceneManager)
		app->_sceneManager->mouseDragged(x, y, button);
	app->mouseDragged(x, y, button);
	
	if(app->bDebug) {
		if(app->_bEditingWarpPoints) {
			if(app->_currentWarpPoint >= 0) {
				app->_quadWarper.setPoint(app->_currentWarpPoint,
					ofVec2f((float)x/ofGetWidth(), (float)y/ofGetHeight()));
			}
		}
#ifdef OFX_APP_UTILS_USE_CONTROL_PANEL
		else {
			app->controlPanel.mouseDragged(x, y, button);
		}
#endif
	}
}

//--------------------------------------------------------------
void ofxApp::RunnerApp::mousePressed(int x, int y, int button) {
	if(app->_sceneManager)
		app->_sceneManager->mousePressed(x, y, button);
	app->mousePressed(x, y, button);
	
	if(app->bDebug) {
		if(app->_bEditingWarpPoints) {
		
			// check if middle of the screen was pressed to exit edit mode
			if((x > ofGetWidth()/2  - 50 && (x < ofGetWidth()/2  + 50) &&
			   (y > ofGetHeight()/2 - 50 && (y < ofGetHeight()/2 + 50))))
			{
				app->_bEditingWarpPoints = false;
				return;
			}
			
			// check if the screen corners are being clicked
			float smallestDist = 1.0;
			app->_currentWarpPoint = -1;
			for(int i = 0; i < 4; i++) {
				float distx = app->_quadWarper.getPoint(i).x - (float) x/ofGetWidth();
				float disty = app->_quadWarper.getPoint(i).y - (float) y/ofGetHeight();
				float dist  = sqrt(distx * distx + disty * disty);

				if(dist < smallestDist && dist < 0.1) {
					app->_currentWarpPoint = i;
					smallestDist = dist;
				}
			}	
		}
#ifdef OFX_APP_UTILS_USE_CONTROL_PANEL
		else {
			app->controlPanel.mousePressed(x, y, button);
		}
#endif
	}
}

//--------------------------------------------------------------
void ofxApp::RunnerApp::mouseReleased(int x, int y, int button) {
	if(app->_sceneManager)
		app->_sceneManager->mouseReleased(x, y, button);
	app->mouseReleased(x, y, button);
	
#ifdef OFX_APP_UTILS_USE_CONTROL_PANEL
	if(app->bDebug) {
		if(!app->_bEditingWarpPoints) {
			app->controlPanel.mouseReleased();
		}
	}
#endif
	app->_currentWarpPoint = -1;
}

//--------------------------------------------------------------
void ofxApp::RunnerApp::windowResized(int w, int h) {
	if(app->_sceneManager)
		app->_sceneManager->windowResized(w, h);
	app->windowResized(w, h);
}

//--------------------------------------------------------------
void ofxApp::RunnerApp::dragEvent(ofDragInfo dragInfo) {
	if(app->_sceneManager)
		app->_sceneManager->dragEvent(dragInfo);
	app->dragEvent(dragInfo);
}

//--------------------------------------------------------------
void ofxApp::RunnerApp::gotMessage(ofMessage msg){
	if(app->_sceneManager)
		app->_sceneManager->gotMessage(msg);
	app->gotMessage(msg);
}

// ofBaseSoundInput
//--------------------------------------------------------------
void ofxApp::RunnerApp::audioIn(float * input, int bufferSize, int nChannels, int deviceID, long unsigned long tickCount) {
	if(app->_sceneManager)
		app->_sceneManager->audioIn(input, bufferSize, nChannels, deviceID, tickCount);
	app->audioIn(input, bufferSize, nChannels, deviceID, tickCount);
}

void ofxApp::RunnerApp::audioIn(float * input, int bufferSize, int nChannel ) {
	if(app->_sceneManager)
		app->_sceneManager->audioIn(input, bufferSize, nChannel);
	app->audioIn(input, bufferSize, nChannel);
}
void ofxApp::RunnerApp::audioReceived(float * input, int bufferSize, int nChannels) {
	if(app->_sceneManager)
		app->_sceneManager->audioIn(input, bufferSize, nChannels);
	app->audioIn(input, bufferSize, nChannels);
}

// ofBaseSoundOutput
//--------------------------------------------------------------
void ofxApp::RunnerApp::audioOut(float * output, int bufferSize, int nChannels, int deviceID, long unsigned long tickCount) {
	if(app->_sceneManager)
		app->_sceneManager->audioOut(output, bufferSize, nChannels, deviceID, tickCount);
	app->audioOut(output, bufferSize, nChannels, deviceID, tickCount);
}

void ofxApp::RunnerApp::audioOut(float * output, int bufferSize, int nChannels) {
	if(app->_sceneManager)
		app->_sceneManager->audioOut(output, bufferSize, nChannels);
	app->audioOut(output, bufferSize, nChannels);
}

void ofxApp::RunnerApp::audioRequested(float * output, int bufferSize, int nChannels) {
	if(app->_sceneManager)
		app->_sceneManager->audioOut(output, bufferSize, nChannels);
	app->audioOut(output, bufferSize, nChannels);
}

#ifdef TARGET_OF_IPHONE
// ofxiPhoneApp
//--------------------------------------------------------------
void ofxApp::RunnerApp::touchDown(ofTouchEventArgs & touch) {
	if(app->_sceneManager)
		app->_sceneManager->touchDown(touch);
	app->touchDown(touch);
}

void ofxApp::RunnerApp::touchMoved(ofTouchEventArgs & touch) {
	if(app->_sceneManager)
		app->_sceneManager->touchMoved(touch);
	app->touchMoved(touch);
}

void ofxApp::RunnerApp::touchUp(ofTouchEventArgs & touch) {
	if(app->_sceneManager)
		app->_sceneManager->touchUp(touch);
	app->touchUp(touch);
}

void ofxApp::RunnerApp::touchDoubleTap(ofTouchEventArgs & touch) {
	if(app->_sceneManager)
		app->_sceneManager->touchDoubleTap(touch);
	app->touchDoubleTap(touch);
}

void ofxApp::RunnerApp::touchCancelled(ofTouchEventArgs & touch) {
	if(app->_sceneManager)
		app->_sceneManager->touchCancelled(touch);
	app->touchCancelled(touch);
}

void ofxApp::RunnerApp::lostFocus() {
	if(app->_sceneManager)
		app->_sceneManager->lostFocus();
	app->lostFocus();
}

void ofxApp::RunnerApp::gotFocus() {
	if(app->_sceneManager)
		app->_sceneManager->gotFocus();
	app->gotFocus();
}

void ofxApp::RunnerApp::gotMemoryWarning() {
	if(app->_sceneManager)
		app->_sceneManager->gotMemoryWarning();
	app->gotMemoryWarning();
}

void ofxApp::RunnerApp::deviceOrientationChanged(int newOrientation) {
	if(app->_sceneManager)
		app->_sceneManager->deviceOrientationChanged(newOrientation);
	app->deviceOrientationChanged(newOrientation);
}
#endif
