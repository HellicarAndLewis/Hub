<?php
require_once 'serverswitch.php';
$server_switch = new ServerSwitch();

// nelson, localhost
// grant all on dewarscube_admin.* to dewarscube_admin@localhost identified by 'dewarscube_admin';
$config['db']['database']		= 'dewarscube_admin';
$config['db']['username']  	= 'dewarscube_admin';
$config['db']['password']  	= 'dewarscube_admin';
$config['db']['host']  		= 'localhost';
$config['upload_dir'] 		= $base .'/uploads';

$config['osc'] = array(
	 'host' 	=> 'localhost'
	,'port' 	=> 4444
);

$server_switch->addServer('nelson', 'nelson', false, $config);
$config = $server_switch->getConfig('nelson');

