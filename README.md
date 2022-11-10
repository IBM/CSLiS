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

    tar -xzf CSLiS-7xx.tgz  ('x' is the specific CSLiS version extension)

It will make a subdirectory named CSLiS-7xx

Change directories to CSLiS-7xx and type in "./buildLiS".  The CSLiS package
will install itself.  After the buildLiS script completes, the CSLiS drivers,
libraries and utility programs will be in the proper places on your system.


KERNEL COMPATIBILITY

This version of CSLiS will install in any kernel version from 3.x
onward (through 5.x).

The LiS software resides outside the kernel source tree.  It runs
as a loadable module.

Jeff L Smith
<jefsmith@us.ibm.com>

