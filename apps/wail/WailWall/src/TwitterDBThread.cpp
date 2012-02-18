#include "TwitterDBThread.h"


void TwitterDBThread::threadedFunction() {
	if(!db.open("twitter.db")) {
		printf("Error: Cannot open twitter db.\n");
	}
	
	if(!db.createTables()) {
		printf("Error: Cannot create database.\n");
	}


	while(true) {
		printf("DB Thread....\n");
		sleep(5);
	}
}


bool TwitterDBThread::insertTweet(const rtt::Tweet& tweet) {
	lock();
		bool result = db.insertTweet(tweet);
	unlock();
	return result;
}

bool TwitterDBThread::getTweetsWithTag(const string& tag, int howMany, vector<rtt::Tweet>& result) {
	lock();
		bool r = db.getTweetsWithTag(tag, howMany, result);
	unlock();
	return r;
}

bool TwitterDBThread::getTweetsNewerThan(int age, int howMany, vector<rtt::Tweet>& result) {
	lock();	
		bool r = db.getTweetsNewerThan(age, howMany, result);
	unlock();
	return r;
}

bool TwitterDBThread::getTweetsWithSearchTerm(const string& q, int youngerThan, int howMany, vector<rtt::Tweet>& result) {
	lock();
		bool r = db.getTweetsWithSearchTerm(q, youngerThan, howMany, result);
	unlock();
	return r;
}
	
bool TwitterDBThread::insertSendQueueItem(const string& username, const string& filename, int& newID) {
	lock();
		bool r = db.insertSendQueueItem(username, filename, newID);
	unlock();
	return r;
}

bool TwitterDBThread::setSendQueueItemAsSend(int queueID) {
	lock();
		bool r = db.setSendQueueItemAsSend(queueID);
	unlock();
	return r;

}

bool TwitterDBThread::getNextSendItemFromSendQueue(string& username, string& filename, int& id) {
	lock();
		bool r = db.getNextSendItemFromSendQueue(username, filename, id);
	unlock();
	return r;
}
