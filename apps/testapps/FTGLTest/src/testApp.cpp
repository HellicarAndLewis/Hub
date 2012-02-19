#include "testApp.h"
#include "iconv.h"
#include "utf8.h"
//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(22,33,44);
	font.loadFont("fonts/montreal-ttf/Montreal-LightIta.ttf", 130, true, true, false);
	
<<<<<<< .merge_file_oIuM6U
	std::string str = "being \xf0\x90\x8d\x86\xe6\x97\xa5\xd1\xe0😞 e.g. ü clogged SOME STRING";
   // utf8::replace_invalid(str.begin(), str.end(), back_inserter(message));
	
    message = str;
	message = "RT @AllDaJewsLoveMe: I love Katy Perry 💋💗 ";
//	 string::iterator end_it = utf8::find_invalid(message.begin(), message.end());
//	if (end_it != message.end()) {
//		printf("ERROR\n");
//	}
	
//	vector<unsigned char> utf8result;
//	utf8::utf16to8(str.begin(), str.end(), back_inserter(utf8result));
//	message.clear();
//	std::copy(utf8result.begin(), utf8result.end(), back_inserter(message));
	printf("%s\n", message.c_str());
	
	iconv_t iconv_context =  iconv_open("LATIN1","UTF8");
	iconv_close(iconv_context);
	
=======
	string str = "RT @Natalie10Jones: When you tell me you love me ❤❤❤❤❤❤❤❤❤❤❤❤❤❤❤💙💙💙💙💙💙💙💙💜💜💜💜💜💜💜💛💛💛💛💛💗💗💗💗❤💚💚💙❤💜💚💜❤💜💙❤❤💗💜💜💚💙💚💙❤💗💜💚💛💓";
	string::iterator it = utf8::find_invalid(str.begin(), str.end());
	if(it != str.end()) {
		printf("Invali\n");
	}
	/*
	 string::iterator end_it = utf8::find_invalid(line.begin(), line.end());
        if (end_it != line.end()) {
	*/
>>>>>>> .merge_file_GPFhRe
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
	ofEnableAlphaBlending();
<<<<<<< .merge_file_oIuM6U
	font.drawString(message, 100,300);
=======
	font.drawString("RT @Natalie10Jones: When you tell me you love me ❤❤❤❤❤❤❤❤❤❤❤❤❤❤❤💙💙💙💙💙💙💙💙💜💜💜💜💜💜💜💛💛💛💛💛💗💗💗💗❤💚💚💙❤💜💚💜❤💜💙❤❤💗💜💜💚💙💚💙❤💗💜💚💛💓", 100,100);
//	font.drawString(" being 😞 clogged SOME STRING  ", 100,300);
>>>>>>> .merge_file_GPFhRe
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}