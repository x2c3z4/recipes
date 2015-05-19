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
import fileinput


COOKIE_FILE='.weixin.cookies'
LOGIN_URL = 'https://mp.weixin.qq.com/cgi-bin/login'
POST_ALL_URL = 'https://mp.weixin.qq.com/cgi-bin/masssend?t=ajax-response&token=1599128149&lang=zh_CN'
POST_SINGLE_URL = 'https://mp.weixin.qq.com/cgi-bin/singlesend?t=ajax-response&f=json&token=1599128149&lang=zh_CN'

s = requests.Session()
s.headers.update({'User-Agent':'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_4) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/37.0.2062.94 Safari/537.36',
'Connection':'keep-alive',
'Referer':'https://mp.weixin.qq.com/',
'Content-type':'application/x-www-form-urlencoded',
})
# headers = {'X-Requested-With':'XMLHttpRequest'}

def saveCookies():
  with open(COOKIE_FILE, 'w') as f:
      pickle.dump(requests.utils.dict_from_cookiejar(s.cookies), f)

def loadCookies():
  with open(COOKIE_FILE) as f:
      cookies = requests.utils.cookiejar_from_dict(pickle.load(f))
      s.cookies = cookies
  print >>sys.stderr, '[+] load cookies!!!'

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


def get_credentials(mach):
  # default is $HOME/.netrc
  netrc_machine = mach
  info = netrc.netrc()
  (login, account, password) = info.authenticators(netrc_machine)
  return (login,password)

def debugReq(r):
  pp = pprint.PrettyPrinter(indent=4)
  pp.pprint(r.status_code)
  pp.pprint(r.request.__dict__)
  print >>sys.stderr, r.text
  print >>sys.stderr, s.cookies.get_dict()

def login(user, passwd):
  payload = {
      'username': 'lifeng1519@gmail.com',
      'pwd': '03afad33d847787f6ac8a4378e7e678b',
      'imgcode': '',
      'f': 'json',
  }
  try:
    r = s.post(LOGIN_URL,data = payload)
    #debugReq(r)
  except requests.exceptions.ConnectionError as e:
    print sys.exc_info()[0]
    print "login error"

def post_single_message(msg, user):
  payload = {
      'token': '1599128149',
      'lang': 'zh_CN',
      'f': 'json',
      'ajax': '1',
      'random': '0.8057038299739361',
      'type': '1',
      'content': msg,
      'tofakeid': '195927815',
      'imgcode': '',
  }
  r = s.post(POST_SINGLE_URL, data=payload)
  #debugReq(r)

def post_all_message(msg):
  payload = {
      'token': '1599128149',
      'lang': 'zh_CN',
      'f': 'json',
      'ajax': '1',
      'random': '0.4633970654103905',
      'type': '1',
      'content': msg,
      'cardlimit': '1',
      'sex': '0',
      'groupid': '-1',
      'synctxweibo': '0',
      'enablecomment': '0',
      'country': '',
      'province': '',
      'city': '',
      'imgcode': '',
      'operation_seq': '207055780',
  }
  r = s.post(POST_ALL_URL, data=payload)
  #debugReq(r)

def main():
  if not os.path.exists(COOKIE_FILE):
  #if True:
    #(user, passwd) = get_credentials('oracle')
    (user, passwd) = ("test", "test")
    login(user, passwd)
    saveCookies()
  else:
    loadCookies()


  content = ''
  for line in fileinput.input():
    content += line

  try:
    post_single_message(content, "test")
    post_all_message("I'm a robot")
  except requests.exceptions.ConnectionError as e:
    print sys.exc_info()[0]
    print "send error!"

if __name__ == "__main__":
  main()
