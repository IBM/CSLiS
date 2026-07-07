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

This version of CSLiS will install in any kernel version from 4.x
onward (through 6.x).

The LiS software resides outside the kernel source tree.  It runs
as a loadable module.

Latest update, CSLiS-7.1.1.3-4.tgz, superseeds the May 21, 2026 release. Include APAR fix LI83323, SNA STOP HANGS. 
Includes the previous update that supports RHEL 10.2, RHEL 9.8 with changes to handle changed macro calls in Linux 
kernel applied in May, 2026. Check date by "grep -1 lis_date /usr/src/CSLiS-711/head/version.c"
and check date is at least July 6, 2026. The version will show a "LiS-CS71134" for this new update.

Jeff L Smith
<jefsmith@us.ibm.com>
