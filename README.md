# Script-Manager

<a href="https://github.com/Robiathin/script-manager/releases"><img src="https://img.shields.io/github/release/Robiathin/script-manager.svg" alt="Release"></a> <a href="https://github.com/Robiathin/script-manager/blob/master/LICENSE"><img src="https://img.shields.io/github/license/Robiathin/script-manager.svg?maxAge=2592000" alt="License"></a> <a href="https://travis-ci.org/Robiathin/script-manager"><img src="https://travis-ci.org/Robiathin/script-manager.svg?branch=master" alt="Build Status"></a>

## About

A tool for organizing and managing utility scripts. Allows for easy storage, searching, and executing of small scripts.

This project is not often updated, but is still maintained.

## Usage

For the full usage information read the included man page (sm.1).

Scripts can be added with:

```sh
sm -a script_name -f file_to_add -D "A helpful description for the file."
```

This will create a copy of the script file in `.script-db/` in your home directory as well as store the name and description information in an SQLite database in that directory.

You can then execute the script via `sm -e script_name`. Arguments can be passed to the script by adding `-A argument` for each argument to pass to the script.

To replace the file use:

```sh
sm -r script_name -f new_file
```

This will replace the script file associated with the name `script_name` with the file `new_file`.

Remove scripts from Script Manager with:

```sh
sm -d script_name
```

You can search for scripts using:

```sh
sm -s -n test -D "some description"
```

You can search by either name or description or both. If you search for both, then both must contain the search text.

The output of search looks like this:

```
$ sm -s -n te
name: test
description: stuff
```

`sm -E script_name` can be used to echo a copy of a stored script. The output can be redirected to a file to obtain a copy of the file: `sm -E script_name > file`.

Script-Manager will work with any executable file. Meaning, scripts that have `#!` to specify an interpreter or compiled executables (Such as a C program).

The output from echoing files, searching or listing files is paged by default unless the output is piped or if the `-p` flag is set. By default less is used as the pager, but this can be changed by setting the `SMPAGER` environment variable.

It is possible to edit files directly using:

```sh
sm -V test
```

This will open the file in an editor. The default is Vim, however the editor can be changed by setting the `SMEDITOR` environment variable.

## Installation

First, make sure you have the SQLite development package installed for your system.

Download the latest release from [github.com/Robiathin/script-manager/releases](https://github.com/Robiathin/script-manager/releases). Extract the archive and change directory into it.

Then run:

```sh
make
sudo make install
```

The install prefix (default is `/usr/local`) and the man page prefix (default `/usr/share`) can be changed by setting the enironment variables `prefix` and `man_prefix`. The executable name can be changed by setting `EXECUTABLE`.

Optionally, to install BASH/ZSH tab completion for script names, add the contents of `misc/autocomplete.sh` to `~/.profile` or some other location where your shell will run it on start.

## Compatabillity

Script-Manager should work on any GNU/Linux, BSD or macOS system. Script-Manager on macOS does not support paging output at this time however if wanted it can be done externally: `sm -E test | less`

## Dependencies

 - SQLite 3 headers

## Contributing

See `CONTRIBUTING.md`

## License

ISC License
