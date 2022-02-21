#!/usr/bin/python
# Import modules for CGI handling
from os import environ
import cgi, cgitb
if environ.has_key('HTTP_COOKIE'):
   print("Cookie => " + environ['HTTP_COOKIE']);