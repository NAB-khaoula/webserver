<?php
	if ($_SERVER["REQUEST_METHOD"] === "POST")
	{
		foreach ($_FILES as $key => $value) {
			if ((isset($_FILES[$key]['tmp_name'])&&($_FILES[$key]['error'] == UPLOAD_ERR_OK))) {    
				$name = $_FILES[$key]['name'];    
				echo $name;
				$dir = './../upload/';
				move_uploaded_file($_FILES[$key]['tmp_name'], $dir . "$name");
				echo "File has been uploaded successfully to the server!!";  
			} else {
				echo "Uploading failed!";
			}
		}
	}
?>