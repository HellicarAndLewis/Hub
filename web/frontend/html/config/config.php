<?php
require_once 'serverswitch.php';
$server_switch = new ServerSwitch();

// nelson, localhost
// grant all on dewarscube.* to dewarscube@localhost identified by 'dewarscube';
$config['db']['database']		= 'dewarscube';
$config['db']['username']  	= 'dewarscube';
$config['db']['password']  	= 'dewarscube';
$config['db']['host']  		= 'localhost';
$config['upload_dir'] 		= $base .'/uploads';

$server_switch->addServer('nelson', 'nelson', false, $config);
$config = $server_switch->getConfig('nelson');

