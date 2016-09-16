#--*-- coding:utf-8 --*--
#pip install requests BeautifulSoup tabulate markdown
import requests, requests.utils, pickle
import httplib
import sys
import pprint
from BeautifulSoup import BeautifulSoup
import re
import shutil
import netrc
import os.path
import time
import datetime
from tabulate import tabulate
import markdown
import codecs
import atexit
from optparse import OptionParser
from time import gmtime, strftime
import io
# from markdown.extensions.toc import TocExtension

output="/tmp/house.html"
class Mdprint:
  def __init__(self):
    self.out_base,_=os.path.splitext(output)
    self.out_md= self.out_base + ".md"
    self.out_md_fd = io.open(self.out_md, 'w+', encoding='utf8')
    atexit.register(self.save_md_html)

  def write(self, content):
    self.out_md_fd.write(content)

  def write_table(self, title, table, headers, has_stats):
    title = "\n[%s](%s)\n--------\n--------\n" % (title,"http://bj.lianjia.com/ershoufang/rs%E5%AE%89%E5%A4%96%E8%8A%B1%E5%9B%AD/")
    self.write(title)

    if len(table) == 0:
        return

    if has_stats:
        ''' add summary'''
        total = ["Total"]
        total.extend([sum(t[i] for t in table) for i in range(1, len(headers))])
        table.append(total)
    self.out_md_fd.write(u'\n')
    self.out_md_fd.write(tabulate(table, headers=headers, tablefmt="html"))
    self.out_md_fd.write(u'\n')

  def save_md_html(self):
    self.out_md_fd.close()

    with io.open(self.out_base + ".html", 'w+', encoding='utf8') as f:
      f.write(u'''<head>
<meta charset="UTF-8">
</head>''')
      f.write(markdown.markdown(io.open(self.out_md, 'r', encoding='utf8').read(), extensions=['markdown.extensions.tables']))


s = requests.Session()
s.headers.update({'User-Agent':'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_4) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/37.0.2062.94 Safari/537.36',
'Connection':'keep-alive',
'Content-type':'application/x-www-form-urlencoded'})
# headers = {'X-Requested-With':'XMLHttpRequest'}
def saveCookies():
  with open(COOKIE_FILE, 'w') as f:
      pickle.dump(requests.utils.dict_from_cookiejar(s.cookies), f)

def loadCookies():
  with open(COOKIE_FILE) as f:
      cookies = requests.utils.cookiejar_from_dict(pickle.load(f))
      s.cookies = cookies
  print >>sys.stdout, '[+] load cookies!!!'

def patch_send():
    old_send= httplib.HTTPConnection.send
    def new_send( self, data ):
        print >>sys.stdout, '>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>'
        print >>sys.stdout, data
        print >>sys.stdout, '<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<'
        return old_send(self, data) #return is not necessary, but never hurts, in case the library is changed
    httplib.HTTPConnection.send= new_send

#patch_send()


def debugReq(r):
  pp = pprint.PrettyPrinter(indent=4)
  # pp.pprint(r.status_code)
  # pp.pprint(r.request.__dict__)
  # print >>sys.stdout, r.text
  print >>sys.stdout, s.cookies.get_dict()

def format_markdown(headline, items):
    return


def get_page(url):
  try:
    r = s.get(url)
    r.encoding = 'utf-8'
    # debugReq(r)
    return r.text
  except requests.exceptions.ConnectionError as e:
    print >> sys.stderr, "Get page error"

def main():
  md = Mdprint()
  parser = OptionParser(usage='%prog 小区名称', description='查询小区')
  # parser.add_option("-r", "--re", type="string", help='小区名称')
  # (options, args) = parser.parse_args()
  # if options.re:
  #     matched_re = options.re

  # if options.output:
  #     output = options.output
  #if not os.path.exists(COOKIE_FILE):
  url = "http://bj.lianjia.com/ershoufang/rs%E5%AE%89%E5%A4%96%E8%8A%B1%E5%9B%AD/"
  page = get_page(url)
  soup = BeautifulSoup(page, convertEntities=BeautifulSoup.HTML_ENTITIES)
  items = soup.find("ul", attrs={'class':'listContent'}).findAll('li')
  houses = []
  for item in items:
    # house = {}
    # house['title'] = item.div.div.a.text
    # house['address'] = item.find("div", attrs={'class':'address'}).text
    # house['flood'] = item.find("div", attrs={'class':'flood'}).text
    # house['follow'] = item.find("div", attrs={'class':'followInfo'}).text
    # house['tag'] = item.find("div", attrs={'class':'tag'}).text
    # house['price'] = item.find("div", attrs={'class':'priceInfo'}).text
    house = []
    house.append(item.div.div.a.text)
    house.append(item.find("div", attrs={'class':'address'}).text)
    house.append(item.find("div", attrs={'class':'flood'}).text)
    house.append(item.find("div", attrs={'class':'followInfo'}).text)
    #house.append(item.find("div", attrs={'class':'tag'}).text)
    house.append(item.find("div", attrs={'class':'priceInfo'}).div.text)

    #单价80239元/平米
    price_per = item.find("div", attrs={'class':'priceInfo'}).findAll('div')[1].span.text
    price_per = int(re.search(r'\d+', price_per).group())
    house.append(price_per)
    houses.append(house)
    # print house

  houses_list = sorted(houses, key = lambda house: (-int(house[5])))

  for k,v in enumerate(houses_list):
    v.insert(0, k + 1)

  #headers = ['title', 'address', 'flood', 'follow', 'tag', 'price', 'per']
  headers = ['id', 'title', 'address', 'flood', 'follow', 'price', 'per']
  md.write_table(u"安外花园", houses_list, headers, False)


if __name__ == "__main__":
  main()
