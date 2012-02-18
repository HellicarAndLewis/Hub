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


void TwitterPhotoUploader::threadedFunction() {
	size_t  num = 0;
	
	while(true) {
		/*
		num = upload_queue.size();
		if(num == 0) {
			sleep(3); 
			continue;
		}
	
		// get one entry from queue.
		lock();
			deque<UploadFileInfo>::iterator it = upload_queue.begin();
			UploadFileInfo ufi = *it;
			upload_queue.erase(it);
		unlock();
		*/
		string username;
		string filepath;
		int queue_id;

		// TODO: fix db		
		bool result = app.getNextSendItemFromSendQueue(username, filepath, queue_id);
		if(!result) {
			sleep(2);
			continue;
		}
		
		printf("Got an item from queue: %s %s %d\n", username.c_str(), filepath.c_str(), queue_id);
		sleep(4);
		
		if(!ofFile::doesFileExist(filepath, true)) {
			printf("Error: cannot find file to upload: %s\n", filepath.c_str());
			//lock();
				// TODO: fix db
			app.setSendQueueItemAsSend(queue_id);
			//unlock();
			continue;
		}
		
		bool use_twitter = false;
		if(use_twitter) {
			// USING TWITTER MEDIA UPLOAD
			string message = "@" +username +" Thanks for your input. See the visual result here: "; 
			app.getTwitter().statusesUpdateWithMedia(message, filepath);
		}
		else {
		
			// USING CUSTOM WEBSITE
			rc::Request req(URL_TWITTER_UPLOADER);
			req.getParams().addString("act", "upload");
			req.getParams().addFile("photo", ofToDataPath(filepath, false));
			req.getParams().addString("user", username);
			req.getParams().addString("message", "message");

			string response;
			req.doPost(uploader_curl, response, true);

			
			// we get a json string from the server.
			bool correctly_uploaded = false;
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
					string msg = "no message found.";
					if(json_is_string(node)) {
						msg = json_string_value(node);						
					}

					//printf("Upload result: %s, %d\n", msg.c_str(), result);
				}
			
			}
			
			// when correctly uploaded we can do a tweet
			if(correctly_uploaded) {
			
				// get filepath
				string created_file = "";
				json_t* filepath_node = json_object_get(root, "created_file");
				if(filepath_node != NULL && json_is_string(filepath_node)) {
					created_file = json_string_value(filepath_node);
				}
				
				// get hash
				json_t* node = json_object_get(root, "file_hash");
				if(node != NULL && json_is_string(node)) {
					string file_hash = json_string_value(node);
					//string photo_url = URL_TWITTER_UPLOADER +"?act=img&hash=" +file_hash;
					string photo_url = URL_TWITTER_UPLOADER +"" +created_file;
					
					string message = "Hi @" +username +" check your search result here " +photo_url;
					printf("+++++++++++++ Tweet:  %s\n", message.c_str());
					app.getTwitter().statusesUpdate(message);
					sleep(30);	
				}
				
				
			}
			json_decref(root);
			printf("Response: %s\n", response.c_str());
		}
	}
}
	

} // roxlu