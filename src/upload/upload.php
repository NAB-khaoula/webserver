<?php
	if ($_SERVER["REQUEST_METHOD"] === "POST")
	{
		if (!empty($_FILES))
		{
			foreach ($_FILES as $key => $value) {
				if ((isset($_FILES[$key]['tmp_name']))) {    
					$name = $_FILES[$key]['name'];
					move_uploaded_file($_FILES[$key]['tmp_name'], $name);
					if ($name != "")
						echo "File " . $name . " has been uploaded successfully to the server!!\n\n";  
				}
			}
		}
		if (!empty($_POST))
			var_dump($_POST);
	}
	else
		echo "Invalid method \n";
?>