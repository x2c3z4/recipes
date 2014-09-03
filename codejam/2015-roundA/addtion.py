#!/usr/bin/env python

def main():
  readFromFile("test.txt")
  return

def readFromFile(path):
  with open(path) as f:
    content = f.readlines()
  num = int(content[0])
  index = 1
  for it in range(num):
    """
    A case
    """
    num_a = int(content[index])
    index = index + 1

    num_b = int(content[index + num_a])
    a = []
    for i in range(num_a):
      equation = content[index + i].strip()
      print equation
      item = equation.split("+")
      tmp = item[0:-1] +  item[-1].split("=")
      a.append(sorted(tmp))

    print a

    for i in range(num_b):
      guess = content[index + num_a + 1 + i].strip()
      print guess
      item = guess.split("+")
      print item
      print "Case #%d:\n"%(i,)

      (status, value) = is_fixed(a, item)
      if status:
        print "%s=%s\n"%(guess,value)

    index += num_a + num_b + 1

def filter_list(l):
  newl = []
  sum = 0
  for i in l:
    if i.isdigit():
      sum = sum + int(i)
    else:
      newl.append(i)
  newl.append(sum)
  return sorted(newl)

def exclude_list(hah, big):
  small = hah[:]
  print "small"
  print small
  print "big"
  print big
  for x in small[1:]:
    if x not in big:
      return False,None
  while len(small) > 1:
    for x in big:
      if x in small:
        big.remove(x);
        small.remove(x);
        break;
  big[0] = int(big[0]) - int(small[0])
  print "new"
  print big
  if len(big) > 1:
    return True,big
  else:
    return False,None


def is_fixed(eqs, gu):
  tmp = []
  small = []
  for x in gu:
    for e in eqs:
      selected = False
      if gu == e[1:]:
        return True,e[0]
    if x in e:
      tmp = tmp + e
      (status,value) = try_solve()
    else:
      selected = True
  if selected:
    small.append(e)
  print "==============="
  print small
  print tmp
  '''
  combine
  '''
  tmp1 = filter_list(tmp)
  '''
  remove know
  '''
  while True:
    fixed = False
    for e in small:
      (status, newl) = exclude_list(e, tmp1)
      if status:
        tmp1 = newl
        fixed = True
    if fixed == False:
      break
  print "exclude:"
  print tmp1
  return True,"XXXXXXXXXXXXXXXX"


if __name__ == "__main__":
  main()
