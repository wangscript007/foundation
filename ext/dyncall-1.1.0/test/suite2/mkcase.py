#!/usr/bin/python
#//////////////////////////////////////////////////////////////////////////////
#
# Copyright (c) 2007,2009 Daniel Adler <dadler@uni-goettingen.de>, 
#                         Tassilo Philipp <tphilipp@potion-studios.com>
#
# Permission to use, copy, modify, and distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
#//////////////////////////////////////////////////////////////////////////////


import sys

f = file("design.cfg")

sigmap = { 
  'B':'DCbool'
, 'c':'DCchar'
, 's':'DCshort'
, 'i':'DCint'
, 'l':'DClonglong'
, 'f':'DCfloat'
, 'd':'DCdouble'
, 'p':'DCpointer'
}

apimap = {
  '_':''
, 's':'__declspec(stdcall)'
, 'f':'__declspec(fastcall)'
}

id = 0
maxargs = 0
sys.stdout.write("/* auto generated by mkcase.py (on stdout) */\n");
for line in f: 
  line = line.rstrip().lstrip()
  if len(line) == 0 or line[0] == '#': continue
  types = [];
  # args  = len(line)-1
  args = len(line)
  maxargs = max(maxargs, args)
  # api = apimap[ line[0] ]
  out = [ "VF",str(args),"(", str(id), "," ];
  for i in xrange(0,len(line)):   
    types += [ sigmap[ line[i] ] ]
  out += [ ",".join( types ), ",s_", line, ")\n" ]
  out = "".join(out)
  sys.stdout.write(out)
  id  += 1

sys.stderr.write("/* auto generated by mkcase (on stderr) */\n");
sys.stderr.write("".join( ["#define NCASES ",str(id),"\n"] ) )
sys.stderr.write("".join( ["#define MAXARGS ",str(maxargs),"\n"] ) )



