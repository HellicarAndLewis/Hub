#include "QueryParam.h"
#include "Database.h"

namespace roxlu {

QueryParam::QueryParam() 
	:field("")
	,value("")
	,type(SQL_PARAM_TEXT)
{
}

QueryParam::~QueryParam() {
	//printf("~ QueryParam\n");
}

string QueryParam::getField() {
	return field;
}

int QueryParam::getType() {
	return type;
}

void QueryParam::setType(QueryParamType qpType) {
	type = qpType;
}

string QueryParam::getBindName() {
	string result = "";
	result.append(":");
	result.append(field);
	return result;
}

string QueryParam::getBindSQL() {
	string result;
		switch(type) {
		case SQL_PARAM_TEXT: {
			result.append(":");
			result.append(field);
			break;
		}
		case SQL_PARAM_TIMESTAMP: {
			result.append("datetime(:");
			result.append(field);
			result.append(")");
			break;
		}
		default: {
			break;
		}
	}
	return result;
}

string QueryParam::getValue() {
	return value;
}

} // roxlu
