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
# from markdown.extensions.toc import TocExtension

matched_re = ""
class Blacklist:

  def __init__(self):
    self.file='/tmp/bugno.txt'
    self.lines = []
    self.fd = open(self.file, 'w+')
    self.lines = self.fd.read().splitlines()
    atexit.register(self.save_blacklist)

  def add_blacklist_bug(self, bugno):
    self.lines.append(bugno)

  def is_in_blacklist(self, bugno):
    return bugno in self.lines

  def save_blacklist(self):
    self.fd.write('\n'.join(self.lines))
    self.fd.close()

class Mdprint:

  def __init__(self):
    self.out_md="/tmp/out.md"
    self.out_md_fd = open(self.out_md, 'w+')
    self.out_md_fd.write("[TOC]\n")
    atexit.register(self.save_md_html)

  def write(self, content):
    self.out_md_fd.write(content)

  def write_table(self, table, headers):
    out="\nSummary\n--------------\n"
    self.write(out)

    ''' add summary'''
    total = ["Total"]
    total.extend([sum(t[i] for t in table) for i in range(1, len(headers))])

    table.append(total)
    self.out_md_fd.write('\n')
    self.out_md_fd.write(tabulate(table, headers=headers, tablefmt="pipe"))
    self.out_md_fd.write('\n')

  def save_md_html(self):
    self.out_md_fd.close()

    with open("/tmp/out.html", 'w+') as f:
      f.write(markdown.markdown(open(self.out_md, 'r').read(), extensions=['markdown.extensions.tables', 'markdown.extensions.toc']))

bl = Blacklist()
md = Mdprint()

COOKIE_FILE='/tmp/.oracle.cookies'
LOGIN_URL = 'https://login.oracle.com/oam/server/sso/auth_cred_submit'

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

patch_send()


def get_credentials(mach):
  # default is $HOME/.netrc
  netrc_machine = mach
  info = netrc.netrc()
  (login, account, password) = info.authenticators(netrc_machine)
  return (login,password)

def debugReq(r):
  pp = pprint.PrettyPrinter(indent=4)
  # pp.pprint(r.status_code)
  # pp.pprint(r.request.__dict__)
  # print >>sys.stdout, r.text
  print >>sys.stdout, s.cookies.get_dict()


def login(user, passwd):
  '''
  1. Get site2pstoretoken from https://bug.oraclecorp.com/pls/bug/webbug_print.show?c_rptno=21189094
  2.
  '''
  payload = {
  'v':'v1.4',
  'site2pstoretoken': 'v1.2~840F48D0~E6A73A53563BFCB99C31AE70E224BE860E0F4C21577ED9570B26D809E93574CB2F619286A2FA77F217F6EF66110F8C6E6413C9D4B10F0C9662813DC35881987A4132B95F104125D688BFC094312FCA9F59C9B02048A937E7935DC02BE685B2D34A800E10A6BDC62D77B28559C045C2667CC9E516A5E276BA27E27FC79D9AB1F0E0A6EF6444BEF4F872A82572AC8B380DAA5C3C5E718F86D09DF5D5411D0119AEE3C120DC72E595B9D46181D551068CE82D9EBFAAA24612AFF70DFD48194629F3A43B812F5C205F2FCDFEDECD4043DE3BA4C503328907C7FA8DB9CB7DD55FFC88',
  'request_id':'8283486531936143424',
  'source':'index_nav',
  'OAM_REQ':'VERSION_4~T5nx%2fqs7i7spvPxOphnRYHTKiml46awhmgFe%2fGBQpuTaFoT5vYC09OA9aeaoldGxfA63vvGY73%2beQ%2fyWHtZjF9TCmKqWNqCe4zMeE6YJ1g7Wus6CRYmJYfH6eULbnn6iy85S6mUIPEIRPT5IPTDfFzhTHpGaJMoz24ckXzPFmm5UvStfc9XV7KqqXSneXRU6jS2ATmpy3Fz4TlD%2b60CAl5CxaseO2VzF7T1PE6BeAavtqyLPzAcgxlvYCBHftIeV%2bq%2bJQmXjwxmiswEs38UYQ5%2bsKzI3LZapxBoB%2fyIUEf7IRzxeafOiG4%2b7WW2Fn24HMG8Lg%2fFr09qbLOx%2bbbnfzR8Tt9MH0cKVbq7u9G766%2fM4uncQBrVX1LD79TdAQFULM2B3aTOBqH5dRN9tqTOWh%2f0STukb9wk%2f9qfzr%2bxJn2cHUOfD0RqMDkVY%2fY6F67UAoAZAc4DssUA3AeKniCX%2b6MdxoAw5NVZQrvz8A9jP0xY8mykzzqpNIUKNyLnePdBjKoA6CvPlaEEG3UrM9QzBCZNWQhYcUzMGDLqhjheepzmbVvpsu%2fNN%2bZ1nrHpHxBE9V8EHqO4KAzWQBkdqvhk4w2ePKnuIbwbdzq%2fJYhBzZe4hCSbV7Rkk3mvFqdFl9d3sumizsEDveyVlUj0aIpPmOvmM%2feGT14hOEDKrZcWUMzdhOMmfRqp%2bS5jzLwcMQ1eFCL4Af0oNJHkq1IcVYcqXVezqYHsSM1GyUewu4iN0uz8dfShR9mGFDwQk3gXJxwWt6rmmnMUWePEPIUv9xkhyJjk46c%2fuRX%2fJkeQb1ZHisgKleC8c4zqmMF4t7L4gpqrE5vn2DjWnq7nLp7Lnt8F3CRANVGJsNmI%2fuDaFvboXSzcd84bCeQuUsnGWv8JCXSbEZAiW2EeOCLnmxHrHSptptZj7wVchvCF4znCx%2fjJsB4rPvRUO0%2fkwUFX9pBYpOu52EX81aNJv6CPIHmVMopqPVTahV4RxWMhKtXld9M2etAh4jlkBwz6o0om%2fAgMKYyNsTJLoIYbJaXLGUMYPpRV4toIwUJVb%2bR86Y1rFjnY%2fyeI7wvaLOlmR%2bu7I15AKFHZAivShuQ9NkAYm6tzOUA%2frk8A6bWeOk5UcqqR29Pxbej89HqnSIEPDi3TGZhzUJHz8XmXYVwXy7PgIwv4d64To8s2E%2bwRIzWShc%2fPWPUE%2bAiV%2fJT2TOk0qaDJsmFIEgNSpQHDYTjE9gkc9alSmcZIRLFm7rJW60i6jy5Ko6qrjV0rjIkmIMkDo9aQD4RDDSeNZBCeLh2dAdlWEnPcSGN3prPp3mzp1l9LiqNBwNnEtsI90Kmw9TJl5d%2bc3FWCViaE23Woo%2bUhxJB5WWcDFeakGfWw8dhL%2f5mB7K3tOpr3ONTKYowz5aRLG28o9CL%2fZhDYRRHm6Qo41MWBLTIC%2bES0TTiR1DH%2be%2bVgE4fgKjQLdvDaG3utkJNcJ3uEmDA0yvSFPAodjTamkdh49YSYy4RC3AaKp8Et7p7IHMGTj%2bUqZXkzwmX2oLq2ueQeip3r4Ztc9tguJhCtZuYLfcJUufIV6MdkPN3uiF0vzoGDOoH0ngJumJXTNWwO2jW9HcoQVyA97%2flhseVk0PCqavFRCiOIWevic%2fjiVbXQcCFUJi6PrPknLDq5XZrtz5zjTBpJUoMmD',
  'site2pstoretoken':'v1.4~7A14C962~77456B99BCD3B25EA25BD81D9B0F5BF45F2FF80242A4B8EE5871F67E3B02484C505F35273C5C931A2C0CA6F32F695F94B4F0F1515AD3D9E2DE8ED4F20BC82F8201DDEBE82ECFD83879670FBE2CA209617A103AB640DBC2779422F8E4C28A7E97F49C2FF11BE22F40B6AAED3379A030ED34C47E6B7EEA5B2C7E8443EF890B0DCE51F06F1FCD71FDD9672870215E9C4E7D14D6B06DD63E7C682FA80D0EFF1F9DA36D0B21C673827474',
  'ssousername':user,
  'password':passwd,
  }
  try:
    r = s.post(LOGIN_URL,data = payload)
    #debugReq(r)
  except requests.exceptions.ConnectionError as e:
    print >> sys.stderr, "login error"

def extract(page):
  soup = BeautifulSoup(page, convertEntities=BeautifulSoup.HTML_ENTITIES)
  items = soup.find("tbody", attrs={'id':'data'}).findAll('tr', attrs={'style':'font-family:Arial,Helvetica,Geneva,sans-serif;font-size:9pt;'})
  tmps = []
  bugs = []
  '''
  [u'1.', u'', u'21502504', u'', u'IO-MULTIPATH', u'24-JUL-15', u'YIZZHANG', u'2', u'11', u'Y', u'10006', u'KERNEL', u'11.3', u'11.3', u'rm-io panic at vhci_bind_transport+0x714']
  '''
  for item in items:
    tmps.append([i.text for i in item.findAll('td')])

  for tmp in tmps:
    bug = {}
    bug["bid"] = (tmp[0], "")
    bug["bugno"] = (tmp[-13], "")
    bug["subject"] = (tmp[-1], "Subject")
    bug["0"] = "%s Bug [%s](https://bug.oraclecorp.com/pls/bug/webbug_print.show?c_rptno=%s) - %s\n"%(bug["bid"][0], bug["bugno"][0], bug["bugno"][0], bug["subject"][0])
    # useful
    bug["reported_date"] = (tmp[-11], "Reported date")
    bug["reported_by"] = (tmp[-10],"Reported by")
    bug["serverity"] = (tmp[-8], "Serverity")
    bug["status"] = (tmp[-7], "Status")
    bug["subcomponent"] = (tmp[-4], "Subcomponent")
    bug["re"] = (tmp[-2], "Re")

    if len(matched_re) ==0 or bug["re"][0].encode("utf8") == matched_re:
      bugs.append(bug)

  return bugs

def format_markdown(bugs):
  for bug in bugs:
    desc_list = []
    # print >>sys.stderr, bug
    out = bug["0"]
    desc_list.append(bug["reported_by"])
    desc_list.append(bug["reported_date"])
    desc_list.append(bug["serverity"])
    desc_list.append(bug["status"])
    desc_list.append(bug["subcomponent"])
    desc_list.append(bug["re"])

    out += "\t".join([item[1] + ":" + item[0]  for item in desc_list]) + "\n\n"
    print out

    md.write(out)

def get_bugs_page(url):
  try:
    r = s.get(url)
    # debugReq(r)
    return r.text
  except requests.exceptions.ConnectionError as e:
    print >> sys.stderr, "Get list error"

def get_bugs_list(uuid, status, reported_days = '0'):
  try:
    if reported_days is "0":
      """
      get user's status's bugs
      """
      url="https://bug.oraclecorp.com/pls/bug/WEBBUG_REPORTS.do_edit_report?rpt_title=&fcont_arr=" + status + "&fid_arr=4&fcont_arr=" + uuid + "&fid_arr=6&fcont_arr=&fid_arr=10&fcont_arr=off&fid_arr=157&fcont_arr=2&fid_arr=100&cid_arr=2&cid_arr=3&cid_arr=9&cid_arr=8&cid_arr=7&cid_arr=30&cid_arr=11&cid_arr=6&cid_arr=5&cid_arr=51&cid_arr=13&f_count=5&c_count=11&query_type=1"
    elif status is "0":
      """
      get user's all bugs
      """
      url="https://bug.oraclecorp.com/pls/bug/WEBBUG_REPORTS.do_edit_report?rpt_title=&fcont_arr=" + uuid + "&fid_arr=6&fcont_arr=&fid_arr=10&fcont_arr=off&fid_arr=157&fcont_arr=2&fid_arr=100&cid_arr=2&cid_arr=3&cid_arr=9&cid_arr=8&cid_arr=7&cid_arr=30&cid_arr=11&cid_arr=6&cid_arr=5&cid_arr=51&cid_arr=13&f_count=4&c_count=11&query_type=1"
    else:
      """
      used for get new report bug, not use `status`
      """
      url="https://bug.oraclecorp.com/pls/bug/WEBBUG_REPORTS.do_edit_report?rpt_title=&fcont_arr=" + uuid + "&fid_arr=6&fcont_arr=" + reported_days + "&fid_arr=9&fcont_arr=&fid_arr=10&fcont_arr=off&fid_arr=157&fcont_arr=2&fid_arr=100&cid_arr=2&cid_arr=3&cid_arr=9&cid_arr=8&cid_arr=7&cid_arr=30&cid_arr=11&cid_arr=6&cid_arr=5&cid_arr=51&cid_arr=13&f_count=5&c_count=11&query_type=1"

    r = s.get(url)
    # debugReq(r)
    return r.text
  except requests.exceptions.ConnectionError as e:
    print >> sys.stderr, "Get list error"

#users = ("LILIHE", "LLFENG", "XIALILI", "XIALILI2", "CHUTIAN", "WENWAWAN", "WENBOLI", "SHENGZHA", "YIZZHANG", "RMIAO") #"ORZHANG"
users = ("LLFENG", ) #"ORZHANG"
headers_serverity=["USER", "S1", "S2", "S3", "S4"]
headers_cata=["USER", "MPxIO", "SD/SCSA"]

mpxio_subs=("IO-MULTIPATH", "LIBMPAPI", "LIBMPSCSI_VHCI", "MPATHADM")
sd_scsa_subs=("SD-FIXED","SCSA","SES","SSD","LIBEFI","DISKFORMAT","CMLB","LIBFDISK","SCSI","FDISK","SASINFO","SDPARM","SG3UTILS","SMPUTILS")

def _stat_cata(user, bugs):
  s1 = 0
  s2 = 0

  for bug in bugs:
    if bug["subcomponent"][0].encode('utf8') in mpxio_subs:
      s1 = s1 + 1
    elif bug["subcomponent"][0].encode('utf8') in sd_scsa_subs:
      s2 = s2 + 1
  return [user, s1, s2]

def _stat_serv(user, bugs):
  s1 = 0
  s2 = 0
  s3 = 0
  s4 = 0

  for bug in bugs:
    ch = bug["serverity"][0].encode('utf8')
    if ch is '1':
      s1 = s1 + 1
    elif ch is '2':
      s2 = s2 + 1
    elif ch is '3':
      s3 = s3 + 1
    elif ch is '4':
      s4 = s4 + 1

  return [user, s1, s2, s3, s4]

def report_new_bugs_one_week():
  out = "\nReported in this week\n==============\n"
  print >> sys.stderr, out
  md.write(out)

  stats1 = []
  stats2 = []
  for user in users:
    out = "\n%s\n--------------\n" % (user, )
    print >> sys.stderr, out
    md.write(out)

    url = "https://bug.oraclecorp.com/pls/bug/WEBBUG_REPORTS.do_edit_report?rpt_title=&fcont_arr=0&fid_arr=43&fcont_arr=70&fid_arr=42&fcont_arr=%3D&fid_arr=159&fcont_arr=" + user + "&fid_arr=6&fcont_arr=&fid_arr=122&fcont_arr=AND&fid_arr=136&fcont_arr=&fid_arr=138&fcont_arr=7&fid_arr=9&fcont_arr=INTERNAL%25&fid_arr=200&fcont_arr=&fid_arr=10&fcont_arr=off&fid_arr=157&fcont_arr=2&fid_arr=100&cid_arr=2&cid_arr=3&cid_arr=14&cid_arr=9&cid_arr=8&cid_arr=7&cid_arr=11&cid_arr=6&cid_arr=15&cid_arr=5&cid_arr=51&cid_arr=13&f_count=12&c_count=12&query_type=2"

    #url="https://bug.oraclecorp.com/pls/bug/WEBBUG_REPORTS.do_edit_report?rpt_title=&fcont_arr=%3D&fid_arr=159&fcont_arr=" + user + "&fid_arr=6&fcont_arr=&fid_arr=122&fcont_arr=AND&fid_arr=136&fcont_arr=&fid_arr=138&fcont_arr=7&fid_arr=9&fcont_arr=INTERNAL%25&fid_arr=200&fcont_arr=off&fid_arr=157&fcont_arr=2&fid_arr=100&cid_arr=2&cid_arr=15&cid_arr=3&cid_arr=9&cid_arr=8&cid_arr=7&cid_arr=30&cid_arr=11&cid_arr=6&cid_arr=5&cid_arr=51&cid_arr=13&f_count=9&c_count=12&query_type=2"
    # print >>sys.stderr, url
    bugs = extract(get_bugs_page(url))
    stats1.append(_stat_serv(user, bugs))
    stats2.append(_stat_cata(user, bugs))
    format_markdown(bugs)

  md.write_table(stats1, headers_serverity)
  md.write_table(stats2, headers_cata)

def staff_bugs_all():
  out = "\nUnsolve bugs\n=================\n"
  print >> sys.stderr, out
  md.write(out)

  stats1 = []
  stats2 = []
  for user in users:
    out = "\n%s\n--------------\n" % (user, )
    print >> sys.stderr, out
    md.write(out)

    url ="https://bug.oraclecorp.com/pls/bug/WEBBUG_REPORTS.do_edit_report?rpt_title=&fcont_arr=0&fid_arr=43&fcont_arr=70&fid_arr=42&fcont_arr=%3D&fid_arr=159&fcont_arr=" + user + "&fid_arr=6&fcont_arr=&fid_arr=122&fcont_arr=AND&fid_arr=136&fcont_arr=&fid_arr=138&fcont_arr=INTERNAL%25&fid_arr=200&fcont_arr=off&fid_arr=157&cid_arr=2&cid_arr=3&cid_arr=14&cid_arr=9&cid_arr=8&cid_arr=7&cid_arr=11&cid_arr=6&cid_arr=15&cid_arr=5&cid_arr=51&cid_arr=13&f_count=9&c_count=12&query_type=2"

# url = "https://bug.oraclecorp.com/pls/bug/WEBBUG_REPORTS.do_edit_report?rpt_title=&fcont_arr=9&fid_arr=43&fcont_arr=60&fid_arr=42&fcont_arr=%3D&fid_arr=159&fcont_arr=" + user + "&fid_arr=6&fcont_arr=&fid_arr=122&fcont_arr=AND&fid_arr=136&fcont_arr=&fid_arr=138&fcont_arr=INTERNAL%25&fid_arr=200&fcont_arr=off&fid_arr=157&fcont_arr=2&fid_arr=100&cid_arr=2&cid_arr=15&cid_arr=3&cid_arr=9&cid_arr=8&cid_arr=7&cid_arr=30&cid_arr=11&cid_arr=6&cid_arr=5&cid_arr=51&cid_arr=13&f_count=10&c_count=12&query_type=2"
    #print >>sys.stderr, url
    bugs = extract(get_bugs_page(url))
    stats1.append(_stat_serv(user, bugs))
    stats2.append(_stat_cata(user, bugs))
    format_markdown(bugs)


  md.write_table(stats1, headers_serverity)
  md.write_table(stats2, headers_cata)


def _is_lastest_bugs(bug, in_days):
  if bl.is_in_blacklist(bug[2]):
    print >>sys.stderr, "%s in blacklist"%(bug[2], )
    return False

  url="https://bug.oraclecorp.com/pls/bug/webbug_print.show?c_rptno=" + bug[2]
  r = s.get(url)
  soup = BeautifulSoup(r.text, convertEntities=BeautifulSoup.HTML_ENTITIES)
  items = soup.findAll('b')
  print url

  content = ""
  for i in reversed(items):
    content = content + i.text
  # print content
  p = re.compile(ur'(\d\d\/\d\d\/\d\d) \d\d:\d\d [ap]m \*\*\*')
  date_str = re.search(p, content).group(1)
  lasted_updated_date = datetime.datetime.fromtimestamp(time.mktime(time.strptime(date_str, "%m/%d/%y")))
  today = datetime.datetime.now()
  delta = today - lasted_updated_date

  if delta.days <= in_days:
    return True
  else:
    bl.add_blacklist_bug(bug[2])
    return False

def staff_completed_one_week():
  out="\nCompleted in this week\n==================================\n"
  print >> sys.stderr, out
  md.write(out)

  stats1 = []
  stats2 = []
  for user in users:
    out="\n%s\n--------------\n" % (user, )
    print >> sys.stderr, out
    md.write(out)
    url = "https://bug.oraclecorp.com/pls/bug/WEBBUG_REPORTS.do_edit_report?rpt_title=&fcont_arr=70&fid_arr=43&fcont_arr=100&fid_arr=42&fcont_arr=%3D&fid_arr=159&fcont_arr=" + user + "&fid_arr=6&fcont_arr=&fid_arr=122&fcont_arr=AND&fid_arr=136&fcont_arr=&fid_arr=138&fcont_arr=7&fid_arr=47&fcont_arr=INTERNAL%25&fid_arr=200&fcont_arr=&fid_arr=10&fcont_arr=off&fid_arr=157&fcont_arr=2&fid_arr=100&cid_arr=2&cid_arr=3&cid_arr=14&cid_arr=9&cid_arr=8&cid_arr=7&cid_arr=11&cid_arr=6&cid_arr=15&cid_arr=5&cid_arr=51&cid_arr=13&f_count=12&c_count=12&query_type=2"

    #url="https://bug.oraclecorp.com/pls/bug/WEBBUG_REPORTS.do_edit_report?rpt_title=&fcont_arr=%3D&fid_arr=159&fcont_arr=" + user + "&fid_arr=6&fcont_arr=&fid_arr=122&fcont_arr=AND&fid_arr=136&fcont_arr=&fid_arr=138&fcont_arr=7&fid_arr=47&fcont_arr=INTERNAL%25&fid_arr=200&fcont_arr=off&fid_arr=157&fcont_arr=2&fid_arr=100&cid_arr=2&cid_arr=15&cid_arr=3&cid_arr=9&cid_arr=8&cid_arr=7&cid_arr=30&cid_arr=11&cid_arr=6&cid_arr=5&cid_arr=51&cid_arr=13&f_count=9&c_count=12&query_type=2"
    print >>sys.stderr, url
    bugs = extract(get_bugs_page(url))
    stats1.append(_stat_serv(user, bugs))
    stats2.append(_stat_cata(user, bugs))
    format_markdown(bugs)

  md.write_table(stats1, headers_serverity)
  md.write_table(stats2, headers_cata)


def main():
  global matched_re
  parser = OptionParser(usage='%prog [options] [word]', description='checkout')
  parser.add_option("-r", "--re", type="string", help='get only match re, like 12.0')
  (options, args) = parser.parse_args()
  if options.re:
      matched_re = options.re

  #if not os.path.exists(COOKIE_FILE):
  if True:
    (user, passwd) = get_credentials('oracle')
    login(user, passwd)
    saveCookies()
  else:
    loadCookies()

  staff_bugs_all()
  report_new_bugs_one_week()
  staff_completed_one_week()

if __name__ == "__main__":
  main()
