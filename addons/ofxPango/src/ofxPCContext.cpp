#include "ofxPCContext.h"
#include "ofxPCSurface.h"
#include "ofxPCPangoLayout.h"
#include "ofMain.h"
ofxPCContext::ofxPCContext(ofxPCSurface* pSurface)
:surface(pSurface)
,cr_context(NULL) 
{
	std::cout << "+ ofxPCContext" << std::endl;
	pSurface->ref("before");
	cr_context = cairo_create(pSurface->cr_surface);
	pSurface->ref("after");
	if(cairo_status(cr_context) != CAIRO_STATUS_SUCCESS) {	
		cout << "Error creating the context" << std::endl;
	}
	else {
		cout << "Created PCContext" << std::endl;
	}
}

ofxPCContext::~ofxPCContext() {
	destroy();
	surface->ref("dtor");
	delete surface;
		//cout << "~~~~ ofxPCContext" << std::endl;
		//cout << "~~~~ ofxPCContext, ref count: " << cairo_get_reference_count(cr_context) << std::endl;
		//cout << "~~~~ ofxPCContext: cr_context" << cr_context <<  std::endl;
	
}


ofxPCPangoLayout* ofxPCContext::createPangoLayout() {
	ofxPCPangoLayout* playout = new ofxPCPangoLayout(this);
	return playout;
}

void ofxPCContext::color4f(float fR, float fG, float fB, float fA) {
	cairo_set_source_rgba(cr_context, fR, fG, fB, fA);
}

void ofxPCContext::color3f(float fR, float fG, float fB) {
	cairo_set_source_rgb(cr_context, fR, fG, fB);
}

void ofxPCContext::paint() {
	cairo_paint(cr_context);
}

void ofxPCContext::destroy() {
	cairo_destroy(cr_context);
}

ofxPCSurface* ofxPCContext::getSurface() {
	return surface;
}

void ofxPCContext::translate(double nX, double nY) {
	cairo_translate(cr_context, nX, nY);
}