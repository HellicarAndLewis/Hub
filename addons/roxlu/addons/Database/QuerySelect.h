#ifndef ROXLU_DATABASE_QUERY_SELECTH
#define ROXLU_DATABASE_QUERY_SELECTH

#include "Query.h"
#include <sstream>
#include <string>
#include "QueryResult.h"

using std::string;

namespace roxlu {

class Database;

class QuerySelect : public Query {
public:
	QuerySelect(Database& db);
	QuerySelect(Database& db, const string& selectFields);
	~QuerySelect();
	
	QuerySelect& from(const string& fromTable);
	bool execute(QueryResult& result);
	string toString();
	
	template<typename T>
	QuerySelect& use(const string& fieldName, const T& fieldValue) {
		field_values.use(fieldName, fieldValue);
		return *this;
	}

	QuerySelect& where(const string& whereClause);
	QuerySelect& join(const string& joinClause);
	QuerySelect& order(const string& orderClause);
	
	template<typename T>
	QuerySelect& limit(const T& t) {
		std::stringstream ss;
		ss << t;
		limit(ss.str());
		return *this;
	}
	
	QuerySelect& limit(const string& limitClause);
	
private:
	string select_fields;
	string from_table;
	string join_clause;
	string where_clause;
	string order_clause;
	string limit_clause;
	
	QueryParams field_values;
};

} // roxlu
#endif
