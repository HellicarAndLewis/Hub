//
//  AbstractFader.h
//  audioOutputExample
//
//  Created by Marek Bereza on 02/08/2011.
//
#pragma once

class AbstractFader {
public:
    
    virtual void  setValue(float value) {}; // don't need to respond to this
    virtual float getValue() = 0; // definitely need to respond to this
};