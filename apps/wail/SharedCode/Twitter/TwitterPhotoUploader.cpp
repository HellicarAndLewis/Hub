#include "TwitterPhotoUploader.h"
namespace roxlu {

TwitterPhotoUploader::TwitterPhotoUploader(rt::Twitter& twitter) 
	:twitter(twitter)
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

		bool use_twitter = false;
		if(use_twitter) {
			// USING TWITTER MEDIA UPLOAD
			string message = "@" +ufi.username +" Thanks for your input. See the visual result here: "; 
			twitter.statusesUpdateWithMedia(message, ufi.file);
		}
		else {
			// USING CUSTOM WEBSITE
			rc::Request req(URL_TWITTER_UPLOADER);
			req.getParams().addString("act", "upload");
			req.getParams().addFile("photo", ufi.file);
			req.getParams().addString("user", ufi.username);
			req.getParams().addString("message", ufi.message);
		
			string response;
			req.doPost(uploader_curl, response, true);
			
			// we get a json string from the server.
			json_t *root;
			json_error_t error;
			root = json_loads(response.c_str(), 0, &error);
			if(!root) {
				printf("Error: cannot upload image to remote server. The result we get is:\n%s\n", response.c_str());
				json_decref(root);
				exit(0);
			}
			else {
				json_t* node = json_object_get(root, "result");
				if(!json_is_boolean(node)) {
					printf("Error: cannot parse the result from the remote server for uploading. \n");
				}
				else {
					bool result = json_is_true(node);
					node = json_object_get(root, "msg");
					string msg = "no message found.";
					if(json_is_string(node)) {
						msg = json_string_value(node);						
					}
					printf("Upload result: %s, %d\n", msg.c_str(), result);
				}
				json_decref(root);
			}
			printf("Response: %s\n", response.c_str());
		}
	}
}
	

} // roxlu