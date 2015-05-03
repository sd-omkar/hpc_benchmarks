<?php
error_reporting(E_ALL & ~E_NOTICE);

class MyDB extends SQLite3 {
    function __construct() {
		$this->open('perfdb');
    }
}

$where = $_POST['where'];
if (empty($where)) $where = '1=1';

$sel = "";
foreach ($_POST as $key => $value){
	if ($key != 'where' and $key != 'action')
		$sel .= $value.", ";
}
$sel .= "value_type, value";

if (!empty($sel) and !empty($where)) {
	$db = new MyDB();	
	
	$stmt = $db->prepare("select ".$sel." from master where ".$where." ;");	
	$results = $stmt->execute();
	$num_cols = count($_POST);
	
	if ($_POST['action'] == 'Show Results') {
		print "<table border='1'>";
		print "<tr>";
		foreach ($_POST as $key => $value){
			if ($key != 'where' and $key != 'action')
				print "<td><b>".$key."</td>";
		}
		print "<td><b> Metric Measured</td>";
		print "<td><b> Value </td>";
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
	
	if ($_POST['action'] == 'Get CSV') {
		header('Content-Type: text/csv; charset=utf-8');
		header('Content-Disposition: attachment; filename=data.csv');
		$output = fopen('php://output', 'w');
		
		$head = array();
		foreach ($_POST as $key => $value){
			if ($key != 'where' and $key != 'action')
				array_push($head, $key);
		}
		array_push($head, "Metric_Measured");
		array_push($head, "Value");
		fputcsv($output, $head);
		
		while ($row = $results->fetchArray()) {
			for ($i=0; $i<$num_cols; $i+=2) {
				unset($row[$i]);
			}
			fputcsv($output, $row);
		}
	}
}

?>