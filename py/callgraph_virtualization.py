#!/usr/bin/python
# powerpc-gekko-objdump -d example.elf | python cg.py | dot -Tpng > cg.png && eog cg.png

import re, sys

f_re = re.compile('^[0-9a-f]* <([a-zA-Z0-9_]*)>:$')
c_re = re.compile('\tbl *[0-9a-f]* <([a-zA-Z0-9_]*)>')


_blacklist = set(['sys_init', 'SYS_ResetSystem', 'puts', 'getButtons',
    'malloc', 'free', 'memalign', 'fflush', 'sd_mkdir', 'check_fatpath',
    'memset', 'memcpy', 'DCFlushRange', 'iosAlloc', 'iosFree',
    'DCInvalidateRange'])
_blacklist_pre = ['__', 'str', 'f_', 'VIDEO_']
_blacklist_cont = ['printf']
_blacklist_cont = ['printf']

def blacklist(s):
    for f in _blacklist_cont:
        if f in s: return True
    for f in _blacklist_pre:
        if s.startswith(f): return True
    if s in _blacklist: return True

cg = {}
for line in sys.stdin:
    line = line.strip()
    m = f_re.match(line)
    if m: s = cg[m.group(1)] = []

    m = c_re.search(line)
    if m and not blacklist(m.group(1)): s.append(m.group(1))

print "digraph g {"

l = ['main']
ls = set(l)
for i in l:
    print "%s;" % i
    for g in cg[i]:
        if g in ls: continue
        ls.add(g)
        l.append(g)

for i in ls:
    s = set()
    for j in cg[i]:
        if j not in s:
            s.add(j)
            print "%s -> %s;" % (i, j);
print "}"
