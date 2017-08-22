# Copyright (c) 2016-2017 Robert Tate <rob@rtate.se>
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
use 5.010;

use File::Path;
use Term::ANSIColor;

my $VALGRIND_EXEC = "valgrind --error-exitcode=1 --leak-check=full ";

# Output to expect from both search and list.
my $expected_output = <<EOF;
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
#  Test function (Subroutine) - true for pass and false for fail
sub test {
	say color("blue"), "Testing ", $_[0], "...", color("reset");

	if ($_[1]->()) {
		say color("blue"), "Testing ", $_[0], "... ", color("green"), "PASS", color("reset");
	} else {
		say color("blue"), "Testing ", $_[0], "... ", color("red"), "FAIL", color("reset");
		$result++;
	}
}

sub t_exec {
	say color("purple"), $_[0], color("reset");
	my $output = `$_[0]`;
	say $output;
	return ($output);
}

test("add", sub {
	t_exec("sm -a -n test -f testscript.pl -D 'test file' 2>&1");
	return (!$? && -f $ENV{"HOME"} . "/.script-db/1");
});

test("execute", sub {
	return ("hello world\n" eq t_exec("sm -e test 2>&1"));
});

test("list", sub {
	return ($expected_output eq t_exec("sm -l -p 2>&1"));
});

test("search", sub {
	return ($expected_output eq t_exec("sm -s -n te -p 2>&1"));
});

test("echo", sub {
	return ($script_contents eq t_exec("sm -E test -p 2>&1"));
});

# Write the alternate sample script to a file to test `sm -r`
open($fh, ">", "testscript.pl") or die "Could not open file 'testscript.pl' $!";
print $fh $other_script_contents;
close $fh;

test("replace", sub {
	t_exec("sm -r test -f testscript.pl 2>&1");
	return ($other_script_contents eq t_exec("sm -E test -p 2>&1"));
});

test("completion", sub {
	return (t_exec("sm -C 2>&1") eq "test ");
});

test("edit", sub {
	t_exec("sm -V test 2>&1");
	return (!$?);
});

test("add non-exsistant file", sub {
	t_exec("sm -a -f notafile -n invalid -D desc 2>&1");
	return ($?);
});

# Testing with Valgrind.
# Currently only working on linux.
if ($^O eq "linux") {
	# Reset for valgrind tests
	rmtree $ENV{"HOME"} . "/.script-db";

	test("add with valgrind", sub {
		t_exec("$VALGRIND_EXEC ./sm -a -n test -f testscript.pl -D 'test file' 2>&1");
		return (!$? && -f $ENV{"HOME"} . "/.script-db/1");
	});

	test("list with Valgrind", sub {
		t_exec("$VALGRIND_EXEC ./sm -l -p 2>&1");
		return (!$?);
	});

	test("search with Valgrind", sub {
		t_exec("$VALGRIND_EXEC ./sm -s -n te -p 2>&1");
		return (!$?);
	});

	test("echo with Valgrind", sub {
		t_exec("$VALGRIND_EXEC ./sm -E test -p 2>&1");
		return (!$?);
	});
}

print "\n";

if ($result) {
	say "There were $result test failures.";
} else {
	say "All tests passed!";
}

exit $result;
