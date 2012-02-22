<?php
require_once 'serverswitch.php';
$server_switch = new ServerSwitch();

// nelson, localhost
// grant all on dewarshub.* to dewarshub@localhost identified by 'dewarshub';
// grant all on dewarscube.* to dewarscube@localhost identified by 'dewarscube';
// grant all on db_dewarshub.* to 'sql_dewarshub'@'localhost' identified by 'ijKM3nfBF12'

$config['db']['database']	= 'dewarshub';
$config['db']['username']  	= 'dewarshub';
$config['db']['password']  	= 'dewarshub';
$config['db']['host']  		= 'localhost';
$config['upload_dir'] 		= $base .'/uploads';

$server_switch->addServer('nelson', 'nelson', false, $config);


$config['db']['database'] = 'db_dewarshub';
$config['db']['username'] = 'sql_dewarshub';
$config['db']['password'] = 'ijKM3nfBF12';
$config['db']['host'] = 'localhost';
$server_switch->addServer('netground', 'netground', false, $config);


$config = $server_switch->getConfig('netground');
//print_r($config);
