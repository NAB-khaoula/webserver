<?php
   // var_dump($_POST)
   
   if( $_POST["firstname"] || $_POST["lastname"] ) {
      if (preg_match("/[^A-Za-z'-]/",$_POST['firstname'] ) && preg_match("/[^A-Za-z'-]/",$_POST['lastname'] )) {
         die ("invalid firstname and lastname should be alpha");
      }

      echo "First Name:  ". $_POST['firstname']. ".<br />";
      echo "Last  Name:  ". $_POST['lastname']. ".";
      
      exit();
   }
?>