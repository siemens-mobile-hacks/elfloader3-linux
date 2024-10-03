<?php

$switch = [];

foreach (explode("\n", trim(file_get_contents("a"))) as $line) {
	list ($qt_name) = preg_split("/\s+/", $line);
	$name = str_replace("QT::", "IPC_", strtoupper(preg_replace("/([a-z])([A-Z])/", "$1_$2", $qt_name)));
	
	echo $name.",\n";
	
	$switch[] = "case $qt_name: return $name;";
}

// echo implode("\n", $switch)."\n\n";
