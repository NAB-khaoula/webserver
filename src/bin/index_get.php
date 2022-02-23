<?php
   if( $_GET["firstname"] || $_GET["lastname"] ) {
      echo "First Name:  ". $_GET['lastname']. ".<br />";
      echo "Last Name :  ". $_GET['lastname']. ".";
      
      exit();
   }
?>