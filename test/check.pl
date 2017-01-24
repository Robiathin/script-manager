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

use File::Path;
use Term::ANSIColor;

my $VALGRIND_EXEC = "valgrind --error-exitcode=1 --leak-check=full ";

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

# Write the sample script to a file
open(my $fh, ">", "testscript.pl") or die "Could not open file 'testscript.pl' $!";
print $fh $script_contents;
close $fh;

##
# Test function
#
# Used as a helper for testing.
#
# Args:
#  Test message (string) - ex: Testing echo...\t
#  Test result (boolean) - true for pass and false for fail
sub test {
	print "$_[0]";

	if ($_[1]) {
		print color("green");
		print "PASS\n";
		print color("reset");
	} else {
		print color("red");
		print "FAIL\n";
		print color("reset");
		$result++;
	}
}

`sm -a -n test -f testscript.pl -D 'test file' 2>&1`;

test("Testing add...", (!$? && -f $ENV{"HOME"} . "/.script-db/1"));

test("Testing execute...", ("hello world\n" eq `sm -e test 2>&1`));

test("Testing list...", ($expected_output eq `sm -l -p 2>&1`));

test("Testing search...", ($expected_output eq `sm -s -n te -p 2>&1`));

test("Testing echo...", ($script_contents eq `sm -E test -p 2>&1`));

# Write the alternate sample script to a file to test `sm -r`
open($fh, ">", "testscript.pl") or die "Could not open file 'testscript.pl' $!";
print $fh $other_script_contents;
close $fh;

`sm -r test -f testscript.pl 2>&1`;

test("Testing replace...", ($other_script_contents eq `sm -E test -p 2>&1`));

test("Testing completion...", (`sm -C 2>&1` eq "test "));

`sm -V test 2>&1`;

test("Testing edit...", !$?);

`sm -a -f notafile -n invalid -D desc 2>&1`;

test("Testing no file...", $?);

# Testing with Valgrind...
# Reset for valgrind tests
rmtree $ENV{"HOME"} . "/.script-db";

system("$VALGRIND_EXEC ./sm -a -n test -f testscript.pl -D 'test file' 2>&1");
test("Testing add with valgrind...", (!$? && -f $ENV{"HOME"} . "/.script-db/1"));

system("$VALGRIND_EXEC ./sm -l -p 2>&1");
test("Testing list with Valgrind...", !$?);

system("$VALGRIND_EXEC ./sm -s -n te -p 2>&1");
test("Testing search in valgrind...", !$?);

system("$VALGRIND_EXEC ./sm -E test -p 2>&1");
test("Testing echo in Valgrind...", !$?);

print "\n";

if ($result) {
	print "There were $result test failures.\n";
} else {
	print "All tests passed!\n";
}

exit $result;
