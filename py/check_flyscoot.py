#--*-- coding:utf-8 --*--
import requests
import sys
import pprint
from BeautifulSoup import BeautifulSoup
import re

POST_URL = 'http://book.flyscoot.com/Search.aspx'

def get_cookies(s):
  r = s.get(POST_URL);
  # pp = pprint.PrettyPrinter(indent=4)
  # pp.pprint(r.request.__dict__)
  # print
  # print

# get list of apply
def filter_body(page):
  levy = 74
  exchange_rate = 5
  soup = BeautifulSoup(page)
  try:
    fly = float(soup.findAll('li',attrs={'class':'saver'})[1].find("span").contents[0])
    fly_last = (fly + levy) * exchange_rate
    print >>sys.stderr, "{0:20}: ${1:<10} {2:<10}".format("fly", fly, fly_last)
    flybag = float(soup.findAll('li',attrs={'class':'flexi'})[1].find("span").contents[0])
    flybag_last = (flybag + levy) * exchange_rate
    print >>sys.stderr, "{0:20}: ${1:<10} {2:<10}".format("flybag", flybag, flybag_last)
  except:
    print >>sys.stderr, sys.exc_info()[0]
    return
  # rows = soup.find('table',attrs={'id':'tableShow'}).find("tbody").find_all("tr")
  # print >>sys.stderr, "%s:"%(credit_id,)
  # for row in rows:
  #   cells = row.findAll("td")
  #   m = re.search(r'(\d*/\d*/\d*)', cells[1].text)
  #   print >>sys.stderr,'\t', cells[0]['title'],
  #   if m != None:
  #     print >>sys.stderr, '\t',m.group(0),
  #   print >>sys.stderr,'\t', cells[2].text

def main():
  s = requests.Session()
  s.headers.update({
    'User-Agent':'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_4) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/37.0.2062.94 Safari/537.36',
    'Connection':'keep-alive',
    'Content-type':'application/x-www-form-urlencoded'})
  payload = {
  '__EVENTTARGET':'AvailabilitySearchInputSearchView$LinkButtonSubmit',
  'AvailabilitySearchInput.SearchStationDatesList[0].DepartureStationCode':'SIN',
  'AvailabilitySearchInput.SearchStationDatesList[0].ArrivalStationCode':'TSN',
  'AvailabilitySearchInput.SearchStationDatesList[0].DepartureDate':'10/2/2014',
  'a':'on',
  'AvailabilitySearchInput.AdultsCount':'1',
  'AvailabilitySearchInput.ChildsCount':'0',
  'AvailabilitySearchInput.InfantsCount':'0',
  }
  get_cookies(s);
  try:
    r = s.post(POST_URL, data=payload)
    filter_body(r.text)
  except requests.exceptions.ConnectionError as e:
    print '[-] Max retries exceeded, update session'
    print e
  # pp = pprint.PrettyPrinter(indent=4)
  # pp.pprint(r.request.__dict__)
  # print
  # print
  # pp.pprint(r.__dict__)
  # print
  # print
  # print >>sys.stderr, r.text

if __name__ == "__main__":
  main()
