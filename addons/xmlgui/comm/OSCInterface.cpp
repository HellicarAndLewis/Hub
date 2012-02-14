/**
 *  OSCInterface.cpp
 *
 *  Created by Marek Bereza on 13/02/2012.
 */

#include "OSCInterface.h"
#include "Instantiator.h"


void xmlgui::OSCInterface::setup(Container &root, int receiveOscPort, int sendOscPort, string sendOscHost) {
	this->root = &root;
	
	setupOsc(receiveOscPort, sendOscPort);
}

void xmlgui::OSCInterface::setupOsc(int receiveOscPort, int sendOscPort, string sendOscHost) {
	this->receiveOscPort = receiveOscPort;
	this->sendOscPort = sendOscPort;
	
	if(sendOscPort>0) {
		sender.setup(sendOscHost, sendOscPort); 
	}
	receiver.setup(receiveOscPort);
}

#include "Slider.h"

string xmlgui::OSCInterface::describeGuiAsXml(xmlgui::Control *ctrl) {
	
	if(ctrl->isContainer()) {
		
		string ss = ctrl->id;
		ofStringReplace(ss, " ", "_");
		
		string s = "<container name=\""+ctrl->name+"\" id=\""+ss+"\">\n";
		xmlgui::Container *cont = (xmlgui::Container*) ctrl;
		for(int i = 0; i < cont->getNumChildren(); i++) {
			s += describeGuiAsXml(cont->getChild(i)) + "\n";
		}
		s += "</container>\n";
		return s;
	} else {
		
		string otherAttrs = "";
				
		vector<ParameterInfo> params;
		ctrl->getParameterInfo(params);
		
		for(int i = 0; i < params.size(); i++) {
			Control *c = INSTANTIATE(params[i].type);
			c->pointToValue(params[i].value);
			string key = params[i].xmlName;
			string value = c->valueToString();
			otherAttrs += " "+key+"=\""+value+"\" ";
			printf("Key %s\n", key.c_str());
			delete c;
			
		}
		
		
		
		
		
		
		
		return "<control type=\""+ctrl->type+"\" name=\""+ctrl->name+"\" id=\""+ctrl->id+"\"  "+otherAttrs+" />";
	}
}

#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTMLForm.h"
#include "Poco/Net/HTTPBasicCredentials.h"
#include "Poco/StreamCopier.h"
#include "Poco/Path.h"
#include "Poco/URI.h"
#include "Poco/Exception.h"
#include "Poco/Mutex.h"

#include <iostream>
#include <queue>
#include <istream>


using namespace Poco::Net;
using namespace Poco;


void xmlgui::OSCInterface::update() {
	while(receiver.hasWaitingMessages()) {
		ofxOscMessage m;
		receiver.getNextMessage(&m);
		if(m.getAddress()=="/describe") {
			printf("Sending description to %s\n", m.getArgAsString(0).c_str());
			
			
			URI uri( m.getArgAsString(0).c_str() );
			std::string path(uri.getPathAndQuery());
			if (path.empty()) path = "/";

			HTTPClientSession session(uri.getHost(), uri.getPort());
			HTTPRequest req(HTTPRequest::HTTP_POST, path, HTTPMessage::HTTP_1_1);
			HTMLForm pocoForm;
			pocoForm.set("description", "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"+describeGuiAsXml(root));
			pocoForm.prepareSubmit(req);
			
			pocoForm.write(session.sendRequest(req));
			
			HTTPResponse res;
			istream& rs = session.receiveResponse(res);
		}
	}
}