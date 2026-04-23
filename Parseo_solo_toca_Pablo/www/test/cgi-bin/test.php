<?php
echo "Status: 200 OK\r\n";
echo "Content-Type: text/plain\r\n\r\n";

echo "PHP funcionando\n";

$body = file_get_contents("php://stdin");
echo "Body recibido:\n";
echo $body;
?>