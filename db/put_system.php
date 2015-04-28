<?php

class MyDB extends SQLite3
   {
      function __construct()
      {
         $this->open('perfdb');
      }
   }
 
$os = $_POST['os'];
$compiler = $_POST['compiler'];
$version = $_POST['version'];
$flags = $_POST['flags'];
$library = $_POST['library'];
$library_ver = $_POST['library_ver'];
$backend = $_POST['backend'];
$backend_ver = $_POST['backend_ver'];

if (!empty($os) and !empty($compiler) and !empty($version) and
	!empty($flags) and !empty($library) and !empty($library_ver) and
	!empty($backend) and !empty($backend_ver)) {
	$db = new MyDB();
	
	$stmt = $db->prepare("insert into systems 
						(os, compiler, version, flags, library, library_ver, backend, backend_ver) 
						values (:o, :c, :v, :f, :l, :lv, :b, :bv)");
	$stmt->bindParam(':o', $os, SQLITE3_TEXT);
	$stmt->bindParam(':c', $compiler, SQLITE3_TEXT);
	$stmt->bindParam(':v', $version, SQLITE3_TEXT);
	$stmt->bindParam(':f', $flags, SQLITE3_TEXT);
	$stmt->bindParam(':l', $library, SQLITE3_TEXT);
	$stmt->bindParam(':lv', $library_ver, SQLITE3_TEXT);
	$stmt->bindParam(':b', $backend, SQLITE3_TEXT);
	$stmt->bindParam(':bv', $backend_ver, SQLITE3_TEXT);
	$ret = $stmt->execute();
	
	if (!empty($ret)) {
		$system_id = $db->lastInsertRowID();
		print "Inserted row with system_id = ".$system_id;
	}
}

?>