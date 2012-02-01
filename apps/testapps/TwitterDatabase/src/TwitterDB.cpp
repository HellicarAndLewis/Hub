#include "TwitterDB.h"
#include <sstream>

using std::stringstream;
namespace rtt = roxlu::twitter::type;

TwitterDB::TwitterDB() {
}

bool TwitterDB::open(const string& name) {
	string file = ofToDataPath(name, true);
	return db.open(file);
}

bool TwitterDB::createTables() {
	// TWEETS
	bool result = db.query(
		"CREATE TABLE IF NOT EXISTS tweets( "					\
			" t_id			INTEGER PRIMARY KEY AUTOINCREMENT"	\
			",t_user_id		VARCHAR(50)"						\
			",t_text		INTEGER"							\
			",t_screen_name	VARCHAR(20)"						\
			",t_timestamp	TIMESTAMP"							\	
			",t_longitude	REAL"								\
			",t_latitude	REAL"								\
		");"
	);

	if(!result) {
		printf("Error: cannot create tweets table.\n");
		return false;
	}
	
	// USERLIST
	result = db.query(
		"CREATE TABLE IF NOT EXISTS follow( "
			"fl_id				INTEGER PRIMARY KEY AUTOINCREMENT "	\
			",fl_user_id		VARCHAR(50) UNIQUE"					\
			",fl_screen_name	VARCHAR(20) UNIQUE"					\
			",fl_timestamp		TIMESTAMP "							\
		");"
	);
	return true;
}
/*
	string text;
	string tweet_id;
	string user_id;
	string screen_name;
	string avatar;
	vector<rtt::URL*> urls;
};
*/

bool TwitterDB::insertTweet(const rtt::Tweet& tweet) {
	printf("Insert tweet: %s\n", tweet.text.c_str());
	bool result = db.insert("tweets")
						.use("t_user_id", tweet.user_id)
						.use("t_screen_name", tweet.screen_name)
						.use("t_text", tweet.text)
						.useTimestamp("t_timestamp")
						.execute();
	if(!result) {
		printf("Error: cannot insert: %s\n", tweet.text.c_str());
		return false;
	}
	return true;
}

bool TwitterDB::insertFollower(const rtt::StreamEvent& event) {
	bool result = db.insert("follow")
					.use("fl_user_id", event.target.id_str)
					.use("fl_screen_name", event.target.screen_name)
					.execute();
	if(!result) {
		printf("Error: cannot insert user to follow\n");
		return false;
	}
	return true;
}

bool TwitterDB::removeFollower(const rtt::StreamEvent& event) {
	bool result = db.remove("follow")
					.where("fl_user_id = :fl_user_id")
					.use("fl_user_id", event.target.id_str)
					.execute();
	if(!result) {
		printf("Error: cannot remove follower.\n");
	}
	return true;
}

// Returns a comma separated list with usernames. 
bool TwitterDB::getFollowers(vector<string>& result) {
	roxlu::QueryResult qr(db);
	bool r = db.select("fl_user_id")
					.from("follow")
					.order("fl_screen_name asc")
					.execute(qr);
	if(!r) {
		printf("error: cannot get followers list (no entries in table maybe?).\n");
		return false;
	}

	while(qr.next()) {
		result.push_back(qr.getString(0));
	}
	return true;
}

/*
// source for general event.
struct User {
	bool contributors_enabled;
	string name;
	string id_str;
	string screen_name;
	string location;
	uint64_t statuses_count;
	uint64_t id;
};

// target of general event.
struct TargetObject {
	string name;
	string mode;
	string description;
	User user;	
};

// general event: https://dev.twitter.com/docs/streaming-api/user-streams
struct StreamEvent {
	User target;					// who is affected (i.e. removed, added) or owns the affected object
	TargetObject target_object;		// the "changed" object; i.e. a list
	User source; 					// who initiated the event
	string event;					// what kind of event.
};
*/