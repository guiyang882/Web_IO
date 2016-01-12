#!/usr/bin/env python
# coding=utf-8

import web
import json

urls = (
    "/","index"
)

class index:
    def GET(self):
        web.header('Access-Control-Allow-Origin','*')
        web.header('Content-Type','application/json')
        data = {"status":0,"msg":"OK !"}
        return json.dumps(data)
    
    def POST(self):
        web.header('Access-Control-Allow-Origin','*')
        web.header('Access-Control-Allow-Headers', 'content-type')
        web.header('Content-Type','application/json')
        data = {"status":0,"msg":"OK!"}
        print web.input()
        return json.dumps(data) 
   
    def OPTIONS(self):
        web.header('Access-Control-Allow-Origin','*')
        web.header('Access-Control-Allow-Headers', 'content-type')
        web.header('Content-Type','application/json')
        return 

if __name__ == "__main__":
    web.config.debug = False
    app = web.application(urls,globals())
    app.run()

