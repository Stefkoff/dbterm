# C MySQL terminal browser

This project will show the mysql tables and data in the terminal, by using NCURSES library for TUI

The project is build on Ubuntu 24.04 and not tested yet to another arch. Probably there will not be any issues

# Requirement
There are only two requirements:
1. ncurses: `apt install libncurses5-dev libncursesw5-dev`
2. mysql client: `apt isntall libmysqlclient-dev`

# Building
````
mkdir build
cmake ..
make
````

# Run the browser
After building the project, the executable will be in `bin` folder, called `dbterm`
You can run the browser by executing:

```./bin/dbter -h {host} -u {user} -p {password} -d {database}```

where:
 - `{host}` is the host address of the server. Default to `localhsot`
 - `{user}` is the user for access. Default to the current session user
 - `{password}` is the user's password
 - `{database}` is the initial database to load. Default to `mysql`

Currently `host and  password` are required in order to make a valid connection

# Using the browser

When you run the terminal browser, you can hit `:` and enter `help` command. It will show you how to work with the browser and how to use it

# Screenshots

Coming soon

# TODO

There are a lot of things to be fixed. It is only tested with some basic functionality for browsing.

Here are some notes that needs to be done:

1. Currently, all the tables and columns information is loaded on startup for the given database, and this takes some time to render the initial screen. It will be good to load them by some threads or something else, so the user will not stay and see nothing while loading
2. Total items count are loaded initially (as said above) and tables with a lot of data takes some time to load. It may be better to not show this, but then some functionality can be inactive.
3. Show error messages that comes from the database server, because currently it is very limiting on errors
4. still a lot of ...