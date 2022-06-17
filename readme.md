# Neon Runtime
General toolkit for C applications can be compiled as a dynamic library or static library.

I have developed this library primarily for my own purposes, as such the features it offers,
may or may not suit your needs, you may use it under the terms of a BSD-3-Clause license,
a copy of which can be found in [COPYING](./COPYING).

## Requirements to building this code
The library has a very small number of requirements and should build under most
UNIX like operating systems, I have tested it so far on Linux and FreeBSD.

> Note on windows:

### Submodules
From a fresh clone if you didn't do a recursive clone:

Init all submodules and update them.
```shell
git submodule init
git submodule update
```

### Not in tree
These are the requirements that are not included in the vendor tree:
* jemalloc (if -DUSE_JEMALLOC is set)

# Licence, warranty and support information
Bellow is reproduced the contents of [COPYING](./COPYING).

For more information, or support options contact me at mxavier[AT]my username[DOT]com
```
BSD 3-Clause License

Copyright (c) 2022, Matheus Xavier
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```
