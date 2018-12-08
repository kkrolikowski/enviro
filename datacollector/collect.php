<?php
    include_once './db.inc.php';

    $dsn = 'mysql:host='.DB_HOST.';dbname='.DB_NAME;
    $dbh = new PDO($dsn, DB_USER, DB_PASS);
    
    // timestamp
    
    $time = new DateTime();
    $timestamp = $time->getTimestamp();

    // client data

    $data = json_decode(file_get_contents('php://input'), true);

    // data validation
    $mandatory = array('temp', 'sensor_id', 'hum');
    $valid_array = 1;

    foreach ($mandatory as $param) {
        if (!array_key_exists($param, $data)) {
            $valid_array = 0;
        }
    }

    // collect data

    if ($valid_array) {
        $insert_data = "INSERT INTO sensor_data(sensor_id, temp, hum, timestamp) ".
                        " VALUES('".$data['sensor_id']."', '".$data['temp']."', '".
                        $data['hum'].", '".$timestamp."')";
        
        print $insert_data;
        $query = $dbh->prepare("INSERT INTO sensor_data(`sensor_id`,`temp`,`hum`,`timestamp`) VALUES(?,?,?,?)");
        $query->bindParam(1, $data['sensor_id']);
        $query->bindParam(2, $data['temp']);
        $query->bindParam(3, $data['hum']);
        $query->bindParam(4, $timestamp);
        
        $query->execute();

    }
    else {
        print "Incoplete data, check sensor!";
    }

?>