#--*-- coding:utf-8 --*--
import requests, requests.utils, pickle
import httplib
import pprint
import sys

import urllib,urllib2
import datetime
import re
import os
import threading
import time
import random
from optparse import OptionParser
from multiprocessing import Pool

def patch_send():
    old_send= httplib.HTTPConnection.send
    def new_send( self, data ):
        print >>sys.stderr, '>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>'
        print >>sys.stderr, data
        print >>sys.stderr, '<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<'
        return old_send(self, data) #return is not necessary, but never hurts, in case the library is changed
    httplib.HTTPConnection.send= new_send

#patch_send()

def debugReq(r):
  pp = pprint.PrettyPrinter(indent=4)
  #pp.pprint(r.status_code)
  # pp.pprint(r.request.__dict__)
  if len(r.text) > 10:
    print >>sys.stderr, "exception!"
  else:
    print >>sys.stderr, r.text
  #print >>sys.stderr, s.cookies.get_dict()

s = requests.Session()
s.headers.update({'User-Agent':'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_4) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/37.0.2062.94 Safari/537.36',
  'Connection':'keep-alive',
  'Content-type':'application/x-www-form-urlencoded'})


def get_proxies_with_proxy_com_ru():
  proxies = []
  PROXIES_URL = 'http://proxy.com.ru/'
  r = s.get(PROXIES_URL)
  page_lists = re.findall('list_\d+.html', r.text)
  page_lists = list(set(page_lists))
  for page in page_lists:
    r = s.get(PROXIES_URL + page)
    ips = []
    for (ip, port) in re.findall(r'(\d+\.\d+\.\d+\.\d+)</td><td>(\d+)', r.text):
      ips.append("%s:%s"%(ip,port))
    proxies.extend(ips)
  return list(set(proxies))

def get_proxies_with_freeproxylists_net():
  proxies = []
  PROXIES_URL = 'http://www.freeproxylists.net/'
  r = s.get(PROXIES_URL)
  print r.text
  page_lists = re.findall('page=\d+', r.text)
  page_lists = list(set(page_lists))
  print page_lists
  return
  for page in page_lists:
    r = s.get(PROXIES_URL + page)
    ips = []
    for (ip, port) in re.findall(r'(\d+\.\d+\.\d+\.\d+)</td><td>(\d+)', r.text):
      ips.append("%s:%s"%(ip,port))
    proxies.extend(ips)
  return list(set(proxies))


def submit_vote(ip):
  POST_URL = 'http://proj.meeting365.cn/wcbb/index.php/index/tp/'
  payload = {
      'num': '1007',
  }
  proxies = {
      'http' : ip
  }
  try:
    r = s.post(POST_URL, data=payload, proxies=proxies)
    debugReq(r)
  except requests.exceptions.ConnectionError as e:
    print "skip proxy %s"%s(ip,)

def main():
    proxies = get_proxies_with_proxy_com_ru()
    pool = Pool(50)
    print "ips: %d"%(len(proxies),)

    #pool.map(submit_vote, proxies)
    for ip in proxies:
      pool.apply_async(submit_vote, [ip])
    pool.close()
    pool.join()

if __name__=="__main__":
    main()
