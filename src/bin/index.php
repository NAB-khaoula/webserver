<html>
    <body>
        
        <br> First Name Is:   <?php echo $_GET["fname"]; ?><br>
        Second Name Is:  <?php echo $_GET["lname"]; ?>
        
    </body>
    </html>
<?php
// var_dump($_POST);
// var_dump($_GET);
setcookie("last_name", $_GET["lname"]);
setcookie("first_name", $_GET["fname"]);
// Show all information, defaults to INFO_ALL
// phpinfo();

// var_dump($_COOKIE);
//NOTE - Delete File
// if ($_SERVER["REQUEST_METHOD"] == "DELETE")
// {
//     $deletefile=unlink('test_delete.txt');    
//     if($deletefile)
//     {  
//     echo "File deleted.";    
//     }
//     else
//     {  
//         echo "Unable to Delete the File.";    
//     } 
// }

// Show just the module information.
// phpinfo(8) yields identical results.
// phpinfo(INFO_MODULES);
// phpinfo(INFO_MODULES);

?>
