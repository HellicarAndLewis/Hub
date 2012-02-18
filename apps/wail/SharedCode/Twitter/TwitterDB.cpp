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

	// Full Text Search (FTS) table for tweet texts
	roxlu::QueryResult qr(db);
	bool r = db.select("distinct tbl_name")
					.from("sqlite_master")
					.where("tbl_name = \"tweet_texts\"")
					.execute(qr);
	if(!r) {
		printf("error: Cannot get table info for tweet_text (full text search).\n");
		return false;
	}
	if(!qr.next()) {
		bool result_virtual = db.query("CREATE VIRTUAL TABLE tweet_texts USING fts4(text, id)");
		if(!result_virtual) {
			printf("Error: cannot create virtual table.\n");
			return false;
		}
	}
	
	//db.printCompileInfo();

	// TWEETS
	// -------------
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
	
	// SEND QUEUE
	// -------------
	result = db.query(
		"CREATE TABLE IF NOT EXISTS send_queue( "						\
			" sq_id					INTEGER PRIMARY KEY AUTOINCREMENT"	\
			",sq_screen_name		VARCHAR(50)"						\
			",sq_image_file			VARCHAR(200)"						\
			",sq_created_timestamp	TIMESTAMP"							\
			",sq_send				INTEGER DEFAULT 0"					\
		");"
	);

	if(!result) {
		printf("Error: cannot create send_queue table.\n");
		return false;
	}

	
	
	// The tweet tags are used as a backup for searching tweets which contain
	// a specific tweet
	// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	// TAGS
	// -------------
	result = db.query(
		  "CREATE TABLE IF NOT EXISTS tags( "								\
			  "tag_id			INTEGER  PRIMARY KEY AUTOINCREMENT"			\
			  ",tag_name		VARCHAR(50) UNIQUE "						\
		  ");"
	 );
	result = db.query("CREATE INDEX dx_tag_name ON tags(tag_name)");

	
	// TWEET <---> TAGS
	// -------------
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
	return true;
}


// TWEETS
// -----------------------------------------------------------------------------
bool TwitterDB::insertTweet(const rtt::Tweet& tweet) {
	//printf("> %s\n", tweet.text.c_str());
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
	
	// insert text into virtual table (FTS) for some superfast searching!
	result = db.insert("tweet_texts").use("text", tweet.text).use("id", tweet_id).execute();
	if(!result) {
		printf("Error: cannot insert into FTS for tweet.\n");
		return false;
	}
	
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
			printf("Error: cannot connect tag: %d with tweet: %s (%d)\n", (*tag_it), tweet_id);
		}
		++tag_it;
	}
	
	db.endTransaction();
	
	return true;
}


// FOLLOWERS (not used in WailWall project)
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
	
	QuerySelect sel = db.select("t_text")
				.from("tweets")
				.join("tweet_tags on tt_tagid = :tt_tagid and tt_tweetid = t_id")
				.use("tt_tagid", tag_id);
				
	if(howMany > 0) {
		sel.limit(howMany);	
	}
		
	QueryResult qr(db);
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
	return true;
}

bool TwitterDB::getTagID(const string& tag, int& result) {
	QueryResult qr(db);
	bool r = db.select("tag_id")
				.from("tags")
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


/**
 * Get a tweet which contains the given search term
 * 
 * @param	const string&				The search query
 *
 * @param	int							Only return tweets which are younger 
 *										then this number of seconds.
 *										
 * @param	int							How many tweets do you want?
 *
 * @param	vector<rtt::Tweet>& [out]	Is filled with tweets
 */
bool TwitterDB::getTweetsWithSearchTerm(const string& q, int youngerThan, int howMany, vector<rtt::Tweet>& result) {
	// create where.
	stringstream where;
	where << "text MATCH '";
	where << q;
	where << "' AND ";
	where << "t_timestamp > ((strftime('%s', 'now')) - ";
	where << youngerThan; 
	where << ")";

	// join on FTS table
	QueryResult qr(db);
	int start = ofGetElapsedTimeMillis();
	bool r = db.select("t_text")
		.from("tweet_texts")
		.where(where.str())
		.join("tweets on t_id = id")
		.limit(howMany)
		.execute(qr);
	
	if(!r) {
		printf("Error: cannot execute search query.\n");
		return false;
	}
	
	while(qr.next()) {
		rtt::Tweet tweet;
		tweet.setText(qr.getString(0));
		result.push_back(tweet);
	}
	int end = ofGetElapsedTimeMillis();	
	int diff = end - start;
	printf("Searched for %s and found %zu rows in %d ms.\n", q.c_str(), result.size(), diff);
	return true;
}



// SEND QUEUE
// -----------------------------------------------------------------------------
bool TwitterDB::insertSendQueueItem(const string& username, const string& filename, int& newID) {
	bool result = db.insert("send_queue")
					.use("sq_screen_name", username)
					.use("sq_image_file", filename)
					.useTimestamp("sq_created_timestamp")
					.execute();
	if(!result) {
		printf("Error: cannot insert to send queue\n");
		return false;
	}
	newID = db.lastInsertID();
	return true;
}


bool TwitterDB::setSendQueueItemAsSend(int queueID) {
	stringstream ss;
	ss << "UPDATE send_queue SET sq_send = 1 WHERE sq_id = \"" << queueID << "\"";
	return db.query(ss.str());
}

bool TwitterDB::getNextSendItemFromSendQueue(string& username, string& filename, int& id) {
	QueryResult qr(db);
	
	bool r = db.select("sq_screen_name, sq_image_file, sq_id")
		.from("send_queue")
		.where("sq_send = 0")
		.limit(1)
		.order("sq_created_timestamp asc")
		.execute(qr);
	
	if(!r) {
		printf("Error: cannot send queue retrievequery.\n");
		return false;
	}
	
	// step to the first row
	qr.next();
	username = qr.getString(0);
	filename = qr.getString(1);
	id = qr.getInt(2);
	return true;
	
}
