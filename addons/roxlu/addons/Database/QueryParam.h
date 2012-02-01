#ifndef ROXLU_DATABASE_QUERY_PARAMH
#define ROXLU_DATABASE_QUERY_PARAMH

#include "sqlite3.h"
#include <string>
#include <sstream>

using std::string;
using std::stringstream;

namespace roxlu {

// default query param for string values.
class QueryParam {
public:
	enum QueryParamType {
		 SQL_PARAM_TEXT  
		,SQL_PARAM_TIMESTAMP
	};
	
	QueryParam();
	~QueryParam();
	
	// just sets the field name; used wen binding.
	QueryParam& use(const string& fieldName) {
		field = fieldName;
		return *this;
	}
	
	template<typename T>
	QueryParam& use(const string& fieldName, const T& fieldValue) {
		stringstream ss;
		ss << fieldValue;
		value = ss.str();
		field = fieldName;
		return *this;
	}
	void setType(QueryParamType valueType);
	virtual int getType();
	
	string getField();
	string getValue();
	string getBindName();
	string getBindSQL();

protected:
	int type;
	string field;
	string value;
	
};



}

#endif
