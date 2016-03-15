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
# from markdown.extensions.toc import TocExtension

matched_re = ""
output="/var/www/html/bugs.html"
bl = None
md = None
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
    self.out_base,_=os.path.splitext(output)
    self.out_md= self.out_base + ".md"
    self.out_md_fd = open(self.out_md, 'w+')
    self.out_md_fd.write("[TOC]\n")
    atexit.register(self.save_md_html)

  def write(self, content):
    self.out_md_fd.write(content)

  def write_table(self, title, table, headers, has_stats):
    title = "\n[%s](#)\n--------\n--------\n" % (title,)
    self.write(title)

    if len(table) == 0:
        return

    if has_stats:
        ''' add summary'''
        total = ["Total"]
        total.extend([sum(t[i] for t in table) for i in range(1, len(headers))])
        table.append(total)
    self.out_md_fd.write('\n')
    self.out_md_fd.write(tabulate(table, headers=headers, tablefmt="html"))
    self.out_md_fd.write('\n')

  def save_md_html(self):
    self.out_md_fd.close()

    with open(self.out_base + ".html", 'w+') as f:
      f.write(markdown.markdown(open(self.out_md, 'r').read(), extensions=['markdown.extensions.tables', 'markdown.extensions.toc']))


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

#patch_send()


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
  Get hidden input
  '''
  TMP_URL='https://gmp.oracle.com/captcha/'
  r = s.get(TMP_URL)
  #debugReq(r)
  soup = BeautifulSoup(r.text)
  hidden_tags = soup.findAll("input", type="hidden")
  hidden_dict = {}
  payload = {}
  for tag in hidden_tags:
      hidden_dict[tag['name']] = tag['value']

  for tag in ('v', 'request_id', 'OAM_REQ', 'site2pstoretoken', 'locale'):
    payload[tag] = hidden_dict[tag]
  payload['ssousername'] = user,
  payload['password'] = passwd,
  try:
    r = s.post(LOGIN_URL,data = payload)
    #debugReq(r)
  except requests.exceptions.ConnectionError as e:
    print "login error"

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
    bug["bugno"] = tmp[-13]
    bug["subject"] = tmp[-1]
    bug["bugno"] = '<a href="https://bug.oraclecorp.com/pls/bug/webbug_print.show?c_rptno=%s">%s</a>' %(bug["bugno"],bug["bugno"])
    # useful
    bug["reported_date"] = tmp[-11]
    bug["reported_by"] = tmp[-10]
    bug["serverity"] = tmp[-8]
    bug["status"] = tmp[-7]
    bug["subcomponent"] = tmp[-4]
    bug["re"] = tmp[-2]

    if len(matched_re) ==0 or bug["re"].encode("utf8") == matched_re:
      bugs.append(bug)

  return bugs

def format_markdown(username, bugs):
  headers =['Id', 'Bugno', 'Subject', 'Reported date', 'Reported by', 'Serverity', 'Status', 'Sub', 'Re']

  bugs_list = []
  for bug in bugs:
    bug_list = []
    bug_list.append(bug["bugno"])
    bug_list.append(bug["subject"])
    bug_list.append(bug["reported_date"])
    bug_list.append(bug["reported_by"])
    bug_list.append(bug["serverity"])
    bug_list.append(bug["status"])
    bug_list.append(bug["subcomponent"])
    bug_list.append(bug["re"])
#    bug_list.append(bug["link"])
    bugs_list.append(bug_list)
  bugs_list = sorted(bugs_list, key = lambda bug_list: (int(bug_list[4]), int(bug_list[5])))

  for k,v in enumerate(bugs_list):
    v.insert(0, k + 1)
  md.write_table(username, bugs_list, headers, False)


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
#users = ("LILIHE", "LLFENG", "XIALILI", "XIALILI2", "CHUTIAN", "WENWAWAN", "WENBOLI", "SHENGZHA", "YIZZHANG", "RMIAO") #"ORZHANG" "MMUZIK",
users = ("LLFENG", "CHUTIAN", "WENWAWAN", "HEMZHAO", "CHORNE", "ORZHANG", "MMYU", "SHENGZHA", "YIZZHANG", "CRHU", "RMIAO", "WENBOLI", "CHENLCH", "SUXLI")
headers_serverity=["USER", "S1", "S2", "S3", "S4"]
headers_cata=["USER", "MPxIO", "SD/SCSA", "iSCSI", "FC", "MISC"]

mpxio_subs=("IO-MULTIPATH", "LIBMPAPI", "LIBMPSCSI_VHCI", "MPATHADM")
sd_scsa_subs=("SD-FIXED","SCSA","SES","SSD","LIBEFI","DISKFORMAT","CMLB","LIBFDISK","SCSI","FDISK")
iscsi_subs=("ISCSI","LIBIMA","ISNS","ISCSIADM","ISER","IDM")
fc_subs=("FCIP","FCOE","FCP","FCSM","FC_TRANSPORT","CFGADM_FP","LIBA5K","LIBG_FC","LIBHBAAPI","LIBSUN_FC","FCINFO","LUXADM","NPIV")
misc_subs=("LIBSUN_SAS","SASINFO","SDPARM","SG3UTILS","SMPUTILS")

def _stat_cata(user, bugs):
  s1 = 0
  s2 = 0
  s3 = 0
  s4 = 0
  s5 = 0

  for bug in bugs:
    if bug["subcomponent"].encode('utf8') in mpxio_subs:
      s1 = s1 + 1
    elif bug["subcomponent"].encode('utf8') in sd_scsa_subs:
      s2 = s2 + 1
    elif bug["subcomponent"].encode('utf8') in iscsi_subs:
      s3 = s3 + 1
    elif bug["subcomponent"].encode('utf8') in fc_subs:
      s4 = s4 + 1
    elif bug["subcomponent"].encode('utf8') in misc_subs:
      s5 = s5 + 1
  return [user, s1, s2, s3, s4, s5]

def _stat_serv(user, bugs):
  s1 = 0
  s2 = 0
  s3 = 0
  s4 = 0

  for bug in bugs:
    ch = bug["serverity"].encode('utf8')
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
  out = "\nReported in this week(%s)\n==============\n" % (
          strftime("%Y-%m-%d", gmtime()),)
  print >> sys.stderr, out
  md.write(out)

  stats1 = []
  stats2 = []
  for user in users:
    print >> sys.stderr, user

    url = "https://bug.oraclecorp.com/pls/bug/WEBBUG_REPORTS.do_edit_report?rpt_title=&fcont_arr=0&fid_arr=43&fcont_arr=60&fid_arr=42&fcont_arr=%3D&fid_arr=159&fcont_arr=" + user + "&fid_arr=6&fcont_arr=&fid_arr=122&fcont_arr=AND&fid_arr=136&fcont_arr=&fid_arr=138&fcont_arr=7&fid_arr=9&fcont_arr=INTERNAL%25&fid_arr=200&fcont_arr=&fid_arr=10&fcont_arr=off&fid_arr=157&fcont_arr=2&fid_arr=100&cid_arr=2&cid_arr=3&cid_arr=14&cid_arr=9&cid_arr=8&cid_arr=7&cid_arr=11&cid_arr=6&cid_arr=15&cid_arr=5&cid_arr=51&cid_arr=13&f_count=12&c_count=12&query_type=2"

    #url="https://bug.oraclecorp.com/pls/bug/WEBBUG_REPORTS.do_edit_report?rpt_title=&fcont_arr=%3D&fid_arr=159&fcont_arr=" + user + "&fid_arr=6&fcont_arr=&fid_arr=122&fcont_arr=AND&fid_arr=136&fcont_arr=&fid_arr=138&fcont_arr=7&fid_arr=9&fcont_arr=INTERNAL%25&fid_arr=200&fcont_arr=off&fid_arr=157&fcont_arr=2&fid_arr=100&cid_arr=2&cid_arr=15&cid_arr=3&cid_arr=9&cid_arr=8&cid_arr=7&cid_arr=30&cid_arr=11&cid_arr=6&cid_arr=5&cid_arr=51&cid_arr=13&f_count=9&c_count=12&query_type=2"
    #print >>sys.stderr, url
    bugs = extract(get_bugs_page(url))
    stats1.append(_stat_serv(user, bugs))
    stats2.append(_stat_cata(user, bugs))
    format_markdown(user, bugs)

  md.write_table("Serverity Summary", stats1, headers_serverity, True)
  md.write_table("User Summary", stats2, headers_cata, True)

def staff_bugs_all():
  out = "\nUnsolve bugs(%s)\n=================\n" % (
          strftime("%Y-%m-%d", gmtime()),)
  print >> sys.stderr, out
  md.write(out)

  stats1 = []
  stats2 = []
  for user in users:
    print >> sys.stderr, user
    url ="https://bug.oraclecorp.com/pls/bug/WEBBUG_REPORTS.do_edit_report?rpt_title=&fcont_arr=0&fid_arr=43&fcont_arr=60&fid_arr=42&fcont_arr=%3D&fid_arr=159&fcont_arr=" + user + "&fid_arr=6&fcont_arr=&fid_arr=122&fcont_arr=AND&fid_arr=136&fcont_arr=&fid_arr=138&fcont_arr=INTERNAL%25&fid_arr=200&fcont_arr=off&fid_arr=157&cid_arr=2&cid_arr=3&cid_arr=14&cid_arr=9&cid_arr=8&cid_arr=7&cid_arr=11&cid_arr=6&cid_arr=15&cid_arr=5&cid_arr=51&cid_arr=13&f_count=9&c_count=12&query_type=2"

# url = "https://bug.oraclecorp.com/pls/bug/WEBBUG_REPORTS.do_edit_report?rpt_title=&fcont_arr=9&fid_arr=43&fcont_arr=60&fid_arr=42&fcont_arr=%3D&fid_arr=159&fcont_arr=" + user + "&fid_arr=6&fcont_arr=&fid_arr=122&fcont_arr=AND&fid_arr=136&fcont_arr=&fid_arr=138&fcont_arr=INTERNAL%25&fid_arr=200&fcont_arr=off&fid_arr=157&fcont_arr=2&fid_arr=100&cid_arr=2&cid_arr=15&cid_arr=3&cid_arr=9&cid_arr=8&cid_arr=7&cid_arr=30&cid_arr=11&cid_arr=6&cid_arr=5&cid_arr=51&cid_arr=13&f_count=10&c_count=12&query_type=2"
    #print >>sys.stderr, url
    bugs = extract(get_bugs_page(url))
    stats1.append(_stat_serv(user, bugs))
    stats2.append(_stat_cata(user, bugs))
    format_markdown(user, bugs)


  md.write_table("Serverity Summary", stats1, headers_serverity, True)
  md.write_table("User Summary", stats2, headers_cata, True)


def _is_lastest_bugs(bug, in_days):
  if bl.is_in_blacklist(bug[2]):
    print >>sys.stderr, "%s in blacklist"%(bug[2], )
    return False

  url="https://bug.oraclecorp.com/pls/bug/webbug_print.show?c_rptno=" + bug[2]
  r = s.get(url)
  soup = BeautifulSoup(r.text, convertEntities=BeautifulSoup.HTML_ENTITIES)
  items = soup.findAll('b')

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
  out="\nCompleted in this week(%s)\n==================================\n" % (
          strftime("%Y-%m-%d", gmtime()),)
  print >> sys.stderr, out
  md.write(out)

  stats1 = []
  stats2 = []
  for user in users:
    print >> sys.stderr, user
    ''' use fixed col from bugdb'''
    url = "https://bug.oraclecorp.com/pls/bug/WEBBUG_REPORTS.do_edit_report?rpt_title=&fcont_arr=70&fid_arr=43&fcont_arr=100&fid_arr=42&fcont_arr=%3D&fid_arr=159&fcont_arr=" + user + "&fid_arr=6&fcont_arr=&fid_arr=122&fcont_arr=AND&fid_arr=136&fcont_arr=&fid_arr=138&fcont_arr=7&fid_arr=47&fcont_arr=INTERNAL%25&fid_arr=200&fcont_arr=&fid_arr=10&fcont_arr=off&fid_arr=157&fcont_arr=2&fid_arr=100&cid_arr=2&cid_arr=3&cid_arr=14&cid_arr=9&cid_arr=8&cid_arr=7&cid_arr=11&cid_arr=6&cid_arr=15&cid_arr=5&cid_arr=51&cid_arr=13&f_count=12&c_count=12&query_type=2"

    #url="https://bug.oraclecorp.com/pls/bug/WEBBUG_REPORTS.do_edit_report?rpt_title=&fcont_arr=%3D&fid_arr=159&fcont_arr=" + user + "&fid_arr=6&fcont_arr=&fid_arr=122&fcont_arr=AND&fid_arr=136&fcont_arr=&fid_arr=138&fcont_arr=7&fid_arr=47&fcont_arr=INTERNAL%25&fid_arr=200&fcont_arr=off&fid_arr=157&fcont_arr=2&fid_arr=100&cid_arr=2&cid_arr=15&cid_arr=3&cid_arr=9&cid_arr=8&cid_arr=7&cid_arr=30&cid_arr=11&cid_arr=6&cid_arr=5&cid_arr=51&cid_arr=13&f_count=9&c_count=12&query_type=2"
    #print >>sys.stderr, url
    bugs = extract(get_bugs_page(url))
    ''' get status=60 from bugdb'''
    url = "https://bug.oraclecorp.com/pls/bug/WEBBUG_REPORTS.do_edit_report?rpt_title=&fcont_arr=60&fid_arr=4&fcont_arr=%3D&fid_arr=159&fcont_arr=" + user + "&fid_arr=6&fcont_arr=&fid_arr=122&fcont_arr=AND&fid_arr=136&fcont_arr=&fid_arr=138&fcont_arr=7&fid_arr=35&fcont_arr=INTERNAL%25&fid_arr=200&fcont_arr=&fid_arr=10&fcont_arr=off&fid_arr=157&fcont_arr=2&fid_arr=100&cid_arr=2&cid_arr=3&cid_arr=14&cid_arr=9&cid_arr=8&cid_arr=7&cid_arr=11&cid_arr=6&cid_arr=15&cid_arr=5&cid_arr=51&cid_arr=13&f_count=11&c_count=12&query_type=2"
    bugs.extend(extract(get_bugs_page(url)))
    stats1.append(_stat_serv(user, bugs))
    stats2.append(_stat_cata(user, bugs))
    format_markdown(user, bugs)

  md.write_table("Serverity Summary", stats1, headers_serverity, True)
  md.write_table("User Summary", stats2, headers_cata, True)


def main():
  global matched_re,output
  global bl, md
  parser = OptionParser(usage='%prog [options] [word]', description='checkout')
  parser.add_option("-r", "--re", type="string", help='get only match re, like 12.0')
  parser.add_option("-o", "--output", type="string", help='output file, default /var/www/html/bugs.html')
  (options, args) = parser.parse_args()
  if options.re:
      matched_re = options.re

  if options.output:
      output = options.output
  #if not os.path.exists(COOKIE_FILE):
  bl = Blacklist()
  md = Mdprint()
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
