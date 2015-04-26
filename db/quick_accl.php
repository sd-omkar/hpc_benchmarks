<?php

class MyDB extends SQLite3
   {
      function __construct()
      {
         $this->open('perfdb');
      }
   }
  
$accl_id = $_POST['accl'];

if (!empty($accl_id)) {
	$db = new MyDB();	
	
	$stmt = $db->prepare('SELECT name, library, backend, NNZ, value_type, value 
						FROM master WHERE accl_id=:aid;');
	$stmt->bindParam(':aid', $accl_id, SQLITE3_INTEGER);
	$results = $stmt->execute();
	
	print "<table border='1'>";
	print "<tr>
			<td>Name</td>
			<td>Library</td>
			<td>Backend</td>
			<td>Size</td>
			<td>Metric</td>
			<td>Value</td>
			</tr>";
	while ($row = $results->fetchArray()) {
		print "<tr>
				<td>".$row['name']."</td>
				<td>".$row['library']."</td>
				<td>".$row['backend']."</td>
				<td>".$row['NNZ']."</td>
				<td>".$row['value_type']."</td>
				<td>".$row['value']."</td>
				</tr>";
	}
	print "</table>";
	
}

?>