<?php
	if ($_SERVER["REQUEST_METHOD"] == "DELETE")
	{
		parse_str(file_get_contents('php://input'), $body);
		foreach ($body as $key => $value) {
			// $body = file_get_contents('php://input');
			// $body = json_decode(file_get_contents('php://input'), true);
			echo $key . " == > " . $value . "\n";
			var_dump($value);
			unlink($value);
		}
	}
?>