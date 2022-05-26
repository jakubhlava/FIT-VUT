#!/usr/bin/env bash
wget https://search.maven.org/remotecontent\?filepath\=com/google/code/gson/gson/2.8.6/gson-2.8.6.jar -O gson-2.8.6.jar

wget https://gluonhq.com/download/javafx-11-0-2-sdk-linux/ -O openjfx-linux.zip
unzip openjfx-linux.zip
rm -f openjfx-linux.zip
