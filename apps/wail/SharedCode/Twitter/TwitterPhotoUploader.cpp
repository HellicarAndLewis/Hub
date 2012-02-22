#include "TwitterPhotoUploader.h"
#include "TwitterApp.h"
namespace roxlu {

TwitterPhotoUploader::TwitterPhotoUploader(TwitterApp& app) 
	:app(app)
{
}

void TwitterPhotoUploader::addFile(
							 const string& file
							,const string& username
							,const string& message
)
{
	lock();
		UploadFileInfo ufi = {file, username, message};
		upload_queue.push_back(ufi);
	unlock();
}

void TwitterPhotoUploader::setup(const string& twitterKey, const string& twitterSecret, const string& tokensFile) {
	twitter.setConsumerKey(twitterKey);
	twitter.setConsumerSecret(twitterSecret);
	if(!twitter.loadTokens(tokensFile)) {
		printf("Warning: cannot load tokens file for twitter photo uploader thread.\n");
	}
}


void TwitterPhotoUploader::threadedFunction() {
	
	while(true) {
		string username;
		string filepath;
		int queue_id;
		
		// get next item from send queue; when it's empty wait for a while
		bool result = app.getNextSendItemFromSendQueue(username, filepath, queue_id);
		if(!result) {
			printf("Info: there are no items in the send queue. Waiting for a couple of seconds\n");
			sleep(2);
			continue;
		}
		
		printf("Got an item from queue: %s %s %d\n", username.c_str(), filepath.c_str(), queue_id);
		
		// When we cannot find an image for this item we remove it from queue.
		if(!ofFile::doesFileExist(filepath, true)) {
			printf("Error: cannot find file to upload: %s\n", filepath.c_str());
			app.setSendQueueItemAsSend(queue_id);
			continue;
		}
	
		// POST IMAGE TO WEBSITE
		// ---------------------
		rc::Request req(URL_TWITTER_UPLOADER);
		req.getParams().addString("act", "upload");
		req.getParams().addFile("photo", ofToDataPath(filepath, false));
		req.getParams().addString("user", username);
		req.getParams().addString("message", "message");

		string response;
		req.doPost(uploader_curl, response, true);

		
		// PARSE RESULT
		// -------------
		bool correctly_uploaded = false;
		string upload_message;
		json_t *root;
		json_error_t error;
		root = json_loads(response.c_str(), 0, &error);
		if(!root) {
			printf("Error: cannot upload image to remote server. The result we get is:\n%s\n", response.c_str());
			json_decref(root);
		}
		else {
			json_t* node = json_object_get(root, "result");
			if(!json_is_boolean(node)) {
				printf("Error: cannot parse the result from the remote server for uploading. \n");
				correctly_uploaded = false;
			}
			else {
				correctly_uploaded = json_is_true(node);
				node = json_object_get(root, "msg");
				upload_message = "no message found.";
				if(json_is_string(node)) {
					upload_message = json_string_value(node);						
				}
			}
		}
		
		if(!correctly_uploaded) {
			printf("Warning: got message form server %s\n", upload_message.c_str());
			printf("Warning: we cannot upload the image; we get an unexpected error from the remote server.\n");
			printf("Warning: we do not remove this item from the queue; we will wait and retry\n");
			sleep(5);
			json_decref(root);
			continue;
		}

		
		// get filepath
		string created_file = "";
		json_t* filepath_node = json_object_get(root, "created_file");
		if(filepath_node != NULL && json_is_string(filepath_node)) {
			created_file = json_string_value(filepath_node);
		}
		
		// get hash
		json_t* node = json_object_get(root, "file_hash");
		if(node == NULL || !json_is_string(node)) {
			printf("Warning: no file hash found in result. \n");
			json_decref(root);
			continue;
		}
		
		string file_hash = json_string_value(node);
		string photo_url = URL_TWITTER_UPLOADER +"uploads/"  +created_file;					
		string message = "@" +username +" check your search result here " +photo_url;
		printf(">>>>>>> %s\n", message.c_str());
	
		twitter.statusesUpdate(message);

		printf("status update response: %s\n", twitter.getResponse().c_str());

		// handle the result of the status update.
		bool success = true;				
		long response_code = app.getTwitter().getHTTPResponseCode();
		if(response_code == 403) {
			printf("Warning: we have reached our rate limit. We will sleep for a while  now ....\n");
			success = false;
		}
		else if(response_code == 401) {
			success = false;
			printf("Error: cannot do status update; we got unauthorized\n");
		}
		else if(response_code != 200) {
			success = false;
			printf("Error unhandled response code: %lu\n", response_code);
		}
		
		if(!success) {
			// we will wait on failure..
			printf("Warning: no success when uploading image.. sleeping now...\n");
			sleep(30);		
		}
		else {
			// okay we posted a new tweet; make sure we do not 
			// send it again.
			app.setSendQueueItemAsSend(queue_id);
		}
		json_decref(root);
	}
}
	

} // roxlu