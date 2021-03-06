#include "Parameter.h"

namespace roxlu {
namespace curl {
namespace parameter {

Parameter::Parameter() 
	:type(PARAM_STRING)
{
}

Parameter::Parameter(Parameter* other) 
	:type(other->type)
	,name(other->name)
	,value(other->value)
{
}

Parameter::Parameter(const std::string& n, const std::string& v) 
	:name(n)
	,value(v)
	,type(PARAM_STRING)
{
}

Parameter::~Parameter() {
	//printf("~ Parameter()\n");
}

void Parameter::print() {
	printf("%s = '%s' (%d)", getName().c_str(), getStringValue().c_str(), type);
}

bool Parameter::mustUseInSignature() {
	return true;
}



}}} // roxlu::curl::parameter