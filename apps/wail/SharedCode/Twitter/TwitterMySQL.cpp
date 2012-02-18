#include "TwitterMySQL.h"
#include <sstream>

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

// TODO: trying to check if MySQL is faster then sqlite.
bool TwitterMySQL::insertTweet(const rtt::Tweet& tweet) {
	MYSQL_STMT* stmt;
	MYSQL_BIND param[3];
	string sql = "insert into tweets(t_userid, t_screen_name, t_text, t_timestamp) values(?,?,?,UNIX_TIMESTAMP(NOW()))";

	stmt = mysql_stmt_init(&conn);
	if(!stmt) {
		printf("Cannot create statement.\n");
		return false;
	}
	
	if(mysql_stmt_prepare(stmt, sql.c_str(), sql.length()) != 0) {
		printf("Cannot prepare statement.\n");
		return false;
	}
	
	unsigned long s = tweet.user_id.length();
	param[0].buffer_type = MYSQL_TYPE_STRING;
	param[0].buffer = (void*)tweet.user_id.c_str();
	param[0].buffer_length = 50;
	param[0].length = &s;
	param[0].is_null = 0;
	
	unsigned long size_name = tweet.screen_name.length();
	param[1].buffer_type = MYSQL_TYPE_STRING;
	param[1].buffer = (void*)tweet.screen_name.c_str();
	param[1].buffer_length = 20;
	param[1].length = &size_name;
	param[1].is_null = 0;
		
	unsigned long size_text = tweet.text.length();
	param[2].buffer_type = MYSQL_TYPE_STRING;
	param[2].buffer = (void*)tweet.text.c_str();
	param[2].buffer_length = size_text;
	param[2].length = &size_text;
	param[2].is_null = 0;
	
	int status = 0;
	status = mysql_stmt_bind_param(stmt, param);
	if(status) {
		printf("Error: %s (errno: %d)\n", mysql_stmt_error(stmt), mysql_stmt_errno(stmt));
	}
	
	status = mysql_stmt_execute(stmt);
	if(status) {
		printf("Error: %s (errno: %d)\n", mysql_stmt_error(stmt), mysql_stmt_errno(stmt));
	}
	return true;
}


} // roxlu