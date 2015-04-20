<?php

class MyDB extends SQLite3
   {
      function __construct()
      {
         $this->open('perfdb');
      }
   }
 
$type = $_POST['type'];
$accl_name = $_POST['accl_name'];
$arch = $_POST['arch'];
$memory = $_POST['memory'];
$cores = $_POST['cores'];
$details = $_POST['type'];

if (!empty($type) & !empty($accl_name) & !empty($arch) &
	!empty($memory) & !empty($cores) & !empty($details)) {
	$db = new MyDB();
	
	$stmt = $db->prepare("insert into accelerators 
						(type, accl_name, arch, memory, cores, details) 
						values (:t, :an, :ar, :m, :c, :d)");
	$stmt->bindParam(':t', $type, SQLITE3_TEXT);
	$stmt->bindParam(':an', $accl_name, SQLITE3_TEXT);
	$stmt->bindParam(':ar', $arch, SQLITE3_TEXT);
	$stmt->bindParam(':m', $memory, SQLITE3_INTEGER);
	$stmt->bindParam(':c', $cores, SQLITE3_INTEGER);
	$stmt->bindParam(':d', $details, SQLITE3_TEXT);
	$ret = $stmt->execute();
	
	if (!empty($ret)) {
		$accl_id = $db->lastInsertRowID();
		print "Inserted row with accl_id = ".$accl_id;
	}
}

?>