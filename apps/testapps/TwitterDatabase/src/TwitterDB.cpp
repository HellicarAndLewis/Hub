#include "TwitterDB.h"
#include <sstream>

using std::stringstream;
using namespace roxlu;
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
	
	result = db.query("CREATE INDEX dx_tweet_timestamp ON tweets(t_timestamp)");
	
	// TAGS
	result = db.query(
		  "CREATE TABLE IF NOT EXISTS tags( "								\
			  "tag_id			INTEGER  PRIMARY KEY AUTOINCREMENT"			\
			  ",tag_name		VARCHAR(50) UNIQUE "						\
		  ");"
	 );
	result = db.query("CREATE INDEX dx_tag_name ON tags(tag_name)");

	
	// TWEET <---> TAGS
	result = db.query(
		  "CREATE TABLE IF NOT EXISTS tweet_tags( "					\
		  	" tt_tagid			INTEGER "							\
		  	",tt_tweetid		INTEGER "							\
		  	",PRIMARY KEY(tt_tagid, tt_tweetid) "
		  ");"
		  );
	if(!result) {
		printf("Error: cannot create tweet_tags table.\n");
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


// TWEETS
// -----------------------------------------------------------------------------
bool TwitterDB::insertTweet(const rtt::Tweet& tweet) {
	printf("> %s\n", tweet.text.c_str());
	// get ids for new tags.
	
	db.beginTransaction();
	
	// insert tweet
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
	int tweet_id = db.lastInsertID();
	
	// insert tags
	vector<string>::const_iterator it = tweet.tags.begin();
	while(it != tweet.tags.end()) {	
		db.insert("tags")
			.use("tag_name", (*it))
			.orIgnore()
			.execute();
		++it;
	}
	
	// insert tweet-tag relations.
	roxlu::QueryResult tag_result(db);
	result = db.select("tag_id").from("tags").where("tag_name in (%s) ",tweet.tags).execute(tag_result);
	if(!result) {
		printf("Error: Cannot get tags, something went wrong inserting them..\n");
		return false;
	}
	vector<int> tag_ids;
	while(tag_result.next()) {
		tag_ids.push_back(tag_result.getInt(0));
	}
	
	vector<int>::iterator tag_it = tag_ids.begin();
	while(tag_it != tag_ids.end()) {
		result = db.insert("tweet_tags")
					.use("tt_tagid", (*tag_it))
					.use("tt_tweetid", tweet_id)
					.execute();
		if(!result) {
			printf("Error: cannot connect tag: %d with tweet: %s\n", (*tag_it), tweet_id);
		}
		++tag_it;
	}
	
	db.endTransaction();
	
	return true;
}


// FOLLOWERS
// -----------------------------------------------------------------------------
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

bool TwitterDB::getTweetsWithTag(const string& tag, int howMany, vector<rtt::Tweet>& result) {
	int tag_id = 0;
	if(!getTagID(tag, tag_id)) {
		printf("Tag: %s not found.\n", tag.c_str());
		return false;
	}
	
	//bool r = db.select("t_text")
	
	QueryResult qr(db);
	QuerySelect sel = db.select("t_text");
	sel.toString();
				sel.from("tweets")
				.join("tweet_tags on tt_tagid = :tt_tagid and tt_tweetid = t_id")
				.use("tt_tagid", tag_id);
//				.execute(qr);
	if(howMany > 0) {
		sel.limit(howMany);	
	}
	
	sel.toString();
	return false;
	/*
	bool r = sel.execute(qr);
	if(!r) {
		printf("no tweets found.\n");
		return false;
	}
	
	// @todo put in function
	while(qr.next()) {
		rtt::Tweet tweet;
		tweet.setText(qr.getString(0));
		printf("> %s\n", qr.getString(0).c_str());
		result.push_back(tweet);
	}
//	printf("tag id: %d\n", tag_id);
	return true;
	*/
}

bool TwitterDB::getTagID(const string& tag, int& result) {
	printf("getTagID for: %s\n", tag.c_str());
	QueryResult qr(db);
	bool r = db.select("tag_id")
				.from("tags")
				//.where("tag_name = \"NeverTrustAGuyWho\"")
				.where("tag_name = :tag_name")
				.use("tag_name", tag)
				.execute(qr);
	if(!r) {
		return false;
	}
	qr.next();
	result = qr.getInt(0);	
	printf("found: %d\n", result);
	return true;
}

bool TwitterDB::getTweetsNewerThan(int age, int howMany, vector<rtt::Tweet>& result) {
	QueryResult qr(db);
	stringstream where;
	where << "t_timestamp > ((strftime('%s', 'now')) - ";
	where << age; 
	where << ")";
	
	QuerySelect sel = db.select("t_text")
						.from("tweets")
						.where(where.str());
	
	if(howMany > 0) {
		sel.limit(howMany);
	}
	
	bool r = sel.execute(qr);
	if(!r) {
		return false;
	}
	
	// @todo put in function
	while(qr.next()) {
		rtt::Tweet tweet;
		tweet.setText(qr.getString(0));
		printf("> %s\n", qr.getString(0).c_str());
		result.push_back(tweet);
	}
	return true;
}

bool TwitterDB::getTweetsWithSearchTerm(const string& q, vector<rtt::Tweet>& result) {
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