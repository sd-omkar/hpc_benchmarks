<?php

class MyDB extends SQLite3
   {
      function __construct()
      {
         $this->open('perfdb');
      }
   }
 
$url = $_POST['url'];
$hash = $_POST['hash'];

if (!empty($url) and !empty($hash)) {
	$db = new MyDB();
	
	$stmt = $db->prepare("insert into sources (URL, commit_hash) values (:u, :ch)");
	$stmt->bindParam(':u', $url, SQLITE3_TEXT);
	$stmt->bindParam(':ch', $hash, SQLITE3_TEXT);
	$ret = $stmt->execute();
	
	if (!empty($ret)) {
		$source_id = $db->lastInsertRowID();
		print "Inserted row with source_id = ".$source_id;
	}
}

?>