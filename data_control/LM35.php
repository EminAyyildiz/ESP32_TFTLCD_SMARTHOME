<?php

$hostname = "localhost"; 
$username = "root"; 
$password = ""; 
$database = "read_data"; 

$conn = mysqli_connect($hostname, $username, $password, $database);

if (!$conn) { 
    die("Connection failed: " . mysqli_connect_error()); 
} 

echo "Database connection is OK<br>"; 

if(isset($_POST["Temp"]) && isset($_POST["humidity"])) {
    $t = $_POST["Temp"];
    $h = $_POST["humidity"];

    $sql = "UPDATE led_statu SET Temp = '$t', Humidity = '$h' WHERE ID = 0"; 


    if (mysqli_query($conn, $sql)) { 
        echo "\nRecord updated successfully"; 
    } else { 
        echo "Error: " . $sql . "<br>" . mysqli_error($conn); 
    }
}
?>
