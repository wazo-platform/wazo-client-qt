XiVO Client
===========

XiVO Client is a Desktop [CTI client](https://github.com/xivo-pbx/xivo-ctid) based on [QT](http://qt-project.org/).
The Client provides various CTI services such as controlling a phone, sending a fax, or monitoring a XiVO 
[Call Center](http://en.wikipedia.org/wiki/Call_centre).

Building the client
-------------------

1. Install QT 4, make and GCC
2. run ```qmake```
3. run ```make```

You can find more complete documentation on building the XiVO Client on the 
[XiVO documentation web site](http://documentation.xivo.fr/production/developers/xivo_client/build.html).

Running the client
------------------

Once the client is built, you can run it with the following command:

    LD_LIBRARY_PATH=bin bin/xivoclient


Running unit tests
------------------

1. Install google-mock library with ```apt-get install google-mock```
2. Build the client with ```qmake CONFIG+=tests CONFIG+=gmock```
3. Run ```unit-tests/alltests.sh```

Running functional tests
------------------------

1. Install and configure the [XiVO Acceptance](https://github.com/xivo-pbx/xivo-acceptance) library
2. Build the client with ```make FUNCTESTS=yes```
3. Export the path to the binary in ```XC_PATH```. (e.g. ```export XC_PATH=/home/user/xivo-client-qt/bin```)
3. Run any of the tests in xivo-acceptance that need the client
