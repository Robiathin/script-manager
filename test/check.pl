use strict;

my $expected_output = <<EOF;
id: 1
name: test
description: test file

EOF

my $script_contents = <<EOF;
#!/usr/bin/env perl

print "hello world\n";

EOF

# Test list
unless ($expected_output == `sm -l -p`) {
	print STDERR "List check failed\n";
	exit 1;
}

# Test search
unless ($expected_output == `sm -s -n te`) {
	print STDERR "Search check failed\n";
	exit 1;
}

# Test echo
unless ($script_contents == `sm -E test -p`) {
	print STDERR "Echo check failed\n";
	exit 1;
}

exit 0;
