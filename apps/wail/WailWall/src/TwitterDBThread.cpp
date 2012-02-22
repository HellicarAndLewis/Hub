#include "TwitterDBThread.h"

TwitterDBThread::TwitterDBThread() 
	:search_for_older_then(0)
{
	
}

void TwitterDBThread::threadedFunction() {
	if(!db.open("twitter.db")) {
		printf("Error: Cannot open twitter db.\n");
	}
	printf("Info: opened database.\n");
	if(!db.createTables()) {
		printf("Error: Cannot create database.\n");
	}
	
	TwitterDBThreadTask task(TwitterDBThreadTask::TASK_NONE);
	bool has_task = false;
	//bool create_new_search_task = false;
	while(true) {
	
	
		lock();
			if(tasks.size() > 0) {
				task = tasks.front();	
				has_task = true;
				tasks.pop_front();	
			
				if(task.kind_of_task == TwitterDBThreadTask::TASK_SEARCH) {
					search_results.clear();
					
					bool r = db.getTweetsWithSearchTerm(
										 task.search_term
										,task.search_younger_than
										,task.search_howmany
										,search_for_older_then
										,search_results
									);
					
					if(r && search_results.size() > 0) {
						std::sort(search_results.begin(), search_results.end(), TweetTimestampSorter());
						for(int i = 0; i < search_results.size(); ++i) {
							printf("[%zu][%s] %s\n", search_results[i].created_at_timestamp, search_results[i].created_at_string.c_str(), search_results[i].text.c_str());
						}
						
						search_for_older_then = search_results.back().created_at_timestamp;
						printf("NEXT MUST BE OLDER THAN: %zu\n", search_for_older_then);
					}
					else {
						//search_for_older_then -= 3600;
						//create_new_search_task = true;
//						printf(".... %zu\n", search_for_older_then);
						printf("Cannot search results.\n");
					}
				}
			}
		unlock();
//		if(create_new_search_task) {
//			create_new_search_task = false;
//			getMoreTweetsMatchingCurrentSearchTerm();
//		}
		sleep(1);
	}
}

void TwitterDBThread::addTask(TwitterDBThreadTask task) {
	lock();
		tasks.push_back(task);
	unlock();
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

void TwitterDBThread::getMoreTweetsMatchingCurrentSearchTerm() {
	TwitterDBThreadTask task(TwitterDBThreadTask::TASK_SEARCH);
	task.setSearchYoungerThan(100);
	task.setSearchHowMany(search_howmany);
	task.setSearchTerm(search_term);
	addTask(task);
}

// TODO remote howmany
// TODO remove the vector<rtt::Tweet> params!
bool TwitterDBThread::getTweetsWithSearchTerm(const string& q, int youngerThan, int howMany, vector<rtt::Tweet>& result) {
//	TwitterDBThreadTask task(TwitterDBThreadTask::TASK_SEARCH);
//	task.setSearchYoungerThan(youngerThan);
//	task.setSearchHowMany(howMany);
//	task.setSearchTerm(q);

	// remember current query for "getMoreTweetsMatchingCurrentSearchTerm"
	search_howmany = howMany;
	search_term = q;
	search_for_older_then = time(NULL); 
	getMoreTweetsMatchingCurrentSearchTerm();
	return true;
}

bool TwitterDBThread::retrieveSearchResultsFromThread(vector<rtt::Tweet>& result) {
	lock();
	if(search_results.size()) {
		result = search_results; // copy
		search_results.clear(); // and remove
		unlock();
		return true;
	}
	unlock();
	return false;
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
