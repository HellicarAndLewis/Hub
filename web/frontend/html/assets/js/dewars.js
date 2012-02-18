var dewars = null;

function disableSelect(el){			
    if(el.addEventListener){
        el.addEventListener("mousedown",disablerExec,"false");
    } else {
        el.attachEvent("onselectstart",disablerExec);
    }
}
 
function enableSelect(el){
    if(el.addEventListener){
	el.removeEventListener("mousedown",disablerExec,"false");
    } else {
        el.detachEvent("onselectstart",disablerExec);
    }
}
function disablerExec(e){
    if(e.preventDefault){ e.preventDefault(); }
    return false;
}

var Site = new Class({
	 disabler:null
	,last_entries:null
	,big_image:null
	,image_popup:null
	,image_popup_user_name:null
	,image_popup_message:null
	,image_popup_user_img: null
	,topbar:null
	,topbar_list:null
	,topbar_lis:null
	,topbar_tween:null
	,last_scroll:null
	
	
	,initialize: function() {
		this.disabler = $('disabler').hide();
		this.big_image = $('big_image');	
		this.last_entries = $('last');
		this.image_popup = $('image_popup').hide();
		this.topbar = $('topbar');
		this.topbar_list = $('topbar_list');
		this.topbar_tween = new Fx.Tween(this.topbar_list, {property:'left'});
		
		var desc = $('image_description');
		this.image_popup_user_name = desc.getElement('span.username');
		this.image_popup_message = desc.getElement('span.message');
		this.image_popup_user_image = desc.getElement('img.user_image');
			
		this.resizeDisabler();
		this.initListUpdater();
		this.initImagePopup();
		this.initTopBar();
		this.setListEvents();
		
		var me = this;
		window.addEvent('resize',function() {
			me.resize();
		});
	}
	,resize: function() {
		this.resizeDisabler();
		this.resizeImagePopup();
	}
	
	,resizeDisabler: function() {
		var s = document.getScrollSize();
		var w = document.getSize().x;
		this.disabler.setStyles({
			width:w +"px"
			,height: s.y +"px"
		});
	}
	
	,resizeImagePopup: function() {
		var s = document.getScrollSize();
		var w = document.getSize().x;
		this.image_popup.setStyles({
			 width:w +"px"
			,height: s.y +"px"
		});
	}
	
	
	,showDisabler: function() {
		document.getElement('body').addClass('disabled');
		this.disabler.set('tween',{duration:300});
		this.disabler.show().fade('hide').fade('in');
	}
	
	,hideDisabler:function() {
		document.getElement('body').removeClass('disabled');
		this.disabler.show().fade('out');
	}
	
	,initListUpdater:function() {
		var me = this;
		var li = null;
		
		var update = function() {
			li = me.last_entries.getElement('li');
			
			if(!li) {
				return;
			}
			var id = li.get('class').split(' ')[0].split('_')[1];
			var req = new Request.HTML({
				url:"/?act=new_row&id=" +id
				,onComplete:function(responseTree, responseEls, html) {
					if(html != '') {
						if(responseTree && responseTree[0]) {
							var el = responseTree[0].inject(me.last_entries, 'top');
							var slide = new Fx.Slide(el, {mode:'vertical'}).hide().slideIn();
							me.setListEvents();
							
							var lis = me.last_entries.getElements('li');
							if(lis.length > 10) {
								var last_li = lis[lis.length-1];
								last_li.destroy();
								lis[lis.length-2].addClass("bottom");
							}
						}
					}
				}
			}).send();
		}
		update.periodical(1500);		
	}
	
	,setListEvents: function() {
		var me = this;
		var thumbs = this.last_entries.getElements('li div.thumb');
		thumbs.removeEvents();
		thumbs.addEvent('click', function() {
			var src = this.getElement('img').get('src').split('/');
			var file = "/uploads/" +src[3] +"/" +src[4];
			var li = this.getParent('li');
			var username = li.getElement('div.user img').get('alt');
			var message = li.getElement('p span.message').get('text');
			
			// check if there exists an image in the topbar for the clicked one
			var tiny_img_src = "/uploads/tiny/" +src[3] +"/" +src[4];
			var topbar_image = me.topbar_list.getElement('li img[src="' +tiny_img_src +'"]');
			
			// when the image exists in the topbar, set it as first and selected
			if(topbar_image) {
				var topbar_li = topbar_image.getParent('li');
				topbar_li.inject(me.topbar_list,'top');
				me.showImagePopup();
				me.setSelectedTopBarItem(topbar_li);
			}
			else {
				// show image.
				me.showImagePopup();
				me.showImage(file, username, message);
			}
		});
	}
	
	,initImagePopup: function() {
		var me = this;
		
		$('close_image_popup').addEvent('click', function(){
			me.hideImagePopup();
			me.hideDisabler();
		});
		
		// proxy click events
		$('big_prev').addEvent('click', function(){
			$('topbar_prev').fireEvent('click');
		});
		
		$('big_next').addEvent('click', function(){
			$('topbar_next').fireEvent('click');
		});
		
		
		$('big_next').fade('out');
		$('big_prev').fade('out');
					
		$('image_container').addEvents({
				mouseenter:function(){
					me.checkNav();
				}
				,mouseleave:function(){
					me.checkNav();
				}
		});
	}
	
	,checkNav: function() {
		var me = this;
		if(me.mustShowPrev()) {
			$('big_prev').fade('in');
			$('topbar_prev').fade('in');
		}
		else {
			$('big_prev').fade('out');
			$('topbar_prev').fade('out');
		}
		if(me.mustShowNext()) {
			$('big_next').fade('in');
			$('topbar_next').fade('in');
		}
		else {
			$('big_next').fade('out');
			$('topbar_next').fade('out');
		}
	}
	
	,mustShowPrev: function() {
		var li = this.topbar_list.getElement('li.selected');
		var lis = this.topbar_list.getElements('li');
		if(!li) {
			return;
		}
		
		var dx = lis.indexOf(li); 
		return (dx > 0);
	}
	,mustShowNext: function() {
		var li = this.topbar_list.getElement('li.selected');
		if(!li) {
			return false;
		}
		var lis = this.topbar_list.getElements('li');
		var dx = lis.indexOf(li);
		return true;
	}

	
	,hideImagePopup: function() {
		this.image_popup.fade('out');
		this.topbar.fade('out');
		document.getElement('body').scrollTo(this.last_scroll.x, this.last_scroll.y);
	}
	
	,showImagePopup: function() {
		this.resizeImagePopup();
		
		// show popup
		this.showDisabler();
		this.image_popup.show().fade('hide').fade('in');
		
		// show topbar
		this.topbar.show().fade('hide').fade('in');
		
		var body = document.getElement('body');
		this.last_scroll = body.getScroll();
		var timeout = function() {
			body.scrollTo(0,0);
		
		};
		setTimeout(timeout, 200);
		
	}	
	
	,showImage: function(file, username, message) {
		this.big_image.set('src', file);
		
		// set data	
		this.image_popup_user_name.set('text', username);
		this.image_popup_message.set('text', message);
		this.image_popup_user_image.set('src', 'https://api.twitter.com/1/users/profile_image?screen_name=' +username);
	
	}
	
	,initTopBar: function() {
		var me = this;
		this.setTopBarEvents();
		var reload = function(offset, ready) {
			var req = new Request.HTML({
							url:"/?act=get_topbar_tweets&offset=" +offset
							,onComplete:function(responseTree, responseEls, html) {
								if(html == "") {
									ready(false);
									return;
								}
								var len = responseTree.length;
								for(var i = 0; i < len; ++i) {
									if(responseTree[i]) {
										var el = responseTree[i].inject(me.topbar_list);
									}
								}
								me.setTopBarEvents();
								ready(true);
							}
			}).send();
		}
		
		// handle next button
		$('topbar_next').addEvent('click',function() {
			var li_sel = me.topbar_list.getElement('li.selected');
			if(li_sel) {
				var next = li_sel.getNext();
				if(next) {
					me.setSelectedTopBarItem(next);
				}
				else {
					var offset = me.topbar_list.getElements('li').length;
					var cur = this;
					reload(offset, function(succes) {
						if(succes) {
							me.setSelectedTopBarItem(li_sel.getNext());
						}
					});
				}
			}
		});
		
		// handle prev button
		$('topbar_prev').addEvent('click',function() {
			var li_sel = me.topbar_list.getElement('li.selected');
			if(li_sel) {
				var prev = li_sel.getPrevious();
				if(prev) {
					me.setSelectedTopBarItem(prev);
					var prev_prev = prev.getPrevious();
					if(!prev_prev) {
						$('big_prev').fade('out');
					}
				}
			}
			else {
				
			}
		});
		
		disableSelect(this.topbar);
	}
	
	,setTopBarEvents: function() {
		var me = this;
		this.topbar_lis = this.topbar_list.getElements('li');
		this.topbar_lis.removeEvents();
		this.topbar_lis.set('tween',{duration:200});
		this.topbar_lis.addEvents({
				click: function() {
					me.setSelectedTopBarItem(this);
				}
				,mouseenter:function() {
					this.fade('in');
				}
				,mouseleave:function() {
					if(!this.hasClass("selected")) {
						this.fade(0.5);
					}
				}
		});
		this.topbar_lis.setStyle('opacity',0.5);
		this.topbar_list.setStyle('width', (this.topbar_lis.length) * this.getTopBarItemWidth());
	}
	
	,getTopBarItemWidth:function() {
		return 55;
	}
	
	,getTopBarPageWidth: function() {
		return 825;
	}
	
	,setSelectedTopBarItem: function(li) {
		
		this.topbar_lis.removeClass("selected");
		
		// check if we need to tween the item into view
		var w = this.getTopBarPageWidth();
		var dx = this.topbar_lis.indexOf(li);
		var current_page = Math.abs(this.topbar_list.getStyle('left').toInt())/w;
		var li_page = (dx / 15).toInt();
		if(li_page != current_page) {
			var from = -(current_page*w);
			var to =  -(li_page*w);
			this.topbar_tween.start(from,to);
		}
		
		// set selected and correct HTML
		li.addClass("selected");
		li.fade('in');
		var img = li.getElement('img');
		var src = img.get('src').split('/');
		var img_src = "/uploads/"+src[3] +"/" +src[4];
		var username = img.get('alt');
		var message = li.getElement('p').get('text');		
		this.showImage(img_src, username, message);
		
		this.checkNav();
		this.resize();
	}
});

document.addEvent('domready', function() {
	dewars = new Site();
});
	
	
