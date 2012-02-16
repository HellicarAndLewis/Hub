<?php
require_once 'serverswitch.php';
$server_switch = new ServerSwitch();

// nelson, localhost
// grant all on dewarshub.* to dewarshub@localhost identified by 'dewarshub';
// grant all on dewarscube.* to dewarscube@localhost identified by 'dewarscube';
$config['db']['database']	= 'dewarshub';
$config['db']['username']  	= 'dewarshub';
$config['db']['password']  	= 'dewarshub';
$config['db']['host']  		= 'localhost';
$config['upload_dir'] 		= $base .'/uploads';

$server_switch->addServer('nelson', 'nelson', false, $config);
$config = $server_switch->getConfig('nelson');

