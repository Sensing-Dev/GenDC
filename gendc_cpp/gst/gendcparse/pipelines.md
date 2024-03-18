Simple
======

./gst-gendcparse-launch gendcparsesrc ! videoconvert ! xvimagesink

Format filter
=============

./gst-gendcparse-launch gendcparsesrc ! video/x-raw,format=GRAY16_LE,depth=12 ! videoconvert ! xvimagesink
