<?php
require_once 'prepend.php';
$dewars->preExecute($_REQUEST);
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
		
		<!-- Styles -->
		<link href="/assets/css/dewarsadmin.css" rel="stylesheet" type="text/css" media="screen" />
		<script type="text/javascript" src="/assets/js/mootools/mootools-core-1.4.3.js"></script>
		<script type="text/javascript" src="/assets/js/mootools/mootools-more-1.4.0.1.js"></script>
		<script type="text/javascript" src="/assets/js/dewarsadmin.js"></script>
		<title>Dewars' admin</title>
	</head>
	
	<body>
	<div id="content">
		<?php
		if($dewars->isLoggedIn()) {
		?>
		<div id="menu_wrapper">
			<ul id="menu" class="menu">
				<li class="<?php if($_REQUEST['act'] == 'badwords') echo "active"; ?>"><a href="/?act=badwords"><span>Bad words</span></a></li>
				<li class="<?php if($_REQUEST['act'] == 'hashtags') echo "active"; ?>"><a href="/?act=hashtags"><span>Hash tags</span></a></li>
				<li class="<?php if($_REQUEST['act'] == 'commands') echo "active"; ?>"><a href="/?act=commands"><span>Commands</span></a></li>
			</ul>
		</div>
		<div id="wrapper">
		<?php
		}
		$dewars->execute($_REQUEST);
		?>
		</div>
		
	</div>
	</body>
</html>
