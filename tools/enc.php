<?php
$conv = [];

$codepages = [];
for ($i = 0; $i <= 0xFF; $i++) {
	$c = iconv("cp1251", "utf-8", chr($i));
	if ($c === false)
		$c = iconv("cp1251", "utf-8", " ");
	
	if ($c == "%" || $c == " ") {
		$codepages[] = '"'.$c.'"';
		
		if ($c != chr($i)) {
			$conv[sprintf("%02x", $i)] = trim(preg_replace("/(..)/", "$1 ", bin2hex($c)));
		}
	} else {
		$codepages[] = '"'.str_replace("%", "\\x", urlencode($c)).'"';
		
		if ($c != chr($i)) {
			$conv[sprintf("%02x", $i)] = trim(preg_replace("/(..)/", "$1 ", bin2hex($c)));
		}
	}
}

foreach ($conv as $f => $to) {
	echo "$f -> $to\n";
}

foreach ($codepages as $i => $c) {
	if ($i % 8 == 0)
		echo "\n";
	echo "$c, ";
}

echo "\n\n";
echo "------------\n";

$codepages = [];
for ($i = 0; $i <= 0xFF; $i++) {
	$c = iconv("cp1251", "utf-16le", chr($i));
	if ($c === false)
		$c = iconv("cp1251", "utf-16le", " ");
	$codepages[] = sprintf("0x%04X", mb_ord($c, "utf-16le"));
}

foreach ($codepages as $i => $c) {
	if ($i % 8 == 0)
		echo "\n";
	echo "$c, ";
}

echo "\n\n";
echo "------------\n";

$codepages = [];
for ($i = 0; $i <= 0xFF; $i++) {
	$c = iconv("cp1252", "utf-16le", chr($i));
	if ($c === false)
		$c = iconv("cp1252", "utf-16le", " ");
	$codepages[] = sprintf("0x%04X", mb_ord($c, "utf-16le"));
}

foreach ($codepages as $i => $c) {
	if ($i % 8 == 0)
		echo "\n";
	echo "$c, ";
}
