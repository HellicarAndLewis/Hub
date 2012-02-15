<?php
$path = pathinfo(__FILE__);
$base = $path['dirname'];

require_once $base .'/config/config.php';
require_once $base .'/inc/functions.php';
require_once $base .'/inc/Dewars.php';

$dewars = new Dewars($config);

