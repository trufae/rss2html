/*
 * Copyright (C) 2005, 2006, 2024
 *       pancake <pancake@nopcode.org>
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

#include "main.h"
#include "storage.h"

extern int posts_showed;
		
void strtrim(char *txt)
{
	register int i;
	for(i=strlen(txt)-1;i&&(txt[i]==' '||txt[i]=='\t'||txt[i]=='\n');i--);
	if (i!=strlen(txt-1)) txt[i+1]=0;
	for(i=0;txt[i]&&(txt[i]==' '||txt[i]=='\n'||txt[i]=='\t');i++);
	if (txt[i])	strcpy(txt,txt+i);
}

char* get_prefix_string()
{
	if (cfg.prefix)
		return cfg.prefix;
	return "";
}

int matches_grep(char *b,char *b2)
{
	char *ptr = cfg.grep;
	char *comma;

	if (cfg.grep == false )
	{
		return true;
	}

	while( true )
	{
		comma = strchr(ptr,',');
		if (comma) comma[0] = 0;

		if ( strstr(b,ptr) || (b2&&strstr(b2,ptr)) )
		{
			return true; // string found!
		}

		if (comma == 0) break;
		comma[0]=',';
		ptr = comma+1;
	}

	return false;
}

bool is_rss(char *buf)
{
	char *b;

	/* TODO more checks? */
	b = strstr(buf,"<rdf:RDF xmlns=");	
	if (b == NULL)
	b = strstr(buf,"<rdf");
	if (b == NULL)
	b = strstr(buf,"<rss");

	return b != NULL;
}

void get_enclosure_url(char *buf, char **buffer)
{
	char *b, *e;

	*buffer = NULL;

	b = strstr(buf,"<enclosure url=\"");
	if (b == NULL) return;

	b = b + 16;
	e = strstr(b,"\"");
	if (e == NULL)
	{
		fprintf(stderr,"error: unterminated url string in <enclosure>\n");
		return;
	}
	e[0]='\0';
	*buffer = strdup(b);
	e[0]='\"';
}

void get_tag_info(char *buf, char *str, char *limit, char **buffer)
{
	char *b,*e;
	char tmp;
	bool cdata = false;
	char *tag;
	char *ptr;
	char *e2 = NULL;

	*buffer = NULL;

	tag = (char *)malloc(strlen(str)+4); // 4=<+/+>+0
	if (strchr (str, ':')) {
		sprintf(tag,"<%s ",str);
	} else {
		sprintf(tag,"<%s>",str);
	}
	
	b = strstr(buf,tag);
	if (b == NULL) {
		return; /* tag not found */
	}
	if (limit != 0)
	if (b > limit) return; /* limit reached */

	b = b + strlen(tag);
	if (! memcmp(b,"<![CDATA[",8) ) { b = b + 9; cdata=true;} // CDATA
	sprintf(tag,"</%s>",str);
	e = strstr(buf,tag);
	if (e == NULL) {
		sprintf(tag,"/>");
		e = strstr(buf,tag);
		if (e == NULL) {
			return; /* end tag not found */
		}
	}
	e[0]='\0';
	tmp = '<';

	if (cdata)
	{
		ptr = strstr(b,"]]>");
		if (ptr) { e[0]='<'; e = ptr; e[0]='\0'; }
	} else {
		b = htmlize_string(b);
	}

	/* simple fixup : must be rewritten in the future. */
	switch(cfg.o)
	{
	case O_XML2DOC:
	case O_MXML:
		for(ptr=b;*ptr;ptr++)
		{
			switch(*ptr)
			{
			case '<': *ptr='['; break;
			case '>': *ptr=']'; break;
			}
		}
		break;
	}
	if (strchr (str, ':')) {
		char *url = strstr (b, "url=\"");
		if (url) {
			b = url + 5;
			e2 = strchr (b, '"');
			if (e2) {
				*e2 = 0;
			}
		}
	}

	*buffer = strdup(b);
	e[0] = tmp;
	if (e2) {
		e2[0] = '"';
	}

	free (tag);
}

char *find_next_post(char *buf)
{
	char *ptr;

	ptr = strstr(buf,"<item>");

	/* RDF tag */
	if (ptr == NULL)
	{
		ptr = strstr(buf,"<item ");
	}

	if (ptr != NULL)
	{
		ptr = ptr + 6;
	}

	return ptr;
}

char *find_endof_post(char *buf)
{
// TODO
	return NULL;
}

void fetch_global_info(char *buf)
{
	char *title;
	char *link;
	char *description;
	char *limit = find_next_post(buf);

	if (limit == NULL)
	{
		return;
	}

	get_tag_info(buf,"title",limit,&title); // blog's title
	get_tag_info(buf,"link",limit,&link); // blogs url
	get_tag_info(buf,"description",limit,&description); // blog description
}

void parse_buffer(char *url, char *txt, char *buf)
{
	int n;
	char *title;
	char *link;
	char *author;
	char *category;
	char *pubDate;
	char *media = NULL;
	char *content;
	char *enclosure;

	if (! is_rss(buf) )
	{
		fprintf(stderr,"This is not an RSS/XML file. (%s)\n",txt);
		return;
	}

	/* adds this blog to the blogs list */
	storage_add_blog(txt, url);

	fetch_global_info(buf);

	n = 0; /* counter of posts */
	while(( buf = find_next_post(buf) ))
	{
		if (cfg.Limit && (posts_showed == cfg.Limit)) break;
		if (cfg.limit && (n == cfg.limit)) break;

		get_tag_info(buf,"title",NULL,&title);
		get_tag_info(buf,"author",NULL,&author);
		if (author == NULL) {
			get_tag_info(buf,"dc:creator",NULL,&author);
		}
		get_tag_info(buf,"link",NULL,&link);
		get_tag_info(buf,"category",NULL,&category);
		get_tag_info(buf,"pubDate",NULL,&pubDate);
		get_tag_info(buf,"media:content",NULL,&media);
		if (pubDate == NULL)
		{
			// <dc:date>2006-01-23T01:43:51Z</dc:date>
			/* check RDF date format */
			get_tag_info(buf,"dc:date",NULL,&pubDate);
			if (pubDate)
			{
				char *tmp = (char *)rdf_to_rfc2822(pubDate);
				free(pubDate);
				pubDate = tmp;
			}
		}
		get_enclosure_url(buf,&enclosure);
		get_tag_info(buf,"content:encoded",NULL,&content);
		if (content == NULL)
		{
			get_tag_info(buf,"description",NULL,&content);
		}

		if (enclosure != NULL)
		{
			/* Tricky hack */
			link = enclosure;
		}

		if (matches_grep(title,content)) // title, descr
		{
			do_resume( content );
			if (!cfg.planet)
			output_post(title,author,link,category,pubDate,content, media);
		}

		/* Storage it */
		storage_add_post();
		storage_set("title",title);
		storage_set("link",link);
		storage_set("author",author);
		storage_set("category",category);
		storage_set("pubDate",pubDate);
		storage_set("media",media);
		storage_set("content",content);

		free(title);
		free(link);
		free(author);
		free(category);
		free(media);
		free(pubDate);
		free(content);

		posts_showed++;
		n++;
	}
}
