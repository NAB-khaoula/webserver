<?php
	if ($_SERVER["REQUEST_METHOD"] == "DELETE")
	{
		if ($_SERVER["CONTENT_TYPE"] == "text/plain")
		{
			// echo "Content type is raw\n";
			$value = file_get_contents('php://input');
			if (strstr( $value, '../' ) ) {
				echo "you dont have permission to delete this file : " . $value . " \n";
				exit (1);
			}
			unlink($value);
			echo $value . " deleted successfully\n";
		}
		else if ($_SERVER["CONTENT_TYPE"] == "application/json")
		{
			// echo "Content type is application/json\n";
			$body = json_decode(file_get_contents('php://input'), true);
			foreach ($body as $key => $value) {
				if (strstr( $value, '..' ) ) {
					echo "you dont have permission to delete this file\n";
					continue ;
				}
				unlink($value);
				echo $value . " deleted successfully\n";
			}
		}
		else if ($_SERVER["CONTENT_TYPE"] == "application/javascript")
		{
			// echo "Content type is application/javascript\n";
			parse_str(file_get_contents('php://input'), $body);
			foreach ($body as $key => $value) {
				if (strstr( $value, '..' ) ) {
					echo "you dont have permission to delete this file\n";
					continue ;
				}
				unlink($value);
				echo $value . " deleted successfully\n";
			}
		}
		else if ($_SERVER["CONTENT_TYPE"] == "application/x-www-form-urlencoded")
		{
			// echo "Content type is x-www-form-urlencoded\n";
			parse_str(file_get_contents('php://input'), $body);
			foreach ($body as $key => $value) {
				if (strstr( $value, '..' ) ) {
					echo "you dont have permission to delete this file\n";
					continue ;
				}
				unlink($value);
				echo $value . " deleted successfully\n";
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