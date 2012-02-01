#include "testApp.h"
namespace rtt = roxlu::twitter::type;

//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(33);
	
	setWords();
	int test = 1;
	if(test == 1) {
		if(!db.open(ofToDataPath("test1.db")))  {
			printf("Cannot create db");
			exit();
		}
		
		createTablesForTest1();
		vector<rtt::Tweet> found_tweets;
		//doTest1();
		findTest1(found_tweets);
	}

}

void testApp::createTablesForTest1() {
	// TWEETS
	bool result = db.query(
						   "CREATE TABLE IF NOT EXISTS tweets( "					\
						   " t_id			INTEGER PRIMARY KEY AUTOINCREMENT"		\
						   ",t_user_id		VARCHAR(50)"							\
						   ",t_text			INTEGER"								\
						   ",t_screen_name	VARCHAR(20)"							\
						   ",t_timestamp	TIMESTAMP"								\	
						   ",t_longitude	REAL"									\
						   ",t_latitude	REAL"										\
						   ");"
						   );
	
	if(!result) {
		printf("cannot create tables for test 1\n");
		exit();
	}
	
	// TAGS
	result = db.query(
					  "CREATE TABLE IF NOT EXISTS tweet_tags( "				\
					  " tt_id			INTEGER "							\
					  ",tt_tag			VARCHAR(50) "						\
					  ",PRIMARY KEY(tt_id, tt_tag) "
					  ");"
					  );
	
	if(!result) {
		printf("cannot create tables for test 1\n");
		exit();
	}
}

/*
Results Inserting/Selecting
-------------------------------------------------------------------------------
run 1:		tweets: 1.000.000		tweet_tags: 3.498.980 	time: 199308 ms
-------------------------------------------------------------------------------
> rows found  4820, looped over in  1492
> rows found  4765, looped over in  1367
> rows found   781, looped over in  1238
> rows found   999, looped over in  1464
> rows found   980, looped over in  1495
> rows found   691, looped over in  1226
> rows found  4798, looped over in  1318
> rows found  4977, looped over in  1390
> rows found   399, looped over in  1297
> rows found  4839, looped over in  1415
> rows found     0, looped over in  1282
> rows found  4777, looped over in  1448
> rows found    15, looped over in  1240
> rows found  4938, looped over in  1317
> rows found  4944, looped over in  1314
> rows found  4920, looped over in  1338
> rows found  4873, looped over in  1360
> rows found   742, looped over in  1833
> rows found  1054, looped over in  1655
> rows found  4790, looped over in  1487
> rows found  4837, looped over in  1632
> rows found     0, looped over in  1552
> rows found   363, looped over in  1411
> rows found  4923, looped over in  1387
> rows found    68, looped over in  1316
> rows found  5028, looped over in  2036
> rows found  4972, looped over in  1781
> rows found   593, looped over in  1243
> rows found  4908, looped over in  1602
> rows found  4805, looped over in  1350
> rows found     0, looped over in  1552
> rows found  4953, looped over in  1495
> rows found  2004, looped over in  1327
> rows found  4808, looped over in  1351
> rows found   365, looped over in  1229
> rows found  4810, looped over in  1289
> rows found  4838, looped over in  1343
> rows found  4788, looped over in  1332
> rows found   253, looped over in  1366
> rows found  1226, looped over in  1267
> rows found  4928, looped over in  1440
> rows found  4962, looped over in  1442
> rows found     0, looped over in  1263
> rows found  4822, looped over in  1309
> rows found   181, looped over in  1214
> rows found  4930, looped over in  1298
> rows found     0, looped over in  1343
> rows found  4845, looped over in  1501
> rows found    16, looped over in  1442
> rows found   176, looped over in  1638
It took 70736 to loop over  50 tags
-------------------------------------------------------------------------------
*/

void testApp::doTest1() {
	int num_tweets = 1000000;
	int num_tags = 1;
	vector<rtt::Tweet> tweets;
	
	createTweets(num_tweets, tweets);
	printf("ok :%d\n", tweets.size());
	int now = ofGetElapsedTimeMillis();
	int c = tweets.size();
	db.beginTransaction();
	for(int i = 0; i < c; ++i) {
		rtt::Tweet& tweet = tweets[i];

		// insert tweet
		// -------------
		db.insert("tweets")
			.use("t_text", tweet.text)
			.use("t_user_id", tweet.user_id)
			.use("t_screen_name", tweet.screen_name)
			.useTimestamp("t_timestamp")
			.execute();
		//printf("> %d\n", db.lastInsertID());
		
		// insert the tags.
		// ---------------
		num_tags = tweet.tags.size();
		for(int j = 0; j < num_tags; ++j) {
			db.insert("tweet_tags")
				.use("tt_id", db.lastInsertID())
				.use("tt_tag", tweet.tags[j])
				.execute();
		}
		//printf("> %s\n", tweet.text.c_str());
	}	
	db.endTransaction();
	int done = ofGetElapsedTimeMillis();
	int diff = done-now;
	printf("It took %d to insert %d queries\n", diff, num_tweets);
}

void testApp::findTest1(vector<rtt::Tweet>& result) {
	int now = ofGetElapsedTimeMillis();
	int find_num = 50;
	int word_count = words.size();
	int rows_found = 0;
	for(int i = 0; i < find_num; ++i) {
		string word = words[ofRandom(0,word_count-1)];
		QueryResult result(db);
		//  select datetime('now','localtime','+3.5 seconds','+10 minutes');
		int start = ofGetElapsedTimeMillis();
		db.select("t_text, t_user_id, t_timestamp,  datetime('now', '-1.5 hours') as test")
			.from("tweets")
			.join("tweet_tags on tt_id = t_id")
			.where("tt_tag = :tt_tag and t_timestamp > datetime('now', '-1.5 hours')")
			.limit(100)
			//.order("t_timestamp DESC")
			.use("tt_tag", word)
			.execute(result);
			
		rows_found = 0;
		//result.next();
		
		while(result.next()) {	
			printf("%s > %s\n", result.getString(2).c_str(), result.getString(3).c_str());
			++rows_found;
		}
		
		int end = ofGetElapsedTimeMillis();
		int row_diff = end - start;
		
		printf("> rows found %5d, looped over in %5d\n", rows_found, row_diff);
	}
	int done = ofGetElapsedTimeMillis();
	int diff = done-now;
	printf("It took %d to loop over  %d tags\n",diff, find_num);
}


void testApp::createTweets(int num, vector<rtt::Tweet>& result) {
	int num_words = 9; // pick 5 words to create text for tweet
	int num_hashtags = 0;
	int word_count = words.size();
	for(int i = 0; i < num; ++i) {
		rtt::Tweet tweet;
		tweet.user_id = ofToString(ofRandom(1,100000)); // random pool of 100.000 users
		tweet.screen_name = words[ofRandom(0,word_count-1)];
		
		
		// create nonsense tweet text
		for(int j = 0; j < num_words; ++j) {
			tweet.text.append(words[ofRandom(0,word_count-1)]);
			tweet.text.append(" ");
		}
		
		// create hashtags.
		num_hashtags = ofRandom(1,7);
		for(int j = 0; j < num_hashtags; ++j) {
			tweet.tags.push_back(words[ofRandom(0,word_count-1)]);
			
		}
		result.push_back(tweet);
	}
}

void testApp::createTablesForTest2() {
}


void testApp::doTest2() {
}

//--------------------------------------------------------------
void testApp::update(){
	
}

//--------------------------------------------------------------
void testApp::draw(){
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
	
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 
	
}


void testApp::setWords() {
	words.push_back("scioperata");
	words.push_back("scioperate");
	words.push_back("scioperati");
	words.push_back("scioperato");
	words.push_back("scioperava");
	words.push_back("scioperavi");
	words.push_back("scioperavo");
	words.push_back("scioperer");
	words.push_back("scioperi");
	words.push_back("scioperino");
	words.push_back("sciopero");
	words.push_back("sciophilous");
	words.push_back("sciophyte");
	words.push_back("scioptic");
	words.push_back("sciopticon");
	words.push_back("scioptics");
	words.push_back("scioptric");
	words.push_back("sciosophist");
	words.push_back("sciosophy");
	words.push_back("sciot");
	words.push_back("scioterical");
	words.push_back("scioterique");
	words.push_back("sciotheism");
	words.push_back("sciotheric");
	words.push_back("sciotherical");
	words.push_back("sciotherically");
	words.push_back("scious");
	words.push_back("scirenga");
	words.push_back("scirophoria");
	words.push_back("scirophorion");
	words.push_back("sciroppi");
	words.push_back("sciroppo");
	words.push_back("scirpus");
	words.push_back("scirrhi");
	words.push_back("scirrhogastria");
	words.push_back("scirrhoid");
	words.push_back("scirrhoma");
	words.push_back("scirrhosis");
	words.push_back("scirrhous");
	words.push_back("scirrhus");
	words.push_back("scirrosity");
	words.push_back("scirtopod");
	words.push_back("scirtopoda");
	words.push_back("scirtopodous");
	words.push_back("scissel");
	words.push_back("scissible");
	words.push_back("scissile");
	words.push_back("scission");
	words.push_back("scissione");
	words.push_back("scissioni");
	words.push_back("scissiparity");
	words.push_back("scissor");
	words.push_back("scissorbill");
	words.push_back("scissorbird");
	words.push_back("scissorer");
	words.push_back("scissoring");
	words.push_back("scissorium");
	words.push_back("scissorlike");
	words.push_back("scissorlikeness");
	words.push_back("scissors");
	words.push_back("scissorsbird");
	words.push_back("scissorsmith");
	words.push_back("scissorstail");
	words.push_back("scissortail");
	words.push_back("scissorwise");
	words.push_back("scissura");
	words.push_back("scissure");
	words.push_back("scissurella");
	words.push_back("scissurellid");
	words.push_back("scissurellidae");
	words.push_back("scitaminales");
	words.push_back("scitamineae");
	words.push_back("sciup");
	words.push_back("sciupa");
	words.push_back("sciupai");
	words.push_back("sciupammo");
	words.push_back("sciupando");
	words.push_back("sciupano");
	words.push_back("sciupare");
	words.push_back("sciuparono");
	words.push_back("sciupasse");
	words.push_back("sciupassi");
	words.push_back("sciupaste");
	words.push_back("sciupasti");
	words.push_back("sciupata");
	words.push_back("sciupate");
	words.push_back("sciupati");
	words.push_back("sciupato");
	words.push_back("sciupava");
	words.push_back("sciupavamo");
	words.push_back("sciupavano");
	words.push_back("sciupavate");
	words.push_back("sciupavi");
	words.push_back("sciupavo");
	words.push_back("sciuper");
	words.push_back("sciuperai");
	words.push_back("sciuperei");
	words.push_back("sciuperemo");
	words.push_back("sciuperete");
	words.push_back("sciupi");
	words.push_back("sciupiamo");
	words.push_back("sciupiate");
	words.push_back("sciupino");
	words.push_back("sciupo");
	words.push_back("sciurid");
	words.push_back("sciuridae");
	words.push_back("sciurine");
	words.push_back("sciuroid");
	words.push_back("sciuromorph");
	words.push_back("sciuromorpha");
	words.push_back("sciuromorphic");
	words.push_back("sciuropterus");
	words.push_back("sciurus");
	words.push_back("scivol");
	words.push_back("scivola");
	words.push_back("scivolai");
	words.push_back("scivolammo");
	words.push_back("scivolando");
	words.push_back("scivolano");
	words.push_back("scivolare");
	words.push_back("scivolasse");
	words.push_back("scivolassi");
	words.push_back("scivolaste");
	words.push_back("scivolasti");
	words.push_back("scivolata");
	words.push_back("scivolate");
	words.push_back("scivolati");
	words.push_back("scivolato");
	words.push_back("scivolava");
	words.push_back("scivolavi");
	words.push_back("scivolavo");
	words.push_back("scivoler");
	words.push_back("scivolerai");
	words.push_back("scivolerei");
	words.push_back("scivoli");
	words.push_back("scivoliamo");
	words.push_back("scivoliate");
	words.push_back("scivolino");
	words.push_back("scivolo");
	words.push_back("sclaff");
	words.push_back("sclate");
	words.push_back("sclater");
	words.push_back("sclav");
	words.push_back("sclavonian");
	words.push_back("sclaw");
	words.push_back("scler");
	words.push_back("sclera");
	words.push_back("scleral");
	words.push_back("scleranth");
	words.push_back("scleranthaceae");
	words.push_back("scleranthus");
	words.push_back("scleratogenous");
	words.push_back("sclere");
	words.push_back("sclerectasia");
	words.push_back("sclerectomy");
	words.push_back("scleredema");
	words.push_back("sclereid");
	words.push_back("sclerema");
	words.push_back("sclerencephalia");
	words.push_back("sclerenchyma");
	words.push_back("sclerenchymatous");
	words.push_back("sclerenchyme");
	words.push_back("sclererythrin");
	words.push_back("scleretinite");
	words.push_back("scleria");
	words.push_back("scleriasis");
	words.push_back("sclerification");
	words.push_back("sclerify");
	words.push_back("sclerite");
	words.push_back("scleritic");
	words.push_back("scleritis");
	words.push_back("sclerized");
	words.push_back("sclerobase");
	words.push_back("sclerobasic");
	words.push_back("scleroblast");
	words.push_back("scleroblastema");
	words.push_back("scleroblastemic");
	words.push_back("scleroblastic");
	words.push_back("sclerocauly");
	words.push_back("sclerochorioiditis");
	words.push_back("sclerochoroiditis");
	words.push_back("scleroconjunctival");
	words.push_back("scleroconjunctivitis");
	words.push_back("sclerocornea");
	words.push_back("sclerocorneal");
	words.push_back("sclerodactylia");
	words.push_back("sclerodactyly");
	words.push_back("scleroderm");
	words.push_back("scleroderma");
	words.push_back("sclerodermaceae");
	words.push_back("sclerodermata");
	words.push_back("sclerodermatales");
	words.push_back("sclerodermatitis");
	words.push_back("sclerodermatous");
	words.push_back("sclerodermi");
	words.push_back("sclerodermia");
	words.push_back("sclerodermic");
	words.push_back("sclerodermite");
	words.push_back("sclerodermitic");
	words.push_back("sclerodermitis");
	words.push_back("sclerodermous");
	words.push_back("sclerogen");
	words.push_back("sclerogeni");
	words.push_back("sclerogenoid");
	words.push_back("sclerogenous");
	words.push_back("scleroid");
	words.push_back("scleroiritis");
	words.push_back("sclerokeratitis");
	words.push_back("sclerokeratoiritis");
	words.push_back("scleroma");
	words.push_back("scleromata");
	words.push_back("scleromeninx");
	words.push_back("scleromere");
	words.push_back("sclerometer");
	words.push_back("sclerometric");
	words.push_back("scleronychia");
	words.push_back("scleronyxis");
	words.push_back("scleropages");
	words.push_back("scleroparei");
	words.push_back("sclerophthalmia");
	words.push_back("sclerophyll");
	words.push_back("sclerophyllous");
	words.push_back("sclerophylly");
	words.push_back("scleroprotein");
	words.push_back("sclerosal");
	words.push_back("sclerosarcoma");
	words.push_back("scleroscope");
	words.push_back("sclerose");
	words.push_back("sclerosed");
	words.push_back("scleroseptum");
	words.push_back("sclerosis");
	words.push_back("scleroskeletal");
	words.push_back("scleroskeleton");
	words.push_back("sclerospora");
	words.push_back("sclerostenosis");
	words.push_back("sclerostoma");
	words.push_back("sclerostomiasis");
	words.push_back("sclerotal");
	words.push_back("sclerote");
	words.push_back("sclerotia");
	words.push_back("sclerotial");
	words.push_back("sclerotic");
	words.push_back("sclerotica");
	words.push_back("sclerotical");
	words.push_back("scleroticectomy");
	words.push_back("scleroticochorioiditis");
	words.push_back("scleroticochoroiditis");
	words.push_back("scleroticonyxis");
	words.push_back("scleroticotomy");
	words.push_back("sclerotinia");
	words.push_back("sclerotinial");
	words.push_back("sclerotiniose");
	words.push_back("sclerotioid");
	words.push_back("sclerotitic");
	words.push_back("sclerotitis");
	words.push_back("sclerotium");
	words.push_back("sclerotized");
	words.push_back("sclerotoid");
	words.push_back("sclerotome");
	words.push_back("sclerotomic");
	words.push_back("sclerotomy");
	words.push_back("sclerous");
	words.push_back("scleroxanthin");
	words.push_back("sclerozone");
	words.push_back("scliff");
	words.push_back("sclim");
	words.push_back("sclimb");
	words.push_back("scm");
	words.push_back("scndendo");
	words.push_back("scoad");
	words.push_back("scob");
	words.push_back("scobby");
	words.push_back("scobicular");
	words.push_back("scobiform");
	words.push_back("scobs");
	words.push_back("scodella");
	words.push_back("scodelle");
	words.push_back("scodinzol");
	words.push_back("scodinzola");
	words.push_back("scodinzoli");
	words.push_back("scodinzolo");
	words.push_back("scoff");
	words.push_back("scoffer");
	words.push_back("scoffery");
	words.push_back("scoffing");
	words.push_back("scoffingly");
	words.push_back("scoffingstock");
	words.push_back("scofflaw");
	words.push_back("scog");
	words.push_back("scoggan");
	words.push_back("scogger");
	words.push_back("scoggin");
	words.push_back("scogginism");
	words.push_back("scogginist");
	words.push_back("scogli");
	words.push_back("scogliera");
	words.push_back("scogliere");
	words.push_back("scoglio");
	words.push_back("scoiattoli");
	words.push_back("scoiattolo");
	words.push_back("scoinson");
	words.push_back("scoke");
	words.push_back("scolara");
	words.push_back("scolare");
	words.push_back("scolari");
	words.push_back("scolaro");
	words.push_back("scolb");
	words.push_back("scold");
	words.push_back("scoldable");
	words.push_back("scoldenore");
	words.push_back("scolder");
	words.push_back("scolding");
	words.push_back("scoldingly");
	words.push_back("scoleces");
	words.push_back("scoleciasis");
	words.push_back("scolecid");
	words.push_back("scolecida");
	words.push_back("scoleciform");
	words.push_back("scolecite");
	words.push_back("scolecoid");
	words.push_back("scolecology");
	words.push_back("scolecophagous");
	words.push_back("scolecospore");
	words.push_back("scoleryng");
	words.push_back("scolex");
	words.push_back("scoli");
	words.push_back("scolia");
	words.push_back("scolices");
	words.push_back("scoliid");
	words.push_back("scoliidae");
	words.push_back("scoliograptic");
	words.push_back("scoliokyposis");
	words.push_back("scoliometer");
	words.push_back("scolion");
	words.push_back("scoliorachitic");
	words.push_back("scoliosis");
	words.push_back("scoliotic");
	words.push_back("scoliotone");
	words.push_back("scolite");
	words.push_back("scollata");
	words.push_back("scollate");
	words.push_back("scollati");
	words.push_back("scollato");
	words.push_back("scollop");
	words.push_back("scolo");
	words.push_back("scolog");
	words.push_back("scolopaceous");
	words.push_back("scolopacidae");
	words.push_back("scolopacine");
	words.push_back("scolopax");
	words.push_back("scolopendra");
	words.push_back("scolopendrella");
}