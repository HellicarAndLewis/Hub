Dewar's Hub Admin
------------------
I've created this admin for the Twitter part of the WailWall app. It's used 
onsite to maintain a list of "bad words" and "hash tags". The site is created
for use with an ipad / safari. 

The admin has 3 parts:

# Bad words: 	Add or remove words to the bad words list

# Hash tags: 	Create a list with tags you want the app to receive. Note,
				that when you add new tags and you want the app to receive these
				new tags, you must execute the "Update hashtags" command from
				the "Commands" menu.
				
# Commands:		This part sends a couple of OSC commands to the app 
				(See SharedCode/Twitter/TwitterOSCReceiver). 
				
				- Update hashtags: This will send an osc command to update the 
				list of hashtag we track; the hash tags can be any keyword 
				for which you want to receive tweets.
				
				- Update badwords: This will reload the list of bad words
				that is used to censor tweets in the app.
				
				- Simulate search: You can simulate a tweet w/o actually 
				doing a tweet.. used for developing.
