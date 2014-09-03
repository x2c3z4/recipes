#!/usr/bin/env python

def main():
  readFromFile("test.txt")
  return

glo = [126,63,109,121,51,91,95,112,127,123]


def readFromFile(path):
  with open(path) as f:
    content = f.readlines()
  num = int(content[0])
  glo.reverse()
  for it in range(num):
    print 
    print 
    print 
    segments = []
    line = content[1+it]
    item = line.split()
    count = int(item[0])
    for i in item[1:]:
      segments.append(int(i,2))
    for x in range(len(glo) - count):
      sel = glo[x:x+count]
      ret = []
      for y in range(count):
        ret.append(segments[y] ^ sel[y])
      print ret

if __name__ == "__main__":
  main()
