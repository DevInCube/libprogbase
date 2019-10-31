# libprogbase

[![Travis-CI][travis-badge]][travis-builds]

[Repository home][home]

## Releases

Now all releases and corresponding release notes will be published in [__Releases__][releases] section

## Older releases

All release notes up to 0.3.4 are [here](/old_releases.md)

## Installation and linkage

Clone this repository with command:

```sh
git clone https://github.com/PublicHadyniak/libprogbase.git
```

Change directory to the newly created `libprogbase/` directory and execute this command:

```sh
sudo make install
```

Include header files from `include/` directory and compile your code with linker option `-lprogbase`:

```sh
gcc main.c -lprogbase -lm
```

## Check library version (since version 0.1.0)

You need to change directory to cloned `libprogbase/` directory and execute command:

```sh
make version
```

## Update library

### Update with script (since version 0.1.0)

Works only if you cloned this repository with Git.

Change directory to cloned `libprogbase/` and execute command:

```sh
sudo make update
```

### Reinstall

You can reinstall the library to update it to newer version.

[home]: https://github.com/PublicHadyniak/libprogbase
[releases]: https://github.com/PublicHadyniak/libprogbase/releases
[travis-badge]: https://travis-ci.org/PublicHadyniak/libprogbase.svg?branch=master
[travis-builds]: https://travis-ci.org/PublicHadyniak/libprogbase/builds
