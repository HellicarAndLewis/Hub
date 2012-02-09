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

		bool use_twitter = true;
		if(use_twitter) {
			// USING TWITTER MEDIA UPLOAD
			string message = "@" +ufi.username +" Thanks for your input. See the visual result here: "; 
			//twitter.statusesUpdateWithMedia(message, ufi.file);
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
		}
		
	}
}
	

} // roxlu