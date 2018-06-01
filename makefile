# Makefile för Intra version 2.1.x
# %W% %D%
#
intra: category subject title inbox subscribe article document comment message link about profile menu publication image audio video group
#
subject: subject.o
	cc subject.o ../utils.o ../cgi.o  intra.o db.o html.o -o subject.cgi
#
category: category.o
	cc category.o ../utils.o ../cgi.o intra.o db.o html.o -o category.cgi
#
inbox: inbox.o
	cc inbox.o ../utils.o ../cgi.o intra.o db.o html.o -o inbox.cgi
#
title: title.o  
	cc title.o  ../utils.o ../cgi.o intra.o db.o html.o -o title.cgi
#
subscribe: subscribe.o  
	cc subscribe.o  ../utils.o ../cgi.o intra.o db.o html.o -o subscribe.cgi

#
article: article.o
	cc article.o ../utils.o ../cgi.o intra.o db.o html.o -o article.cgi
#
comment: comment.o
	cc comment.o  ../utils.o ../cgi.o intra.o db.o html.o -o comment.cgi
#
message: message.o
	cc message.o  ../utils.o ../cgi.o intra.o db.o html.o -o message.cgi
#
document: document.o
	cc document.o  ../utils.o ../cgi.o intra.o db.o html.o -o document.cgi
#
publication: publication.o 
	cc publication.o ../utils.o ../cgi.o intra.o db.o html.o -o publication.cgi
#
audio: audio.o
	cc audio.o ../utils.o ../cgi.o intra.o db.o html.o -o audio.cgi
#
image: image.o
	cc image.o ../utils.o ../cgi.o intra.o db.o html.o -o image.cgi
#
video: video.o
	cc video.o ../utils.o ../cgi.o intra.o db.o html.o -o video.cgi
#
rss: rss.o
	cc rss.o ../utils.o ../cgi.o intra.o db.o html.o -o rss
#
group: group.o
	cc group.o ../utils.o ../cgi.o intra.o db.o html.o -o group.cgi
#
search: search.o
	cc search.o ../utils.o ../cgi.o intra.o db.o html.o -o search.cgi
#
about: about.o
	cc about.o  ../utils.o ../cgi.o intra.o db.o html.o -o about.cgi
#
profile: profile.o
	cc profile.o  ../utils.o ../cgi.o intra.o db.o html.o -o profile.cgi
#
link: link.o
	cc link.o ../utils.o ../cgi.o  intra.o db.o html.o -o link.cgi
##
menu: menu.o
	cc menu.o ../utils.o ../cgi.o intra.o db.o html.o -o menu.cgi
#
#
# 
#
# Compile source code
# 
#

#
intra.o: intra.c intra.h
	cc -c intra.c
#
db.o: db.c db.h
	cc -c db.c
#
html.o: html.c html.h
	cc -c html.c

#




subject.o: subject.c  ../cgi.c
	cc -c subject.c
#
menu.o: menu.c  ../cgi.c
	cc -c menu.c
#
title.o: title.c  ../cgi.c
	cc -c title.c
#
article.o: article.c ../cgi.c 
	cc -c article.c
# 
about.o: about.c ../cgi.c 
	cc -c about.c
#
inbox.o: inbox.c ../cgi.c
	cc -c inbox.c
#
category.o: category.c  ../cgi.c
	cc -c category.c
#
comment.o: comment.c  ../cgi.c
	cc -c comment.c
#
message.o: message.c  ../cgi.c
	cc -c message.c
#
document.o: document.c ../cgi.c
	cc -c document.c
#
profile.o: profile.c ../cgi.c
	cc -c profile.c
#
subscribe.o: subscribe.c ../cgi.c
	cc -c subscribe.c
#
link.o: link.c  ../cgi.c
	cc -c link.c

publication.o: publication.c ../cgi.c
	cc -c publication.c
#
image.o: image.c ../cgi.c
	cc -c image.c
#
audio.o: audio.c ../cgi.c
	cc -c audio.c
#
video.o: video.c ../cgi.c
	cc -c video.c
#
#
rss.o: rss.c ../cgi.c
	cc -c rss.c
#
install:
	chmod 755 *.cgi
	cp *.cgi /usr/local/apache/cgi-bin/intra

#
#
clean:
	rm *.o *.bak *~



