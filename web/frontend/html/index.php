<?php
require_once 'prepend.php';
$dewars->execute($_REQUEST);
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
	
		<!-- Styles -->
		<link href="/assets/css/dewars.css" rel="stylesheet" type="text/css" media="screen" />
		<!--[if lte IE 7]><![endif]-->
		<!--[if lte IE 8]><![endif]-->
		<!--[if lte IE 9]><![endif]-->
		
		<!-- Mootools -->
		<script type="text/javascript" src="/assets/js/mootools/mootools-core-1.4.3.js"></script>
		<script type="text/javascript" src="/assets/js/mootools/mootools-more-1.4.0.1.js"></script>
		<script type="text/javascript" src="/assets/js/dewars.js"></script>
		
		<title>Dewars' Hub</title>
	</head>
	<body>
	</body>
	
	<div id="site">
		<div id="last_container">
		<?php
			$home_images = $dewars->getHomeImages();
			echo $home_images;
		?>
		</div>
	</div>
	<div id="topbar">
		<div id="topbar_list_container">
			<ul id="topbar_list">
			<?php
				echo $dewars->getTopBarTweets();
			?>
			</ul>
		</div>
		<div id="topbar_prev"></div>
		<div id="topbar_next"></div>
	</div>
	<div id="disabler">
	</div>
	<div id="image_popup">
		<div class="close_button" id="close_image_popup"></div>
		
		<div id="image_container">
			<div id="big_prev"></div>
			<div id="big_next"></div>
			<div id="big_image_wrapper">
				<img id="big_image" src="#" />
				<div id="image_description">
					<div class="user left">
						<!-- https://api.twitter.com/1/users/profile_image?screen_name=' .$row['user'] .'&size=bigger -->
						<img class="user_image" src="https://api.twitter.com/1/users/profile_image?screen_name=roxlu" alt="" title="" />
					</div>
					<p> 
						<span class="username"></span><br />
						<span class="message"></span>
					</p>
				</div>
			</div>
			
		</div>
	</div>
	<div id="footer"></div>
</body>
	
