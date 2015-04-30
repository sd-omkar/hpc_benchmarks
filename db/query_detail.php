<?php

class MyDB extends SQLite3
   {
      function __construct()
      {
         $this->open('perfdb');
      }
   }
  
$sel = $_POST['select'];
$whe = $_POST['where'];

if (empty($sel)) $sel = '*';
if (empty($whe)) $whe = '1=1';

if (!empty($sel) and !empty($whe)) {
	$db = new MyDB();	
	
	$stmt = $db->prepare("select ".$sel." from master where ".$whe." ;");	
	$results = $stmt->execute();
	
	$row_0 = $results->fetchArray();	
	$num_cols = count($row_0)/2;
	
	print "<table border='1'>";
	print "<tr>";
	for ($i=0; $i<$num_cols; $i++) {
		print "<td>".$row_0[$i]."</td>";
	}			
	print "</tr>";
	
	while ($row = $results->fetchArray()) {
		print "<tr>";
		for ($i=0; $i<$num_cols; $i++) {
			print "<td>".$row[$i]."</td>";
		}
	print "</tr>";
	}
	
	print "</table>";
	
}

?>