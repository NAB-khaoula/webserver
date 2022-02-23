<?php
	if ($_SERVER["REQUEST_METHOD"] === "POST")
	{
		if (!empty($_FILES))
		{
			foreach ($_FILES as $key => $value) {
				if ((isset($_FILES[$key]['tmp_name']) && ($_FILES[$key]['error'] == UPLOAD_ERR_OK))) {    
					$name = $_FILES[$key]['name'];
					$dir = './../upload/';
					if (!file_exists($dir)) {
						mkdir($dir, 0777, true);
					}
					move_uploaded_file($_FILES[$key]['tmp_name'], $dir . $name);
					echo "File " . $name . " has been uploaded successfully to the server!!\n";  
				} else {
					echo "Uploading failed!\n";
				}
			}
		}
		if (!empty($_POST))
			var_dump($_POST);
	}
	else
		echo "Invalid method \n";
?>