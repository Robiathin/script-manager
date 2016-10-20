# Copyright (c) 2016 Robert Tate <rob@rtate.se>
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

use strict;
use warnings;

# Output to expect from both search and list.
my $expected_output = <<EOF;
id: 1
name: test
description: test file

EOF

# Main script file contents.
my $script_contents = <<EOF;
#!/usr/bin/env perl

print "hello world\\n";
EOF

# Alternate script file contents for testing `sm -r`
my $other_script_contents = <<EOF;
#!/usr/bin/env perl

print "other script\\n";
EOF

my $result = 0;

open(my $fh, '>', "testscript.pl") or die "Could not open file 'testscript.pl' $!";
print $fh $script_contents;
close $fh;

print "Testing add...\t";

`sm -a -n test -f testscript.pl -D 'test file'`;

if (!$? && -f $ENV{"HOME"} . "/.script-db/1") {
	print "PASS\n"
} else {
	print "FAIL\n";
	$result++;
}

print "Testing execute...\t";

if ("hello world\n" eq `sm -e test`) {
	print "PASS\n"
} else {
	print "FAIL\n";
	$result++;
}

print "Testing list...\t";

if ($expected_output eq `sm -l -p`) {
	print "PASS\n";
} else {
	print "FAIL\n";
	$result++;
}

print "Testing search...\t";

if ($expected_output eq `sm -s -n te -p`) {
	print "PASS\n";
} else {
	print "FAIL\n";
	$result++;
}

print "Testing echo...\t";

if ($script_contents eq `sm -E test -p`) {
	print "PASS\n";
} else {
	print "FAIL\n";
	$result++;
}

print "Testing replace...\t";

open($fh, '>', "testscript.pl") or die "Could not open file 'testscript.pl' $!";
print $fh $other_script_contents;
close $fh;

`sm -r test -f testscript.pl`;

if ($other_script_contents eq `sm -E test -p`) {
	print "PASS\n";
} else {
	print "FAIL\n";
	$result++;
}

print "\n"

if ($result) {
	print "There were $result test failures.\n";
} else {
	print "All tests passed!\n";
}

exit $result;
