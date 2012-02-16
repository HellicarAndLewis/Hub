
/**     ___           ___           ___                         ___           ___     
 *     /__/\         /  /\         /  /\         _____         /  /\         /__/|    
 *    |  |::\       /  /::\       /  /::|       /  /::\       /  /::\       |  |:|    
 *    |  |:|:\     /  /:/\:\     /  /:/:|      /  /:/\:\     /  /:/\:\      |  |:|    
 *  __|__|:|\:\   /  /:/~/::\   /  /:/|:|__   /  /:/~/::\   /  /:/  \:\   __|__|:|    
 * /__/::::| \:\ /__/:/ /:/\:\ /__/:/ |:| /\ /__/:/ /:/\:| /__/:/ \__\:\ /__/::::\____
 * \  \:\~~\__\/ \  \:\/:/__\/ \__\/  |:|/:/ \  \:\/:/~/:/ \  \:\ /  /:/    ~\~~\::::/
 *  \  \:\        \  \::/          |  |:/:/   \  \::/ /:/   \  \:\  /:/      |~~|:|~~ 
 *   \  \:\        \  \:\          |  |::/     \  \:\/:/     \  \:\/:/       |  |:|   
 *    \  \:\        \  \:\         |  |:/       \  \::/       \  \::/        |  |:|   
 *     \__\/         \__\/         |__|/         \__\/         \__\/         |__|/   
 *
 *  Description: 
 */

#include "ofMain.h"
#include "ofVec2f.h"
namespace tricks {
	namespace math {

		// gets the intersection of two infinitely long lines described by two points each.
		ofPoint getIntersection(ofPoint a1, ofPoint a2, ofPoint b1, ofPoint b2);

		bool pointIsInsideTriangle(ofVec2f pt, ofVec2f v1, ofVec2f v2, ofVec2f v3);


		/**
		 * Distance of p from line [a to b]
		 */
		float pointLineDistance(const ofVec2f &a, const ofVec2f &b, const ofVec2f &p);


		/**
		 * Which way the curve is turning
		 */
		float loopSign(ofVec2f p1, ofVec2f p2, ofVec2f p3);


		/**
		 * This is the angle of B at a triangle of ABC
		 */
		float angleBetweenPoints(ofVec2f a, ofVec2f b, ofVec2f c);


		/**
		 * This returns a positive value if the point is on one side,
		 * and negative if it's on the other side
		 */
		float whichSideOfLineIsPoint(const ofVec2f &a, const ofVec2f &b, const ofVec2f &p);

		/**
		 * This modifies the rectangle so the point falls within its bounds.
		 */
		void modifyRect(ofRectangle &rect, ofVec2f point);
		
		/**
		 * combines 2 bounding boxes so that the end result encapsulates both
		 */
		ofRectangle combineRects(const ofRectangle &a, const ofRectangle &b);
		
		
		/**
		 * This rotates a rectangle and returns the non aligned bounding box of the result.
		 */
		ofRectangle rotateRect(ofRectangle &r, float rotation);
		
		
		/**
		 * Gets the opengl modelview matrix
		 */
		ofMatrix4x4 getModelViewMatrix();
		
		/**
		 * Gets the opengl projection matrix.
		 */
		ofMatrix4x4 getProjectionMatrix();
		
	}
}