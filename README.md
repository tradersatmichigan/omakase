# Omakase

> Build plates of sushi

## Setup

First, we need to install OpenSSL. On Mac, we can do `brew install openssl`.
Then, add the following to your `~/.bashrc` or `~/.zshrc`:

```sh
export LDFLAGS="-L$(brew --prefix openssl@3)/lib"
export CPPFLAGS="-I$(brew --prefix openssl@3)/include"
export PKG_CONFIG_PATH="$(brew --prefix openssl@3)/lib/pkgconfig"
```

Note that this works on OpenSSL version 3.4.1, make sure this is the one you install.

Also, if you are on another platform, you need to modify the `Makefile`
to see your OpenSSL installation, i.e., replace `$(shell brew --prefix openssl)/lib`
with your installations lib folder when setting `LDFLAGS`. On linux, this will look like:

```sh
export LDFLAGS="-L/usr/lib/x86_64-linux-gnu"
export CPPFLAGS="-I/usr/include/openssl"
export PKG_CONFIG_PATH="/usr/lib/x86_64-linux-gnu/pkgconfig"
```

But make sure that these directories exists and are correct.

Now, run `sudo install.sh` to install necessary dependencies. Note: I'm not
sure how cross-platform this is yet, but should work on Apple Silicon.
