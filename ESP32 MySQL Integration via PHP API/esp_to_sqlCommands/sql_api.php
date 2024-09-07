<?php
$servername = "localhost";
$username = "root"; // replace with your MySQL username
$password = ""; // replace with your MySQL password
$dbname = "esp_to_sql"; // replace with your database name

$conn = new mysqli($servername, $username, $password, $dbname);

if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

$input = json_decode(file_get_contents("php://input"), true);
$sql_query = $input['sql_query'];

$result = $conn->query($sql_query);

$response = array();

if ($result === false) {
    $response["status"] = "error";
    $response["message"] = "SQL Error: " . $conn->error;
} else {
    if ($result === true) {
        $response["status"] = "success";
        $response["message"] = "Query executed successfully";
    } elseif ($result->num_rows > 0) {
        $response["status"] = "success";
        $response["data"] = array();

        while ($row = $result->fetch_assoc()) {
            $response["data"][] = $row;
        }
    } else {
        $response["status"] = "success";
        $response["message"] = "No records found";
    }
}

echo json_encode($response);

$conn->close();
?>
