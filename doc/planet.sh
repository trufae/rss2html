#!/bin/sh
#
# Sample planet crontab script

PLANET_OPTS="-r 150 -P nopcode@blog -d -n 2"
PLANET_TEMPLATE="/etc/rss2html.template"
BLOGS="
Marc@http://marc.noctambuls.net/?feed=rss2
pancake@http://news.nopcode.org/pancake/rss2.xml
Brainstorm@http://blogs.nopcode.org/brainstorm/feed/
pof@http://pof.eslack.org/blog/feed/
plue@http://plue.uni.cc/feed/
TripleDES@http://midgard.bounceme.net/wp/?feed=rss2
esteve@http://esteve.tizos.net/feed/
xtraeme@http://plog.xtrarom.org/rss.php?blogId=1&amp;profile=rss20
MiKi@http://mikiHQ.com/?feed=rss2
"

rss2html -o html ${PLANET_OPTS} -T ${PLANET_TEMPLATE} ${BLOGS} > /var/www/blogs.nopcode.org/index.html
rss2html -o rss ${PLANET_OPTS} -T ${PLANET_TEMPLATE} ${BLOGS} > /var/www/blogs.nopcode.org/rss2.xml
