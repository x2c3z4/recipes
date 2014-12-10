#--*-- coding:utf-8 --*--
import requests
import sys
import pprint
from BeautifulSoup import BeautifulSoup
import re
import shutil
import netrc

LOGIN_URL = 'http://www.newsmth.net/nForum/user/ajax_login.json'
POST_URL = 'http://www.newsmth.net/nForum/article/Career_Campus/ajax_post.json'
CAPTCHAR_URL = 'http://www.newsmth.net/nForum/authimg?_t=1418207693299'
s = requests.Session()
s.headers.update({'User-Agent':'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_4) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/37.0.2062.94 Safari/537.36',
'Connection':'keep-alive',
'Content-type':'application/x-www-form-urlencoded'})
headers = {'X-Requested-With':'XMLHttpRequest'}

def get_credentials(mach):
  # default is $HOME/.netrc
  netrc_machine = mach
  info = netrc.netrc()
  (login, account, password) = info.authenticators(netrc_machine)
  return (login,password)

def debugReq(r):
  pp = pprint.PrettyPrinter(indent=4)
  pp.pprint(r.request.__dict__)
  print >>sys.stderr, r.text, r.cookies

def login(user, passwd):
  payload = {
  "id":user,
  "passwd":passwd,
  }
  try:
    r = s.post(LOGIN_URL,data = payload, headers = headers)
    # debugReq(r)
  except requests.exceptions.ConnectionError as e:
    print "login error"

def checkStatus(json):
  if json['ajax_code'] == '0406':
    return True
  return False

def sendPostWithAuth(subject, content, auth):
  payload = {
  "subject":subject,
  "content":content,
  'auth':auth,
  'signature':'0',
  'id':'0',
  }
  try:
    # add nforms id
    # cookies = {'NFORUM':'u4cirv4rtes0hvvq6qe0fk6de7'}
    r = s.post(POST_URL,data = payload, headers = headers)
    return checkStatus(r.json())
  except requests.exceptions.ConnectionError as e:
    print "post error"
    return False

def sendpost(subject, content):
  r = s.get(CAPTCHAR_URL, stream=True)
  with open('chapt.png', 'wb') as out_file:
      shutil.copyfileobj(r.raw, out_file)
  # auth = raw_input("Please enter chaptcode: ")
  for i in range(18, -18, -1):
    auth = i
    if sendPostWithAuth(subject, content, auth):
      break

def main():
  (user, passwd) = get_credentials('general_user')
  login(user, passwd)
  sendpost("ORACLE招聘", "")

if __name__ == "__main__":
  main()
