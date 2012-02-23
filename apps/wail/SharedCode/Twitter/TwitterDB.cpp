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
			",t_tweet_id	VARCHAR(50)"						\
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
	return true;
}


// TWEETS
// -----------------------------------------------------------------------------
bool TwitterDB::insertTweet(const rtt::Tweet& tweet) {
	db.beginTransaction();
	
	// insert tweet
	bool result = db.insert("tweets")
						.use("t_user_id", tweet.user_id)
						.use("t_screen_name", tweet.screen_name)
						.use("t_text", tweet.text)
						.use("t_tweet_id", tweet.tweet_id)
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
	
	db.endTransaction();
	return true;
}

bool TwitterDB::deleteTweetByTweetID(const string& id) {
	string del = "delete from tweets where t_tweet_id =\"" +id +"\"";
	if(!db.query(del)) {
		return false;
	}
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
 * @param	time_t						When you pass a value for youngerThan
 *										you cannot use this; and should use
 *										an value for olderThan == 0
 *										When olderThan isnt 0, you'll get only
 *										tweets which are younger then the 
 *										given timestamp.
 * 										
 * @param	int							How many tweets do you want?
 *
 *
 * @param	vector<rtt::Tweet>& [out]	Is filled with tweets
 */
bool TwitterDB::getTweetsWithSearchTerm(const string& q, time_t olderThan, int howMany,vector<rtt::Tweet>& result) {

	// create where.
	stringstream where;
	where << "tweet_texts MATCH '";
	where << q;
	where << "' AND ";
	where << "t_timestamp < " << olderThan;
	
	
	// join on FTS table
	QueryResult qr(db);
	int start = ofGetElapsedTimeMillis();
	bool r = db.select("t_text,t_screen_name,t_timestamp")
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
		tweet.setScreenName(qr.getString(1));
		tweet.setCreatedAt(qr.getInt(2));
		result.push_back(tweet);
	}
	int end = ofGetElapsedTimeMillis();	
	int diff = end - start;
	printf("Searched for %s and found %zu rows in %d ms. Using older than: %zu and max %d many rows.\n", q.c_str(), result.size(), diff, olderThan, howMany);
	return true;
}

// Send queue: is queried in a thread which sends messages to twitter users
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
		printf("Error: getNextSendItemFromSendQueue() not executed.\n");
		return false;
	}
	
	// step to the first row
	qr.next();
	username = qr.getString(0);
	filename = qr.getString(1);
	id = qr.getInt(2);
	if(id == 0) {
		return false;
	}
	return true;	
}
