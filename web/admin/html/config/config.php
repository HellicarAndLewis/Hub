<?php
require_once 'serverswitch.php';
$server_switch = new ServerSwitch();

// nelson, localhost
// Give localhost and remotes access:
// grant all on dewarshub_admin.* to dewarshub_admin@"localhost" identified by "dewarshub_admin"
// grant all on dewarshub_admin.* to dewarshub_admin@"%" identified by "dewarshub_admin"

$config['db']['database']	= 'dewarshub_admin';
$config['db']['username']  	= 'dewarshub_admin';
$config['db']['password']  	= 'dewarshub_admin';
$config['db']['host']  		= 'localhost';
$config['upload_dir'] 		= $base .'/uploads';

$config['osc'] = array(
	 'host' 	=> 'localhost'
	,'port' 	=> 4444
);

$server_switch->addServer('nelson', 'nelson', false, $config);
$config = $server_switch->getConfig('nelson');


