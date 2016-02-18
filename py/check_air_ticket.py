#--*-- coding:utf-8 --*--
import requests, requests.utils, pickle
import httplib
import sys
import pprint
from BeautifulSoup import BeautifulSoup
import re
import shutil
import netrc
import os.path
import json

COOKIE_FILE='.cookies'

s = requests.Session()
s.headers.update({'User-Agent':'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_4) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/37.0.2062.94 Safari/537.36',
'Connection':'keep-alive',
'Content-type':'application/x-www-form-urlencoded'})
# headers = {'X-Requested-With':'XMLHttpRequest'}

def patch_send():
    old_send= httplib.HTTPConnection.send
    def new_send( self, data ):
        print >>sys.stderr, '>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>'
        print >>sys.stderr, data
        print >>sys.stderr, '<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<'
        return old_send(self, data) #return is not necessary, but never hurts, in case the library is changed
    httplib.HTTPConnection.send= new_send

# def patch_getresponse():
#     old_getresponse= httplib.HTTPConnection.getresponse
#     def new_getresponse( self, buffering=False):
#         data = old_getresponse(self, buffering) #return is not necessary, but never hurts, in case the library is changed
#         print data
#         return data
#     httplib.HTTPConnection.getresponse= new_getresponse
# patch_getresponse()

patch_send()



def debugReq(r):
  pp = pprint.PrettyPrinter(indent=4)
  pp.pprint(r.status_code)
  # pp.pprint(r.request.__dict__)
  # print >>sys.stderr, r.text
  print >>sys.stderr, s.cookies.get_dict()

def nanhang():
  s.get("http://b2c.csair.com/B2C40/checkLogin/jaxb/checkLogin.ao")
  headers= {"Referer":"http://b2c.csair.com/B2C40/modules/bookingnew/main/flightSelectDirect.html?t=S&c1=SYX&c2=WUH&d1=2015-11-08&at=1&ct=0&it=0"}
  POST_URL = "http://b2c.csair.com/B2C40/query/jaxb/direct/query.ao"
  payload = {"json": {"depcity":"SYX", "arrcity":"WUH", "flightdate":"20151108", "adultnum":"1", "childnum":"0", "infantnum":"0", "cabinorder":"0", "airline":"1", "flytype":"0", "international":"0", "action":"0", "segtype":"1", "cache":"0", "preUrl":"", "isMember":""}}
  try:
    r = s.post(POST_URL,data = json.dumps(payload),headers=headers)
  except requests.exceptions.ConnectionError as e:
    print "post error"
  print r.text

def main():
    nanhang()

if __name__ == "__main__":
  main()
