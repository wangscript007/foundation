/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_thunk_arm32.h
 Description: Thunk - Header for ARM32 (ARM and THUMB mode)
 License:

   Copyright (c) 2007-2018 Daniel Adler <dadler@uni-goettingen.de>,
                           Tassilo Philipp <tphilipp@potion-studios.com>

   Permission to use, copy, modify, and distribute this software for any
   purpose with or without fee is hereby granted, provided that the above
   copyright notice and this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

*/


#ifndef DYNCALL_THUNK_ARM32_H
#define DYNCALL_THUNK_ARM32_H


struct DCThunk_
{
  unsigned int code[2];
  void       (*entry)();
};

#define DCTHUNK_ARM32_SIZE 12


#endif /* DYNCALL_THUNK_ARM32_H */

