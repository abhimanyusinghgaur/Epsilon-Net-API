<?php
	header('Content-Type: application/json');

	if(empty($_POST['epsilon']))
		echo json_encode(["status"=>"Error", "error"=>"Epsilon value not given."]);
	else if(empty($_POST['points']))
		echo json_encode(["status"=>"Error", "error"=>"Points not given."]);
	else {
		//shell_exec('make');
		shell_exec("chmod ugo=rwx enet.out");
		$cmd = './enet.out '.$_POST['epsilon'].' '.$_POST['points'];
		$shell_output = shell_exec($cmd);
		if($shell_output == NULL)
			echo json_encode(["status"=>"Error", "error"=>"Something went wrong! Try again."]);
		else {
			$shell_output = rtrim($shell_output);
			$epsnet = explode(' ', $shell_output);
			echo json_encode(["status"=>"Success", "epsnet"=>$epsnet], JSON_NUMERIC_CHECK);
		}
	}

	header('Content-Type: text/html; charset=utf-8');
?>

<!DOCTYPE html>
<html>
<head>
	<title>Epsilon Net</title>
</head>
<body>
	<form action="" method="post">
		Epsilon: <input type="number" name="epsilon" step="any" min="0" max="1"><br>
		Points: <input type="text" name="points"><br>
		<input type="submit">
	</form>
</body>
</html>