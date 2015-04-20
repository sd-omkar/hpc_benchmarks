<?php

class MyDB extends SQLite3
   {
      function __construct()
      {
         $this->open('perfdb');
      }
   }
  
$table = $_POST['table'];

if (!empty($table)) {
	print "Querying from $table<br><br>";

	$db = new MyDB();
	
	if ($table == 'hosts') {
		$stmt = $db->prepare('SELECT * FROM hosts');
		$results = $stmt->execute();
		print "<table border='1'>";
		print "<tr>
				<td>host_id</td>
				<td>hostname</td>
				</tr>";
		while ($row = $results->fetchArray()) {
			print "<tr>
					<td>".$row['host_id']."</td>
					<td>".$row['hostname']."</td>
					</tr>";
		}
		print "</table>";
	}
	
	else if ($table == 'accelerators') {
		$stmt = $db->prepare('SELECT * FROM accelerators');
		$results = $stmt->execute();
		print "<table border='1'>";
		print "<tr>
				<td>accl_id</td>
				<td>Type</td>
				<td>Accelerator Name</td>
				<td>Architecture</td>
				<td>Memory (GB)</td>
				<td>Cores</td>
				<td>Details</td>
				</tr>";
		while ($row = $results->fetchArray()) {
			print "<tr>
					<td>".$row['accl_id']."</td>
					<td>".$row['type']."</td>
					<td>".$row['accl_name']."</td>
					<td>".$row['arch']."</td>
					<td>".$row['memory']."</td>
					<td>".$row['cores']."</td>
					<td>".$row['details']."</td>
					</tr>";
		}
		print "</table>";
	}
	
	else if ($table == 'systems') {
		$stmt = $db->prepare('SELECT * FROM systems');
		$results = $stmt->execute();
		print "<table border='1'>";print "<tr>
				<td>system_id</td>
				<td>OS</td>
				<td>Compiler</td>
				<td>Compiler Version</td>
				<td>COmpiler Flags</td>
				<td>Library</td>
				<td>Library Version</td>
				<td>Backend</td>
				<td>Backend Version</td>
				</tr>";
		while ($row = $results->fetchArray()) {
			print "<tr>
					<td>".$row['system_id']."</td>
					<td>".$row['OS']."</td>
					<td>".$row['compiler']."</td>
					<td>".$row['version']."</td>
					<td>".$row['flags']."</td>
					<td>".$row['library']."</td>
					<td>".$row['library_ver']."</td>
					<td>".$row['backend']."</td>
					<td>".$row['backend_ver']."</td>
					</tr>";
		}
		print "</table>";
	}
}

?>