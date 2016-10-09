use strict;

my $expected_output = <<EOF;
id: 1
name: test
description: test file

EOF

my $script_contents = <<EOF;
#!/usr/bin/env perl

print "hello world\\n";
EOF

my $other_script_contents = <<EOF;
#!/usr/bin/env perl

print "other script\\n";
EOF

my $result = 0;

print "Testing execute...\t";

if ("hello world\n" eq `sm -e test`) {
	print "PASS\n"
} else {
	print "FAIL\n";
	$result = 1;
}

print "Testing list...\t";

if ($expected_output eq `sm -l -p`) {
	print "PASS\n";
} else {
	print "FAIL\n";
	$result = 1;
}

print "Testing search...\t";

if ($expected_output eq `sm -s -n te`) {
	print "PASS\n";
} else {
	print "FAIL\n";
	$result = 1;
}

print "Testing echo...\t";

if ($script_contents eq `sm -E test -p`) {
	print "PASS\n";
} else {
	print "FAIL\n";
	$result = 1;
}

print "Testing replace...\t";

`sm -r test -f test/test2.pl`;

if ($other_script_contents eq `sm -E test -p`) {
	print "PASS\n";
} else {
	print "FAIL\n";
	$result = 1;
}

exit $result;
