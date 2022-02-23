<?php
   if( $_GET["firstname"] || $_GET["lastname"] ) {
      echo "First Name:  ". $_GET['firstname']. ".<br />";
      echo "Last Name :  ". $_GET['lastname']. ".";
      
      exit();
   }
?>