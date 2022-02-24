<?php
	if ($_SERVER["REQUEST_METHOD"] == "DELETE")
	{
		if ($_SERVER["CONTENT_TYPE"] == "text/plain")
		{
			$value = file_get_contents('php://input');
			if (strstr($value, '../' ) || $value == "upload.php" || $value == "delete.php") {
				echo "you dont have permission to delete this file : " . $value . " \n";
				exit (1);
			}
			if (file_exists($value))
			{
				unlink($value);
				echo $value . " deleted successfully\n";
			}
			else
				echo $value . ": No such file or directory\n";
		}
		else if ($_SERVER["CONTENT_TYPE"] == "application/json")
		{
			$body = json_decode(file_get_contents('php://input'), true);
			foreach ($body as $key => $value) {
				if (strstr($value, '../' ) || $value == "upload.php" || $value == "delete.php") {
					echo "you dont have permission to delete this file : " . $value . " \n";
					continue ;
				}
				if (file_exists($value))
				{
					unlink($value);
					echo $value . " deleted successfully\n";
				}
				else
					echo $value . ": No such file or directory\n";
			}
		}
		else if ($_SERVER["CONTENT_TYPE"] == "application/javascript")
		{
			parse_str(file_get_contents('php://input'), $body);
			foreach ($body as $key => $value) {
				if (strstr($value, '../' ) || $value == "upload.php" || $value == "delete.php") {
					echo "you dont have permission to delete this file : " . $value . " \n";
					continue ;
				}
				if (file_exists($value))
				{
					unlink($value);
					echo $value . " deleted successfully\n";
				}
				else
					echo $value . ": No such file or directory\n";
			}
		}
		else if ($_SERVER["CONTENT_TYPE"] == "application/x-www-form-urlencoded")
		{
			parse_str(file_get_contents('php://input'), $body);
			foreach ($body as $key => $value) {
				if (strstr($value, '../' ) || $value == "upload.php" || $value == "delete.php") {
					echo "you dont have permission to delete this file : " . $value . " \n";
					continue ;
				}
				if (file_exists($value))
				{
					unlink($value);
					echo $value . " deleted successfully\n";
				}
				else
					echo $value . ": No such file or directory\n";
			}
		}
		else
		{
			echo "Content type is " . $_SERVER["CONTENT_TYPE"] . " \n";
			echo "You cant delete the file \n";
		}
	}
	else
		echo "Invalid method \n";
?>