<?php

class MyDB extends SQLite3
   {
      function __construct()
      {
         $this->open('perfdb');
      }
   }
 
$hostname = $_POST['hostname'];

if (!empty($hostname)) {
	$db = new MyDB();
	
	$stmt = $db->prepare("insert into hosts (hostname) values (:hn)");
	$stmt->bindParam(':hn', $hostname, SQLITE3_TEXT);
	$ret = $stmt->execute();
	
	if (!empty($ret)) {
		$host_id = $db->lastInsertRowID();
		print "Inserted row with host_id = ".$host_id;
	}
}

?>