<?php
    function getRequestHeaders() {
        $headers = "";
        foreach($_SERVER as $key => $value) {
            if (substr($key, 0, 5) <> 'HTTP_') {
                continue;
            }
            $header = str_replace(' ', '-', ucwords(str_replace('_', ' ', strtolower(substr($key, 5)))));
            if(!in_array($header, ['Origin', 'Referer', 'Host', 'Sec-Fetch-Dest', 'Sec-Fetch-Mode', 'Sec-Fetch-Site']))
	   	        $headers .= "$header: $value\r\n";
        }
        return $headers;
    }
	if(!isset($_GET["url"])) die("Specify url");
	$url = urldecode($_GET["url"]);
	$input = file_get_contents("php://input");
	$requestHeaders = getRequestHeaders();
    $method = $_SERVER['REQUEST_METHOD'];
    $options = array(
      'http'=>array(
        'method'=>$method,
        'header'=>$requestHeaders,
        'content'=>$input,
      )
    );
    $context = stream_context_create($options);
    $handle = fopen($url, 'rb', false, $context);
    for($i = 0; $i < count($http_response_header); $i++)
        if(explode(":", $http_response_header[$i])[0] != "Location")
            header($http_response_header[$i]);

    fpassthru($handle);
    exit;
?>