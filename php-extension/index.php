<?php declare(strict_types=1); ini_set('display_errors', '1'); ini_set('display_startup_errors', '1'); error_reporting(E_ALL); assert_options(ASSERT_ACTIVE, 1); assert_options(ASSERT_WARNING, 1); ?>
<?php

	foreach(range(0,10000) as $_) {
		http_normalize_proto('hTtp');
		http_normalize_hostname('www.go%6fgle.c%6Fm');
		http_normalize_port(80, false);
		http_normalize_query('a=c&b=d');
	}

	exit
?>
<html>

	<head>
		<title></title>
		<style>input { min-width:30em; }</style>
	</head>

	<body>
		<form method="POST">
			<input type="text" name="in" value="<?= $_POST['in'] ?? '' ?>">
			<button>submit</button>
		</form>

		<?php foreach(range(1, 1) as $_): ?>
			<?php if (($tmp = normalize_http_url($_POST['in'] ?? '')) !== ''): ?>
				<input type="text" value="<?= $tmp ?> " readonly>
			<?php endif; ?>
		<?php endforeach; ?>

	</body>

</html>