- [About](#about)
- [Why](#why)
- [How you can get the svn mirror](#how-you-can-get-the-svn-mirror)


# About
Unofficial mirror of SVN trunk of VICE - the Versatile Commodore emulator.
Provide version tags and complete svn history dump.

Please refer to https://github.com/VICE-Team/svn-mirror for the official sync and to https://svn.code.sf.net/p/vice-emu/code/ for the subversion "gold master"


# Why 
Beacuse I need a mirror of git svn to make some tiny hacks


# How you can get the svn mirror

Execute:

  git svn init -t tags -b branches -T trunk https://svn.code.sf.net/p/vice-emu/code/
  git svn fetch
  git for-each-ref --format="%(refname:short) %(objectname)" refs/remotes/origin/tags | grep /v | grep -v viceplus |  cut -d / -f 3-  |while read ref; do   echo git tag -a $ref -m 'import tag from svn'; done

Total repository is quite big.

Reference: 
https://stackoverflow.com/questions/2244252/how-to-import-svn-branches-and-tags-into-git-svn
