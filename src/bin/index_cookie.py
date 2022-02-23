#!/usr/bin/python

import os

# Hello world python program
print

handler = {}
if 'HTTP_COOKIE' in os.environ:
    cookies = os.environ['HTTP_COOKIE']

    # print(cookies)
    cookies = cookies.split('&')

    for cookie in cookies:
        cookie = cookie.split('=')
        handler[cookie[0]] = cookie[1]

for k in handler:
    print(f"<p> <span style='font-weight: bold'>{k} </span>: {handler[k]} </p>")