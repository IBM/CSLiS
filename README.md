# CSLiS
                Communications Server Linux STREAMS Package

                        C S L i S

NOTE

This is a very abbreviated version of the CSLiS documenation.  The complete
documentation is now in html format.  It comes with the LiS distribution
in the directory htdocs.  It is available over the Internet at
http://github.com/IBM/CSLiS. This package is a forked project off the
Linux Streams package, LiS-2.19 level.

INSTALLATION

We recommend unpacking the tar archive in the directory /usr/src.
While logged in as root, use the following command:

    tar -xzf CSLiS-7.x.x.x.tgz  ('x' is the specific CSLiS version extension)

It will make a subdirectory named CSLiS-7xx

Change directories to CSLiS-7xx and type in "./buildLiS".  The CSLiS package
will install itself.  After the buildLiS script completes, the CSLiS drivers,
libraries and utility programs will be in the proper places on your system.


KERNEL COMPATIBILITY

This version of CSLiS will install in any kernel version from 3.x
onward (through 6.x).

The LiS software resides outside the kernel source tree.  It runs
as a loadable module.

Latest update has correction to Linux 6.8.0 Kernel support to support Ubuntu 24.04 LTS.
The SNA node would hand on restart of the node on Ubuntu 24.04 LTS distrobutions. Fix was made and 
the updates applied December 02, 2024. Check date by "grep -1 lis_date /usr/src/CSLiS-711/head/version.c"
and check date is at least 02 Dec 2024.

Jeff L Smith
<jefsmith@us.ibm.com>
