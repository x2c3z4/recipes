#--*-- coding:utf-8 --*--
#pip install requests BeautifulSoup tabulate markdown
import requests, requests.utils, pickle
import string
import httplib
import sys
import pprint
from bs4 import BeautifulSoup
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

COOKIE_FILE = '/tmp/.oracle.cookies'
s = requests.Session()
s.headers.update({
    'User-Agent':
    'Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_4) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/37.0.2062.94 Safari/537.36',
    'Connection': 'keep-alive',
    'Content-type': 'application/x-www-form-urlencoded'
})


# headers = {'X-Requested-With':'XMLHttpRequest'}
def saveCookies():
    with open(COOKIE_FILE, 'w') as f:
        pickle.dump(requests.utils.dict_from_cookiejar(s.cookies), f)


def loadCookies():
    with open(COOKIE_FILE) as f:
        cookies = requests.utils.cookiejar_from_dict(pickle.load(f))
        s.cookies = cookies
    print >> sys.stdout, '[+] load cookies!!!'


def patch_send():
    old_send = httplib.HTTPConnection.send

    def new_send(self, data):
        print >> sys.stdout, '>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>'
        print >> sys.stdout, data
        print >> sys.stdout, '<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<'
        return old_send(
            self, data
        )  #return is not necessary, but never hurts, in case the library is changed

    httplib.HTTPConnection.send = new_send

#patch_send()


def get_credentials(mach):
    # default is $HOME/.netrc
    netrc_machine = mach
    info = netrc.netrc()
    (login, account, password) = info.authenticators(netrc_machine)
    return (login, password)


def debugReq(r):
    pp = pprint.PrettyPrinter(indent=4)
    pp.pprint(r.status_code)
    pp.pprint(r.__dict__)
    #pp.pprint(r.request.__dict__)
    #print >>sys.stdout, r.text
    #print >> sys.stdout, s.cookies.get_dict()


def login_bld(user, passwd, login_url):
    "get csrftoken"
    r = s.get(login_url + "?next=/myprojects/")
    ##debugReq(r)
    #soup = BeautifulSoup(r.text)
    #hidden_tags = soup.findAll("input", type="hidden")
    #hidden_dict = {}
    payload = {}
    #for tag in hidden_tags:
    #    hidden_dict[tag['name']] = tag['value']

    #for tag in ('v', 'request_id', 'OAM_REQ', 'site2pstoretoken', 'locale'):
    #    payload[tag] = hidden_dict[tag]
    payload['username'] = user
    payload['password'] = passwd
    payload['next'] = ''
    payload['csrfmiddlewaretoken'] = s.cookies['csrftoken']
    try:
        r = s.post(login_url, data=payload, allow_redirects=False)
        #debugReq(r)
    except requests.exceptions.ConnectionError as e:
        print "[-] login error"
    if r.status_code != 302:
        print "[-] login status code error"
        sys.exit(1)


def create_prj(url, bugno, prj_name):
    if not bugno:
        data = s.get(url).text
        soup = BeautifulSoup(data, "html5lib")
        #print soup.prettify()
        prj_id = soup.find("div", {'id': 'workspace_list'}).findAll("tr")[-1].td.text.strip()
        prj_id = re.search(r'(\d+)', prj_id).group(0)
        #parts = url.split('/')
        #parts[-1] = "projects/" + str(prj_id)
        #url = '/'.join(parts)
        #print url
        #return url
        #print url
        return string.replace(url, "myprojects", "projects/" + str(prj_id))

    payload = {'description': prj_name, 'change_request': bugno}
    try:
        r = s.post(url, data=payload, allow_redirects=False)
        #debugReq(r)
    except requests.exceptions.ConnectionError as e:
        print "[-] create_prj error"
    if r.status_code == 302:
        print "[+] Prj created"
        return r.headers['Location']
    else:
        print "[-] create_prj status code error"
        sys.exit(0)


def create_ws(prj_url, code_path, ws_name):
    #//bld.uk.oracle.com/projects/6445/
    if not ws_name:
        data = s.get(prj_url).text
        soup = BeautifulSoup(data, "html5lib")
        #print soup.prettify()
        ws_id = soup.find("div", {'id': 'workspace_list'}).findAll("tr", {'class':'grey'})[-1].td.a['href']
        #/workspaces/9646/
        parts = prj_url.split('/')
        parts[-2] = ws_id.split('/')[-2]
        parts[-3] = ws_id.split('/')[-3]
        ws_url = '/'.join(parts)
        return ws_url

    prj_id = code_path.split("/")[-2]
    payload = {
            'path': code_path,
            'release': 14,
            'project':prj_id,
            'description': ws_name
            }
    try:
        r = s.post(prj_url, data=payload, allow_redirects=False)
        #debugReq(r)
    except requests.exceptions.ConnectionError as e:
        print "[-] create_ws error"

    if r.status_code == 302:
        print "[+] Ws created"
        return r.headers['Location']
    else:
        print "[-] create_ws status code error"
        sys.exit(0)

def start_build(prj_url):
    #payload = {
    #        'RUN_FORM-description':'',
    #        'RUN_FORM-architecture':'1',
    #        'RUN_FORM-architecture':'2',
    #        'RUN_FORM-storage_host':'1',
    #        'RUN_FORM-build_groups':'54',
    #        'RUN_FORM-bob_options':'4',
    #        'baseline':'461',
    #        'RUN_FORM-release_to_use':''
    #        }
    data = s.get(prj_url).text
    soup = BeautifulSoup(data, "html5lib")
    #print soup.prettify()
    baseline = [(item["value"], item.text) for item in soup.find("select", {'id':'id_baseline'}).findAll('option')[0:10]]
    for k,v in enumerate(baseline):
        print "\t%d. Baseline : %s"%(k, v[1])
    id = raw_input("Input your baseline(default 0): ")
    try:
        id = int(id)
        if id < 0 or id > 10:
            id = 0
    except ValueError:
        id = 0

    print "[+] Baseline: ", baseline[id][1]
    data = []
    #data.append(("RUN_FORM-description", ))
    data.append(("RUN_FORM-architecture", 1))
    data.append(("RUN_FORM-architecture", 2))
    data.append(("RUN_FORM-storage_host", 1))
    data.append(("RUN_FORM-build_groups", 54))
    data.append(("RUN_FORM-bob_options", 4))
    data.append(("baseline", baseline[id][0]))
    #data.append(("baseline", 461))
    #data.append(("RUN_FORM-release_to_use", ))


    payload  = '&'.join(["%s=%s" %(action, dictionary) for action, dictionary in data])
    #print payload
    #payload="RUN_FORM-description=&RUN_FORM-architecture=1&RUN_FORM-architecture=2&RUN_FORM-storage_host=1&RUN_FORM-build_groups=54&RUN_FORM-bob_options=4&baseline=461&RUN_FORM-release_to_use="
    try:
        r = s.post(prj_url, data=payload, allow_redirects=False)
        #debugReq(r)
    except requests.exceptions.ConnectionError as e:
        print "[-] start_build error"
    if r.status_code == 302:
        print "[+] Start build"
        return r.headers['Location']
    else:
        print "[-] start_build status code error"
        sys.exit(0)


def main():
    parser = OptionParser(usage='%prog [options] [word]', description='bld')
    parser.add_option("-u", "--us",  default=False, action='store_true', help='build on bld.us.oracle.com')
    parser.add_option("-j", "--prj_name", type="string", help='project name')
    parser.add_option("-b", "--bugno", type="string", help='bugno as project changeset')
    parser.add_option("-w", "--ws_name", type="string", help='bugno as workspace name')
    parser.add_option("-p", "--ws_path", type="string", help='your workspace path')
    (options, args) = parser.parse_args()
    '''
    1. Create workspace
    2. Add your code path
    3. Set the build and start
    '''
    url = "http://bld.uk.oracle.com/myprojects/"
    login_url = 'http://bld.uk.oracle.com/login/?next=/myprojects/'
    if options.us:
        login_url = 'http://bld.us.oracle.com/login/'
        url = "http://bld.us.oracle.com/myprojects/"
    (user, passwd) = get_credentials('oracle')
    login_bld(user, passwd, login_url)
    prj_url = create_prj(url, options.bugno, options.prj_name)
    print "[+] prj_url: ", prj_url
    ws_url = create_ws(prj_url, options.ws_path, options.ws_name)
    print "[+] ws_url: ", ws_url
    start_build(ws_url)


if __name__ == "__main__":
    main()
