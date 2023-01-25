/*
 * Copyright (C) 2005, 2006
 *       pancake <pancake@phreaker.net>
 *
 * rss2html is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * rss2html is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with rss2html; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

/* TODO : SSL support */
#include "main.h"
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

int gen_key(const char *string)
{
	register int i, j;
	int len = strlen(string);
	int key = 0;

	for(i=0;i<len;i++)
	{
		if (j>sizeof(long)) j = 0;
		key += string[i]<<j;
		j++;
	}

	return key;
}

#if HAVE_LIB_CURL

#include <curl/curl.h>

FILE *tfd = NULL; // target filedescriptor

size_t  write_data(void  *buffer,  size_t  size,  size_t  nmemb,  void *userp)
{
	fwrite(buffer, size, nmemb, tfd);

	return size*nmemb;
}

char *do_get(const char *http, char *url, char *file)
{
	CURL *curl;
	CURLcode res;
	char tmpfile[1024];

	curl = curl_easy_init();
	if(curl) {
		sprintf(tmpfile, "%s/%s.%x", cfg.tmpdir, url, gen_key(file));

		tfd = fopen(tmpfile,"w");
		if (!tfd) {
			fprintf(stderr,"Cannot open temporally file '%s' for writting.\n", tmpfile);
			exit(1);
		}

		curl_easy_setopt(curl, CURLOPT_URL, http);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		res = curl_easy_perform(curl);

		curl_easy_cleanup(curl);
		fclose(tfd);

		return strdup(tmpfile);
	}

	return 0;
}

#else

char *do_get(char *url, char *file)
{
	struct hostent *he;
	struct sockaddr_in addr;
	int n,s, lenfile;
	char *get,tmpfile[1024];
	char buf[1025],*data=0;
	FILE *fd;

	he=(struct hostent*)gethostbyname(url);
	if (he==(struct hostent*)0)
		return strdup(":#:Cannot resolve hostname.");

	s = socket(AF_INET,SOCK_STREAM,0);
	addr.sin_family = AF_INET;
	memcpy( (char *)&addr.sin_addr, 
					he->h_addr,
				  	he->h_length);
	addr.sin_port = htons(80);
	if ( connect(s,(struct sockaddr *)&addr,sizeof(addr)) )
		return strdup(":#:Cannot connect to remote server.");

	lenfile = strlen(file);
	get = (char *)calloc(1,lenfile+1024);
	sprintf(get,
		"GET /%s HTTP/1.1\r\n"
		"Host: %s\r\n"
		"Transfer-encoding: quoted-printable\r\n"
		"User-Agent: rss2html %s\r\n"
		"Connection: close\r\n\r\n", file, url, VERSION);
	send(s,get,strlen(get),0);

	sprintf(tmpfile, "%s/%s.%x", cfg.tmpdir, url, gen_key(file));
	fd = fopen(tmpfile,"w");
	if (!fd)
	{
		fprintf(stderr,"Cannot open temporally file '%s' for writting.\n",tmpfile);
		exit(1);
	}

	while(1)
	{
		n = recv(s,buf,1024,0);
		if (n == 0) break;
		if (!data)
		{
			data=strchr(buf,'<');
			if (data)
			{
				buf[n]=0;
				strcpy(buf,data);
				if (data>buf) /* grow/not stacks */
					n-=(int)(data-buf);
				else
					n-=(int)(buf-data);
			}
		}
		if (n>0)
		if (data)
			fwrite(buf,n,1,fd);
	}
	fclose(fd);

	/* Shutdown Engines.. */
	shutdown(s,SHUT_RDWR);
	close(s);
	return strdup(tmpfile);
}
#endif

char *
download(str)
	const char *str;
{
	char *b,*p;

	b = strdup(str);
	if (!strncmp(str,"http://",7))
	{
		strcpy(b, b+7);
		p = strchr(b, '/');

		if (p) {
			p[0] = 0;
#if HAVE_LIB_CURL
			p = do_get(str, b, p+1);
#else
			p = do_get(b,p+1);
#endif
		} else {
			p = b;
		}
	}

	return p;
}
