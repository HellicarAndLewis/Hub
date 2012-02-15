<?php
require_once 'functions.php';
require_once 'Database.php';

class Dewars {
	private $config = null;
	private $db = null;

	function __construct($config) {
		$this->config = $config;
		
		// connect to db.
		$d = $config['db'];
		$this->db = new Database();
		$this->db->connect($d['host'], $d['database'], $d['username'], $d['password']);
	}
	
	public function execute($req) {
		if(isset($req['act'])) {
			
			if($req['act'] == 'upload') {
				$this->handleUpload($req);
				exit;			
			}
			else if($req['act'] == 'new_row') {
				echo ($this->getNewRow($req['id']));
				exit;
			}
			else if($req['act'] == 'get_topbar_tweets') {
				echo $this->getTopBarTweets($req['offset']);
				exit;
			}
			else if($req['act'] == 'validate') {
				$dir = $this->config['upload_dir'];
				
				// tiny dir
				$tiny_dir = $dir .'/tiny';
				if(!is_dir($tiny_dir)) {
					mkdir($tiny_dir, 0777);
					chmod($tiny_dir, 0777);
					if(!is_dir($tiny_dir)) {
						die('cannot create dir: ' .$tiny_dir);
					}
				}
				
				// thumb dir
				$tiny_dir = $dir .'/thumb';
				if(!is_dir($tiny_dir)) {
					mkdir($tiny_dir, 0777);
					chmod($tiny_dir, 0777);
					if(!is_dir($tiny_dir)) {
						die('cannot create dir: ' .$tiny_dir);
					}
				}
				/*
				$dir = $this->config['upload_dir'] .'/super';
				if(!is_dir($dir)) {
					mkdir($dir, 0777);
					chmod($dir, 0777);
					die('nope');
				}
				*/
			}
		}
	}
		
	public function getHomeImages() {
		//fetchAll($table, $fieldString, $whereFields) {
		$query = $this->db->query('select user,message,file,id, date_format(date, "%m.%d") as dir from uploads order by id desc limit 15');
		$rows = $this->db->execute($query);
		if(!is_array($rows)) {
			return '';
		}
		$html = '<ul id="last">' ."\n";
		$c = 0;
		$num = count($rows);
		foreach($rows as $row) {
			++$c;
			$html .= $this->createListRow($row, ($c == $num), ($c == 1), true);
		}
		$html .= '</ul>';
		return $html;
	}
	
	// get a new <li> 
	public function getNewRow($lastID) {
		if(!is_numeric($lastID)) {
			return false;
		}
		$query = $this->db->query('select user,message,file,id, date_format(date, "%m.%d") as dir from uploads where id > ' .$lastID .' order by id desc limit 1');
		if(!$query) {
			return false;
		}
		
		$row = $this->db->execute($query);
		if(!$row) {
			return false;
		}
		return $this->createListRow($row[0], false, true, true);
	}
	
	// create a row for the homepage list
	private function createListRow($row, $last, $first, $createLI) {
		$html = '';
		if($createLI) {
			$html .= '<li class="cube_' .$row['id'] .' '  .($last ? 'bottom' : (($first) ? 'top' : 'row')) .'">';
		}
		
		$html .=			'<div class="user left">'
							.'<img src="https://api.twitter.com/1/users/profile_image?screen_name=' .$row['user'] .'&size=bigger" alt="' .$row['user'] .'" title="" />'
						.'</div>'
						.'<p>' 
								.'<span class="username">' .$row['user'] .'</span><br />'
								.'<span class="message">' .$row['message'] .'</span>' 
						.'</p>'
						.'<div class="thumb right">'
							.'<img  src="/uploads/thumb/' .$row['dir'] .'/' .$row['file'] .'" alt="" title="" />'
						.'</div>';
		if($createLI) {
			$html .='</li>' ."\n";
		}
		return $html;
	}
	
	// get the HTML (lis) for the top bar tweets
	public function getTopBarTweets($offset = 0) {
		$query = $this->db->query('
				select 	user 
						,message
						,file
						,id
						,date_format(date, "%m.%d") as dir 
				from 	uploads 
				order 	by id desc 
				limit ' .$offset  .', 15');
		
		$rows = $this->db->execute($query);
		if(!is_array($rows)) {
			return '';
		}
		$html = '';
		foreach($rows as $row) {
			$html .= '<li class="item">'
						.'<img src="/uploads/tiny/' .$row['dir'] .'/' .$row['file'] .'" alt="' .$row['user'] .'" >'
						.'<p>' .$row['message'] .'</p>'
					.'</li>' ."\n";
		}
		return $html;
	}
	
	private function handleUpload($req) {
		// Did we get a upload error.
		if($_FILES['photo']['error'] != UPLOAD_ERR_OK) {
			$result['result'] = false;
			$result['msg'] = 'got error code from $_FILES: ' .$_FILES['photo']['error'];
			echo json_encode($result);
			exit;
		}
			
		// check if we can upload the file.
		if(!is_writable($this->config['upload_dir'])) {
			$result['result'] = false;
			$result['msg'] = 'upload dir is not writable';
			echo json_encode($resul);
			exit;
		}
			
		// create random filename
		$random = null;
		for ($i = 0; $i < 12; $i++) {
			$random .= chr(rand(97, 122));
		}
		$dir = date('m.d', time());
		
		// upload
		$up = $_FILES['photo'];
		$dest = $this->config['upload_dir'] .'/' .$dir .'/';
		if(!is_dir($dest)) {
			mkdir($dest,0777);
			chmod($dest, 0777);
		}
		$filename = date('H_i', time()) .'_' .$random .'.png';
		$dest .= $filename;
		
		$moved = move_uploaded_file($up['tmp_name'], $dest);
		chmod($dest, 0777);
			
		if(!$moved) {
			$result['result'] = false;
			$result['msg'] = 'error while moving file';
			echo json_encode($result);
			exit;
		}
		// insert into db.
		$fields = array(
			'file' 		=> $filename
			,'user' 		=> $req['user']
			,'message' 	=> $req['message']
		);
		$this->db->insert('uploads', $fields);
		
		// create thumbs
		$this->createThumbnails($dest, $dir,  $filename);
		
		// success
		$result['result'] = true;
		$result['msg'] = 'success';
		echo json_encode($result);
		exit;
	}
	
	private function createThumbnails($file, $subdir, $filename) {
		$thumbs = array(
			array(
				 'width' => 150
				,'height' => 150
				,'dir' => $this->config['upload_dir'] .'/thumb/' .$subdir .'/'
				,'file' => $file
			)
			,array(
				 'width' => 48
				,'height' => 48
				,'dir' => $this->config['upload_dir'] .'/tiny/' .$subdir .'/'
				,'file' => $file
			)
		);
		
		foreach($thumbs as $thumb) {
			if(!is_dir($thumb['dir'])) {
				mkdir($thumb['dir'], 0777);
				chmod($thumb['dir'], 0777);
				
			}
			if(!is_writable($thumb['dir'])) {
				die('dest dir not writable: '.$thumb['dir']);  
			}
			$image_info = getimagesize($file, $image_extra_info);
			$dest_file = $thumb['dir'] .$filename;
			$resized_handle = resizeImage($file, $thumb['width'],$thumb['height'], $image_info);
			imagepng($resized_handle, $dest_file);
			chmod($dest_file, 0777);
		}
	}
	
}

