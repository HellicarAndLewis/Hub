#include "TwitterDBThread.h"

TwitterDBThread::TwitterDBThread() 
	:search_for_older_then(0)
{
	
}

TwitterDBThread::~TwitterDBThread() {
	std::deque<TwitterDBThreadTask*>::iterator it = tasks.begin();
	while(it != tasks.end()) {
		delete (*it);
		++it;
	}
}

void TwitterDBThread::threadedFunction() {
	if(!db.open("twitter.db")) {
		printf("Error: Cannot open twitter db.\n");
	}
	printf("Info: opened database.\n");
	if(!db.createTables()) {
		printf("Error: Cannot create database.\n");
	}
	
	
	while(true) {
		lock();
			bool handle_tasks = tasks.size();
		unlock();
		
		if(!handle_tasks) {
			ofSleepMillis(500);
		}
		else {
			lock();
			std::deque<TwitterDBThreadTask*>::iterator it = tasks.begin();
			while(it != tasks.end()) {		
				TwitterDBThreadTask* task = (*it);
				//tasks.pop_front();	

				// APPLY A SEARCH
				// --------------
				if(task->kind_of_task == TwitterDBThreadTask::TASK_SEARCH) {
					printf("--------- TASK: SEARCH\n");
					search_results.clear();
					TwitterDBThreadTask_Search* search_task = static_cast<TwitterDBThreadTask_Search*>(task);
					bool r = db.getTweetsWithSearchTerm(
										 search_task->search_term
										,search_task->search_older_than
										,search_task->search_howmany
										,search_results
									);
					
					if(r && search_results.size() > 0) {
						std::sort(search_results.begin(), search_results.end(), TweetTimestampSorter());
//						for(int i = 0; i < search_results.size(); ++i) {
//							printf("[%zu] %s\n", search_results[i].created_at_timestamp, search_results[i].text.c_str());
//						}
						
						search_for_older_then = search_results.back().created_at_timestamp;
						printf("NEXT MUST BE OLDER THAN: %zu\n", search_for_older_then);
					}
					else {
						printf("Cannot search results.\n");
					}
				}
				// INSERT TWEET
				// -------------
				else if (task->kind_of_task == TwitterDBThreadTask::TASK_INSERT_TWEET) {
					//printf("--------- TASK: INSERT TWEET\n");
					TwitterDBThreadTask_InsertTweet* insert_task = static_cast<TwitterDBThreadTask_InsertTweet*>(task);
					db.insertTweet(insert_task->tweet);
				}
				// DELETE TWEET
				// -------------
				else if(task->kind_of_task == TwitterDBThreadTask::TASK_DELETE_TWEET) {
					TwitterDBThreadTask_DeleteTweet* del_task = static_cast<TwitterDBThreadTask_DeleteTweet*>(task);
					db.deleteTweetByTweetID(del_task->id);
				}
				delete task;
				it = tasks.erase(it);
				
			}
			unlock();
		} // handl_task
	}
}

void TwitterDBThread::addTask(TwitterDBThreadTask* task) {
	lock();
		tasks.push_back(task);
	unlock();
}

bool TwitterDBThread::insertTweet(const rtt::Tweet& tweet) {
	TwitterDBThreadTask_InsertTweet* task = new TwitterDBThreadTask_InsertTweet(tweet);
	addTask(task);
	return true;
}

bool TwitterDBThread::deleteTweetByTweetID(const string& id) {
	TwitterDBThreadTask_DeleteTweet* task = new TwitterDBThreadTask_DeleteTweet(id);
	addTask(task);
	return true;
}

void TwitterDBThread::getMoreTweetsMatchingCurrentSearchTerm() {
	TwitterDBThreadTask_Search* task = new TwitterDBThreadTask_Search();
	task->setSearchOlderThan(search_for_older_then);
	task->setSearchHowMany(search_howmany);
	task->setSearchTerm(search_term);
	printf("ADD SEARCH TASK!\n");
	addTask(task);
}

bool TwitterDBThread::getTweetsWithSearchTerm(const string& q, int howMany) {
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
		//unlock();
		//return true;
	}
	unlock();
	return false;
}


bool TwitterDBThread::setSendQueueItemAsSend(int queueID) {
	lock();
		bool r = db.setSendQueueItemAsSend(queueID);
	unlock();
	//TwitterDBThreadTask_SetAsSend* task = new TwitterDBThreadTask_SetAsSend(queueID);
	//addTask(task);
	return r;
}

	
bool TwitterDBThread::insertSendQueueItem(const string& username, const string& filename, int& newID) {
	lock();
		bool r = db.insertSendQueueItem(username, filename, newID);
	unlock();
	return r;
}

bool TwitterDBThread::getNextSendItemFromSendQueue(string& username, string& filename, int& id) {
	lock();
		bool r = db.getNextSendItemFromSendQueue(username, filename, id);
	unlock();
	return r;
}
