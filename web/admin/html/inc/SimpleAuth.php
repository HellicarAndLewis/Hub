<?php
/** 
 * Simple authentication class. 
 * ---------------------------
 * - This class allowed you to show a HTTP auth popup.
 * - You can add any number of users by calling $auth->addUser(username, password)
 * - You can specify a list of IP addresses which can use http auth; when you add
 * an IP only this remote address will get a http auth; else isLoggedIn() will
 * always return false 
 * 
 * <example>
 *	$auth = new SimpleAuth();
 *	$auth->addUser('username','password');
 *	$auth->addIP('192.168.0.109');
 *	$auth->addIP('192.168.0.104');
 *	$auth->preAuth();
 *	if(!$auth->isLoggedIn()) {
 *		$auth->showPopup();
 *		exit;
 *	}
 * </example>
 * @author	Diederick Huijbers <diederick@apollomedia.nl>
 *
 * 	MAKE SURE TO ENABLE THE SESSION!!! session_start();
 */
class SimpleAuth {
	private $ips;
	private $users;
	private $logged_in;
	private $use_ip_checks;
	
	function __construct() {
		$this->ips = array();
		$this->users = array();
		$this->logged_in = false;
		$this->use_ip_checks = true;
	}
	
	public function dontUseIPChecks() {
		$this->use_ip_checks = false;
	}
	
	public function preAuth() {
		if(array_key_exists('simple_auth_logged_in', $_SESSION)
			&& $_SESSION['simple_auth_logged_in']) 
		{
			$this->logged_in = true;
			return true;
		}
		// only check something when user entered passwd.
		if(!isset($_REQUEST['AUTH_USER'])
			|| !isset($_REQUEST['AUTH_PW']))
		{
			$this->logged_in = false;
			return false;
		}
		
		// only check IPs when you want to 
		if($this->use_ip_checks) {
			if(count($this->ips) == 0) {
				$this->showIPError();
				exit;
			}
		}
		
		if(!is_array($this->users) || count($this->users) == 0) {
			die('User not found');
		}
					
		// check if IP address is in array
		if($this->use_ip_checks) {
			if(!$this->isRemoteUserInIPList()) {
				$this->showIPError();
				exit;
			}
		}
		
		// validate username
		foreach($this->users as $name => $pwd) {
			if($name == $_REQUEST['AUTH_USER'] && $pwd == $_REQUEST['AUTH_PW']) {
				$this->logged_in = true;
				$_SESSION['simple_auth_logged_in'] = true;
				return true;
			}
		}
		$this->logged_in = false;
		return false;
	}
	
	public function addIP($ip) {
		$this->ips[] = $ip;
	}
	
	public function addUser($user, $pass) {
		$this->users[$user] = $pass;
	}
	
	public function showPopup($message = 'Login', $realm = "Admin") {
		$this->showAuthPopup($message, $realm);
		exit;
	}
	
	private function showIPError() {
		die('No access. Please add the ip: ' .$_SERVER['REMOTE_ADDR'] .' to the allowed ID addresses');
	}
	
	public function isRemoteUserInIPList() {
		if(!is_array($this->ips) || count($this->ips) == 0) {
			return false;
		}
		if(empty($_SERVER['REMOTE_ADDR'])) {
			die('No remote ip address found!');
		}
		foreach($this->ips as $ip) {
			if($ip == $_SERVER['REMOTE_ADDR']) {
				return true;
			}
		}
		return false;
	}

	public function showAuthPopup($message, $realm = 'Admin') {
		//header('WWW-Authenticate: Basic realm="' .$realm.'"');
		//header('HTTP/1.0 401 Unauthorized');
		echo '<body onload="document.getElementById(\'username\').focus();">';
		echo '<form method="post" action="" style="position:relative;left:50px;top:50px;">';
		echo '<span style="font-family:Arial;display:inline-block;width:180px;font-size:20px;">Username:</span><input style="font-family:courier,monospace;padding:4px;font-size:16px" id="username" type="text" name="AUTH_USER"> <br/><br />';
		echo '<span style="font-family:Arial;display:inline-block;width:180px;font-size:20px;">Password:</span><input style="font-family:courier,monospace;padding:4px;font-size:16px" type="password" name="AUTH_PW"> <br/><br />';
		echo '<input type="submit" name="smlogin" value="OK" style="padding:20px;width:100px;height:60px;">';
		echo '</form>';
		echo '</body>';
		//echo $message;
		exit;
	}
	
	public function isLoggedIn() {
		return $this->logged_in;
	}
	
}
