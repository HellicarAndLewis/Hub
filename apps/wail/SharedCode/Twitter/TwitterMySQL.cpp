#include "TwitterMySQL.h"

namespace roxlu {

TwitterMySQL::TwitterMySQL() 
	:connected(false)
{
}

TwitterMySQL::~TwitterMySQL() {
}

bool TwitterMySQL::connect(const string& server, const string& db, const string& user, const string& pass) {
	mysql_init(&conn);
	if(&conn == NULL) {
		printf("Error mysql: cannot create mysql object.\n");
		return false;
	}
	
	if(!mysql_real_connect(&conn, server.c_str(), user.c_str(), pass.c_str(), db.c_str(), 0, NULL, 0)) {
		printf("Error mysql:  %s\n", mysql_error(&conn));
		return false;
	}
	
	host = server;
	database = db;
	username = user;
	password = pass;
	connected = true;
	return true;
}

bool TwitterMySQL::getBadWords(vector<string>& result) {
	if(!connected) {
		printf("MySQL.getBadWords: not (yet) connected.\n");
		return false;
	}
	
	int status = mysql_query(&conn, "SELECT word FROM badwords");
	if(status != 0) {
		printf("Error mysql:  %s\n", mysql_error(&conn));
		return false;
	}
	
	MYSQL_RES* res = mysql_store_result(&conn);
	if(res == NULL) {
		printf("Error mysql:  %s\n", mysql_error(&conn));
		return false;
	}
	
	MYSQL_ROW row;
	while((row = mysql_fetch_row(res))) {
		if(!row[0]) {
			continue;
		}
		result.push_back(row[0]);
	}
	return true;
}

bool TwitterMySQL::getTrackList(vector<string>& result) {
	if(!connected) {
		printf("MySQL.getTrackList: not (yet) connected.\n");
		return false;
	}
	
	int status = mysql_query(&conn, "SELECT name FROM track");
	if(status != 0) {
		printf("Error mysql:  %s\n", mysql_error(&conn));
		return false;
	}
	
	MYSQL_RES* res = mysql_store_result(&conn);
	if(res == NULL) {
		printf("Error mysql:  %s\n", mysql_error(&conn));
		return false;
	}
	
	MYSQL_ROW row;
	while((row = mysql_fetch_row(res))) {
		if(!row[0]) {
			continue;
		}
		result.push_back(row[0]);
	}
	return true;
}

bool TwitterMySQL::getSetting(const string& name, string& result) {
	if(!connected) {
		printf("MySQL.getSetting: not (yet) connected.\n");
		return false;
	}
	
	string sql = "SELECT value FROM settings WHERE name = \"" +name +"\"";
	int status = mysql_query(&conn, sql.c_str());
	if(status != 0) {
		printf("Error mysql:  %s\n", mysql_error(&conn));
		return false;
	}
	
	MYSQL_RES* res = mysql_store_result(&conn);
	if(res == NULL) {
		printf("Error mysql:  %s\n", mysql_error(&conn));
		return false;
	}
	
	MYSQL_ROW row;
	row = mysql_fetch_row(res);
	if(!row[0]) {
		return false;
	}
	result = row[0];
	return true;
}

bool TwitterMySQL::setSetting(const string& name, const string& value) {
	if(!connected) {
		printf("MySQL.getSetting: not (yet) connected.\n");
		return false;
	}
	
	string sql = "UPDATE settings SET value=\"" +value +"\" WHERE name=\"" +name +"\"";
	int status = mysql_query(&conn, sql.c_str());
	if(status != 0) {
		printf("Error mysql:  %s\n", mysql_error(&conn));
		return false;
	}
	return true;
}


} // roxlu