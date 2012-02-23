var admin = null;

var DewarsAdmin = new Class({
	sending_command: false
	
	,initialize: function() {
		this.initBadWordList();
		this.initTrackList();
		this.initCommands();
		this.initTwitterStatusCheck();
	}
	
	// --------------------------------------------------------------------
	// T R A C K 
	// --------------------------------------------------------------------
	,initTrackList: function() {
		var list = $('track_list');
		if(!list) {
			return;
		}
		
		$('new_track_button').addEvent('click',function() {
			var req = new Request.JSON({
					url:"/?act=new_track"
					,onSuccess:function(id) {
						document.location.href = "/?act=hashtags#W" +id
						document.location.reload(true);
					}
			}).post({
				name:$('new_track').get('value').toLowerCase()
			});
		});
		$('new_track').focus();
		
		$('new_track').addEvent('keydown',function(e){
				if(e.key == "enter") {
					$('new_track_button').fireEvent('click');
					e.stop();
					return false;
				}
				
		});
		
		$$('span.remove_button').addEvent('click', function() {
			var c = confirm("Sure?");
			if(c) {
				var button = this;
				var track_id = this.get("rel");
				var req = new Request.JSON({
					url:"/?act=remove_track"
					,onSuccess:function(r) {
						button.getParent('tr').destroy();
					}
				}).post({
					id:track_id
				});
			}
		});
		if(document.location.hash) {
			list.highlight();
		}
		this.setUpTrackButtons();
	}
	
	,setUpTrackButtons: function() {
		$$('.up_button').addEvent('click',function(){
			document.getElement('body').scrollTo(0,0);
		});
	}
	
	// --------------------------------------------------------------------
	// B A D W O R D S
	// --------------------------------------------------------------------
	,initBadWordList: function() {
		var list = $('badword_list');
		if(!list) {
			return;
		}
				
		$('new_badword_button').addEvent('click',function() {
			var req = new Request.JSON({
					url:"/?act=new_badword"
					,onSuccess:function(id) {
						document.location.href = "/?act=badwords#W" +id
						document.location.reload(true);
					}
			}).post({
				word:$('new_badword').get('value').toLowerCase()
			});
		});
		
		$$('span.remove_button').addEvent('click', function() {
			var c = confirm("Sure?");
			if(c) {
				var button = this;
				var word_id = this.get("rel");
				var req = new Request.JSON({
					url:"/?act=remove_badword"
					,onSuccess:function(r) {
						button.getParent('tr').destroy();
					}
				}).post({
					id:word_id
				});
			}
		});
		if(document.location.hash) {
			list.highlight();
		}
		this.setUpBadWordButtons();
	}
	
	,setUpBadWordButtons: function() {
		$$('.up_button').addEvent('click',function(){
			document.getElement('body').scrollTo(0,0);
		});
	}
	
	// --------------------------------------------------------------------
	// C O M M A N D S
	// --------------------------------------------------------------------
	,initCommands: function() {
		var me = this;
		if(me.sending_command) {
			alert("Already sending the command....");
		}
		var button = null;
		var show = function() {
			button.fade('in');
		}
		$$('span.command').addEvent('click', function() {
			this.fade('out');
			button = this;
			me.sending_command = true;
			var data = '';
			if(this.hasClass("search")) {
				data = this.getParent('div').getElement('input.search_term').get('value');
			}
			else if(this.hasClass("remove_tweet")) {
				data = this.getParent('div').getElement('input.tweet_id').get('value');
			}
			var req = new Request.JSON({
					url:"/?act=do_command"
					,onSuccess:function(id) {
						show.delay(500);
						me.sending_command = false;
					}
			}).post({
				command:this.get('rel')
				,data:data
			});
		});
	}
	
	// --------------------------------------------------------------------
	// T W I T T E R  S T A T U S
	// --------------------------------------------------------------------
	,initTwitterStatusCheck: function() {
		var prev = -1;
		var check = function() {
			var req = new Request.JSON({
					url:"/?act=twitter_status"
					,onSuccess:function(r) {
						if(r != prev) {
							$('twitter_status').set('class',(r) ? 'connected' : 'disconnected');
							prev = r;
						}
						
					}
				}).post();
		}
		//check.periodical(500);
	}
	
});
document.addEvent('domready',function() {
	admin = new DewarsAdmin();
});
