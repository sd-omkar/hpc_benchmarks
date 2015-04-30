<?php

class MyDB extends SQLite3
   {
      function __construct()
      {
         $this->open('perfdb');
      }
   }
  
$bench = $_POST['bench'];

if (!empty($bench)) {
	$db = new MyDB();	
	
	$stmt = $db->prepare("SELECT accl_name, library, backend, input, dim_x, dim_y, NNZ, value_type, value 
						FROM master WHERE lower(name) LIKE :pb");
	$stmt->bindParam(':pb', $bench, SQLITE3_TEXT);
	$results = $stmt->execute();
	
	print "<table border='1'>";
	print "<tr>
			<td>Name</td>
			<td>Library</td>
			<td>Backend</td>
			<td>Input</td>			
			<td>dim_x</td>
			<td>dim_y</td>
			<td>NNZ</td>
			<td>Metric</td>
			<td>Value</td>
			</tr>";
	while ($row = $results->fetchArray()) {
		print "<tr>
				<td>".$row['accl_name']."</td>
				<td>".$row['library']."</td>
				<td>".$row['backend']."</td>
				<td>".$row['input']."</td>
				<td>".$row['dim_x']."</td>
				<td>".$row['dim_y']."</td>
				<td>".$row['NNZ']."</td>
				<td>".$row['value_type']."</td>
				<td>".$row['value']."</td>
				</tr>";
	}
	print "</table>";
	
}

?>