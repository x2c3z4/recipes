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

COOKIE_FILE='.cookies'
LOGIN_URL = 'https://login.oracle.com/oam/server/sso/auth_cred_submit'
WIFI_URL_JAPAC = 'https://gmp.oracle.com/captcha/files/airespace_pwd_apac.txt?_dc=1426063232433'
WIFI_URL_AMERICAS = 'https://gmp.oracle.com/captcha/files/airespace_pwd.txt?_dc=1428891906138'
WIFI_URL_EMEA = 'https://gmp.oracle.com/captcha/files/airespace_pwd_emea.txt?_dc=1428891953219'

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
  print >>sys.stderr, '[+] load cookies!!!'

def patch_send():
    old_send= httplib.HTTPConnection.send
    def new_send( self, data ):
        print >>sys.stderr, '>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>'
        print >>sys.stderr, data
        print >>sys.stderr, '<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<'
        return old_send(self, data) #return is not necessary, but never hurts, in case the library is changed
    httplib.HTTPConnection.send= new_send

# def patch_getresponse():
#     old_getresponse= httplib.HTTPConnection.getresponse
#     def new_getresponse( self, buffering=False):
#         data = old_getresponse(self, buffering) #return is not necessary, but never hurts, in case the library is changed
#         print data
#         return data
#     httplib.HTTPConnection.getresponse= new_getresponse
# patch_getresponse()

patch_send()


def get_credentials(mach):
  # default is $HOME/.netrc
  netrc_machine = mach
  info = netrc.netrc()
  (login, account, password) = info.authenticators(netrc_machine)
  return (login,password)

def debugReq(r):
  pp = pprint.PrettyPrinter(indent=4)
  pp.pprint(r.status_code)
  # pp.pprint(r.request.__dict__)
  # print >>sys.stderr, r.text
  print >>sys.stderr, s.cookies.get_dict()

def login(user, passwd):
  payload = {
  'v':'v1.4',
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
    print "login error"

def extract(content):
  return '\n'.join(content.split('\n')[1:]) 

def main():
  #if not os.path.exists(COOKIE_FILE):
  if True:
    (user, passwd) = get_credentials('oracle')
    login(user, passwd)
    saveCookies()
  else:
    loadCookies()

  try:
    r = s.get(WIFI_URL_JAPAC)
    #debugReq(r)
    print >>sys.stdout, "JAPAC:\n%s"%(extract(r.text),)
    r = s.get(WIFI_URL_AMERICAS)
    print >>sys.stdout, "Americas:\n%s"%(extract(r.text),)
    r = s.get(WIFI_URL_EMEA)
    print >>sys.stdout, "EMEA:\n%s"%(extract(r.text),)
  except requests.exceptions.ConnectionError as e:
    print "Get WiFi error"

if __name__ == "__main__":
  main()
