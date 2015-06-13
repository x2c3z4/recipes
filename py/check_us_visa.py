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

POST_URL = 'https://ceac.state.gov/CEACStatTracker/Status.aspx'

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

patch_send()


def debugReq(r):
  pp = pprint.PrettyPrinter(indent=4)
  pp.pprint(r.status_code)
  pp.pprint(r.request.__dict__)
  print >>sys.stderr, r.text
  print >>sys.stderr, s.cookies.get_dict()

def get_page(application_id):
  payload = {
      'ctl00$ToolkitScriptManager1': 'ctl00$ContentPlaceHolder1$UpdatePanel1|ctl00$ContentPlaceHolder1$btnSubmit',
      'ctl00_ToolkitScriptManager1_HiddenField': ';;AjaxControlToolkit, Version=3.5.51116.0, Culture=neutral, PublicKeyToken=28f01b0e84b6d53e',
      'ctl00$ContentPlaceHolder1$ddlApplications': 'NIV',
      'ctl00$ContentPlaceHolder1$ddlLocation': 'BEJ',
      'ctl00$ContentPlaceHolder1$txbCase': application_id,
      '__EVENTTARGET': '',
      '__EVENTARGUMENT': '',
      '__LASTFOCUS': '',
      '__VIEWSTATE': '/wEPaA8FDzhkMjYwMGE2ZmVlYjUyYxgBBR5fX0NvbnRyb2xzUmVxdWlyZVBvc3RCYWNrS2V5X18WAQUjY3RsMDAkQ29udGVudFBsYWNlSG9sZGVyMSRidG5TdWJtaXTJydV5cSIjU4FNs4uT5WylGw1qsg==',
      '__EVENTVALIDATION':
      '/wEW6gEC6rr18QgCuueB3QMCu8WcswoC3KrOwwYCxfTx9gIC1+6r8AoC9MfXtA8C5tme3wICkcD33g8C/tnu3wIC/pjUdQLE3KK2AgKUzuy0CAKBsebrDgLUneWfDAKJleyfAwKIpuzABAL6mIB2Av/Zit8CAqjRrcABAoWViJ8DAtGuscABAv6YhHYC4a3QtgQClLGy6w4C8ce7tQ8C662AtwQCyuP9gAUC7s6YtAgC9Nn63wIC7auJ6QECobiT6gsC/MfvtQ8C6c7AtAgC7quZ6QECn8DT3g8CgsD/3g8Csbjf6gsC4tme3wICvdSVtgEC/7Kr9AsCk8D33g8C46uV6QEC9seftQ8CkLHu6w4CvefmqQYChbHm6w4Csuf6qQYCvbiT6gsChLG26w4Csbif6gsC2bKb9AsC5q28twQCg8DH3g8C/bT+wQ4Ck7HS6g4C4a2ItwQCjpXwnwMCorj76gsCsLjr6gsCgc6wtAgC09zmtQIC+pjAdQKAwK/fDwKGldSfAwKeg+n1DQKriub0DgK5zbOsDwKpiqb1DgK7ir71DgLvmNR1Aq2c0Z4MAqWK/vQOAu/8/aoFAv3Hq7UPAqKcyZ4MAt7c1rUCAuSYrHYC7JiEdgKTzvi0CAKFlZCfAwL9x9e0DwLLx5e1DwKZldyfAwKAlcCfAwKN5+KpBgLm6paABgLl/PmqBQK/iuL0DgK91OW2AQKEwJveDwLcrsXfAQKP1KW2AQKhuPfqCwLE4+GABQKpnOWfDAKP586pBgLpzui0CAKvnL2fDALersnfAQKazsS0CALn6tqfBgLszsS0CALNx9e0DwLf3Mq1AgLd48GABQKAleSfAwL3x++1DwLIsrf0CwKbwP/eDwKq0dHfAQLw2ZLfAgKjiv70DgKw1PW2AQL+tMrBDgLmmIR2Au/8haoFAvPZ4t8CApKx4usOApXOxLQIApyxkusOAtau0d8BAoextusOAuTZ1t4CApOx5usOAvmY2HUCqN+itgIC/8eLtQ8C6vypqwUCq4ra9A4CkrG26w4C9ceLtQ8C9Me/tQ8C4Py1qgUC/8e/tQ8CvLjv6gsC0q65wAEC+ceftQ8CsriT6gsCwMfftA8Cz+PtgAUC6/z5qgUCxOPtgAUC0p+wbALE46GBBQL3x8O0DwL3x4e1DwK3lfyfAwL0x6O1DwL/mLx2AoiV5J8DAtGd3Z4MAurOwLQIAorUsbYBAtuurcABArGmiMAEAv2YqHYCiJXYnwMC1a7x3wECncD/3g8CyLTywQ4Ct6aAwAQC+JjwdQL9q5XpAQLIx5O1DwKeseLrDgLyx5O1DwK356aqBgLC4/WABQKe/IGqBQKe/PmqBQKEldifAwL+2Y7fAgL2x6e1DwLfrrXAAQL39YyrCAKTzsC0CALRnYmfDAL56rqABgLNtK7CDgLn6qqABgL5mNB1AoSx/usOAvjZgt8CAs2u+d8BAvTHw7QPAtvj0YAFAqucyZ4MAtOdpZ8MAtvjxYAFApLA994PApmVxJ8DArDUqbYBAv6YjHYCsqbgwAQClLH+6w4C687EtAgCjZX0nwMCq4ri9A4CrpzJngwC4fyZqgUCnLGe6w4CnvzxqgUC/9m63wICx669wAEChZWAnwMC8JiodgKOg431DQLrzvy0CALwmIR2Atauxd8BAoaV9J8DAur8jaoFAuyYjHYC4JjUdQLVrsXfAQKaseLrDgKinK2fDAKI1IG2AQLW3Ja1AgKinMGeDALrzsi0CAKo36q2AgKFsZ7rDgL/mIR2AouVkJ8DApb8vaoFApWx/usOAuXZvt8CAviYiHYCzrLj9AsC9LKa5g4C5sD0xwQC+NCVogp5wp63hxCpNfOUkr9Yo+MIqLI0Fw==',
      '__ASYNCPOST': 'true',
      'ctl00$ContentPlaceHolder1$btnSubmit.x': '52',
      'ctl00$ContentPlaceHolder1$btnSubmit.y': '16',
  }
  r = s.post(POST_URL, data=payload)
  print r.text
  return
  soup = BeautifulSoup(r.text)
  updated_date = soup.find('span', {'id':'ctl00_ContentPlaceHolder1_ucApplicationStatusView_lblStatusDate'})
  print updated_date
  debugReq(r)

def main():
  #get_page('AA004XDZY4')
  get_page('AA004WVP10')

if __name__ == "__main__":
  main()
