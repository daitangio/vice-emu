Unofficial mirror of SVN trunk of VICE - the Versatile Commodore emulator.
Provide version tags and complete svn history dump.

- [About](#about)
- [Why](#why)
- [Other resources](#other-resources)
- [How you can get a svn mirror like this if you like](#how-you-can-get-a-svn-mirror-like-this-if-you-like)


# About

Wellcome to the  unofficial mirror of SVN trunk of VICE - the Versatile Commodore emulator!
We take extra care to provide version tags and complete svn history dump on major releases.
A "sync branch" which does not contain this README is https://github.com/daitangio/vice-emu/tree/svn-sync
 and it is synced with SVN ["gold master"](https://svn.code.sf.net/p/vice-emu/code/)


# Why 
Because I need a mirror of git svn to study and possibly make tiny hacks.

# Other resources
Refer also to https://github.com/VICE-Team/svn-mirror for the official github sync.


# How you can get a svn mirror like this if you like

Execute:


   git svn init -t tags -b branches -T trunk https://svn.code.sf.net/p/vice-emu/code/
   git svn fetch
   git for-each-ref --format="%(refname:short) %(objectname)" refs/remotes/origin/tags | grep /v | grep -v viceplus |  cut -d / -f 3-  |while read ref; do   echo git tag -a $ref -m 'import tag from svn'; done


Total repository is quite big (1.4Gb) and require some times to be downloaded (one day with some connections retry).


References: 
https://stackoverflow.com/questions/2244252/how-to-import-svn-branches-and-tags-into-git-svn
