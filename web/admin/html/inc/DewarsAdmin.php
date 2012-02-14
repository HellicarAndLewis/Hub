<?php
require_once 'functions.php';
require_once 'SimpleAuth.php';
require_once 'Database.php';
require_once 'OSC.php';

class DewarsAdmin {
	private $auth = null;
	private $db = null;
	private $config = null;
	
	function __construct($config) {
		$this->config = $config;
		
		// authorize
		$this->auth = new SimpleAuth();
		$this->auth->dontUseIPChecks();
		$this->auth->addUser("diederick", "diederick");
		$this->auth->preAuth();
		
		// database		
		$d = $config['db'];
		$this->db = new Database();
		$this->db->connect($d['host'], $d['database'], $d['username'], $d['password'] );
	}
	
	public function preExecute($req) {
		if(!$this->auth->isLoggedIn()) {
			$this->auth->showPopup();
			exit;
		}
		if(isset($req['act'])) {
			if($req['act'] == 'new_badword') {
				$new_id = $this->insertBadWord($req);
				if($new_id) {
					echo json_encode($new_id);
					exit;
				}
				exit;
			}
			else if($req['act'] == 'remove_badword') {
				$r = $this->removeBadWord($req['id']);
				echo json_encode($r);
				exit;
			}
			else if($req['act'] == 'new_track') {
				$new_id = $this->insertTrack($req);
				if($new_id) {
					echo json_encode($new_id);
					exit;
				}
				exit;
			}
			else if($req['act'] == 'remove_track') {
				$r = $this->removeTrack($req['id']);
				echo json_encode($r);
				exit;
			}
			else if($req['act'] == 'do_command') {
				echo json_encode($this->doCommand($req['command'], $req));
				exit;
			}
		}
	}
	
	public function execute($req) {
		if(isset($req['act'])) {
			if($req['act'] == 'badwords') {
				echo $this->getBadWordsList();
			}
			else if($req['act'] == 'hashtags') {
				echo $this->getTrackList();
			}
			else if($req['act'] == 'commands') {
				echo $this->getCommands();
			}
		}
	}
	
	public function isLoggedIn() {
		return $this->auth->isLoggedIn();
	}
	

	// -------------------------------------------------------------------------
	// T R A C K 
	// -------------------------------------------------------------------------
	private function getTrackList() {
		$rows = $this->db->fetchAll('select * from track order by name');
		if(!is_array($rows)) {
			return '';
		}
		$html = '<form method="post" action="#">'
				.'<table class="list" id="track_list">';
		$html .= '<tr>'
						.'<td class="title"><input type="text" name="name" id="new_track" class="text"></td>'
						.'<td class="remove"><span class="save_button" id="new_track_button">Add</td>'
						.'<td></td>'
					.'</tr>';
		foreach($rows as $row) {
			$html .= $this->getTrackRow($row);	
		}
		$html .= '</table>'
				.'</form>';
		return $html;
	}
	
	private function getTrackRow($row) {
		$html = '<tr>'
				.'<td class="title"><div><a name="W' .$row['id'] .'">' .$row['name'] .'</a></div></td>'
				.'<td class="remove"><span class="remove_button" rel="' .$row['id'] .'">Remove</span></td>'
				.'<td class="up"><span class="up_button">UP</span></td>'
			.'</tr>';
		return $html;
	}
	
	private function getTrack($id) {
		$values['id'] = $id;
		$row = $this->db->fetchRow('select * from track where id = :id', $values);
		return $row;
	}
	
	private function insertTrack($req) {
		if(!isset($req['name'])) {
			l('no name given');
			die('no name given');
		}
		$data['name'] = $req['name'];
		
		if($this->db->insert('track', $data)) {
			return $this->db->lastID();
		}
		return false;
	}
	
	private function removeTrack($id) {
		if(!is_numeric($id)) {
			return false;
		}
		$sql = 'delete from track where id = "' .$id .'"';
		$query = $this->db->prepare($sql);
		return $this->db->execute($query);
	}
	
	// -------------------------------------------------------------------------
	// B A D W O R D S
	// -------------------------------------------------------------------------
	private function getBadWordsList() {
		$rows = $this->db->fetchAll('select * from badwords order by word');
		if(!is_array($rows)) {
			return '';
		}
		$html = '<form method="post" action="#">'
				.'<table class="list" id="badword_list">';
		$html .= '<tr>'
						.'<td class="title"><input type="text" name="word" id="new_badword" class="text"></td>'
						.'<td class="remove"><span class="save_button" id="new_badword_button">Add</td>'
						.'<td></td>'
					.'</tr>';
		foreach($rows as $row) {
			$html .= $this->getBadWordsRow($row);	
		}
		$html .= '</table>'
				.'</form>';
		return $html;
	}
	
	private function getBadWordsRow($row) {
		$html = '<tr>'
				.'<td class="title"><div><a name="W' .$row['id'] .'">' .$row['word'] .'</a></div></td>'
				.'<td class="remove"><span class="remove_button" rel="' .$row['id'] .'">Remove</span></td>'
				.'<td class="up"><span class="up_button">UP</span></td>'
			.'</tr>';
		return $html;
	}
	
	private function getBadWord($id) {
		$values['id'] = $id;
		$row = $this->db->fetchRow('select * from badwords where id = :id', $values);
		return $row;
	}
	
	private function insertBadWord($req) {
		$data['word'] = $req['word'];
		if($this->db->insert('badwords', $data)) {
			return $this->db->lastID();
		}
		return false;
	}
	
	private function removeBadWord($id) {
		if(!is_numeric($id)) {
			return false;
		}
		$sql = 'delete from badwords where id = "' .$id .'"';
		$query = $this->db->prepare($sql);
		return $this->db->execute($query);
	}
	
	// -------------------------------------------------------------------------
	// C O M M A N D S
	// -------------------------------------------------------------------------
	private function getCommands() {
		$html = '<div id="commands">'
					.'<span class="command" rel="/twitter/reload_hashtags">Update hashtags</span>'
					.'<span class="command" rel="/twitter/reload_badwords">Update badwords</span>'
					.'<div class="fake_search">'
						.'<input type="text" class="search_term">'
						.'<span class="command search" rel="/twitter/simulate_search">Simulate search</span>'
					.'</div>'
				.'</div>';
		return $html;
	}
	private function doCommand($message, $req) {
		$osc = $this->config['osc'];
		if(empty($osc['host'])) {
			die('no osc host config found');
		}
		if(empty($osc['port'])) {
			die('no osc port config found');
		}
		$data = '';
		if($message == '/twitter/simulate_search') {
			$data = $req['data'];
		}
		$client = new OSCClient($osc['host'], $osc['port']);
		$msg = new OscMessage($message);
		$msg->add_arg($data, "s");
		$client->send($msg);
		return true;
		/*
		 $c->set_destination("192.168.1.5", 3980);

    $m1 = new OSCMessage("/test", array(new Timetag(3294967295, 5), new Infinitum(), new Blob("aoeuaoeu!")));
    $m1->add_arg(28658.93, "d");
    $m2 = new OSCMessage("/bar", array(1, 2, array(1,2,3)
		*/
	}
}


