/*
 * Copyright (C) 2006
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

#include "main.h"

void get_template_block(char *buf, char *tag, char **set)
{
	char *b, *e;
	char *searchstring;

	searchstring = (char *)malloc(strlen(tag)+100);
	if (!searchstring) return;

	sprintf(searchstring,"<!-- (%s) -->", tag);
	b = strstr(buf,searchstring);
	if (b == NULL) goto _get_template_end;
	b = b + strlen(searchstring) + 1;

	sprintf(searchstring,"<!-- (/%s) -->", tag);
	e = strstr(b,searchstring);
	if (e == NULL) goto _get_template_end;

	e[0]='\0';
	*set = strdup(b);
	e[0]='<';

	_get_template_end:
	free(searchstring);
}

char *tmpl_header = NULL;
char *tmpl_footer = NULL;
char *tmpl_pretitle = NULL;
char *tmpl_posttitle = NULL;
char *tmpl_prepost = NULL;
char *tmpl_prepost_title;
char *tmpl_postpost_title;
char *tmpl_prepost_body;
char *tmpl_postpost_body;
char *tmpl_preread_more;
char *tmpl_postread_more;
char *tmpl_postpost;
char *tmpl_preauthor;
char *tmpl_postauthor;

bool parse_template(char *file)
{
	FILE *fd;
	char *buf;
	long size;

	if (file==NULL) return false;

	fd = fopen(file,"r");
	if (!fd) return false;
	
	fseek(fd,0,SEEK_END);
	size = ftell(fd);
	fseek(fd,0,SEEK_SET);

	if (size>MAXFILESIZE) {
		fprintf(stderr,"File %s too big.<br>\n",file);
		return false;
	}
	buf = (char *)malloc(size+10);
	fread(buf,size,1,fd);

	get_template_block(buf, "header",&tmpl_header);
	get_template_block(buf, "footer",&tmpl_footer);
	get_template_block(buf, "title-pre",&tmpl_pretitle);
	get_template_block(buf, "title-post",&tmpl_posttitle);
	get_template_block(buf, "entry-pre",&tmpl_prepost);
	get_template_block(buf, "entry-post",&tmpl_postpost);
	get_template_block(buf, "entry-title-pre",&tmpl_prepost_title);
	get_template_block(buf, "entry-title-post",&tmpl_postpost_title);
	get_template_block(buf, "entry-author-pre",&tmpl_preauthor);
	get_template_block(buf, "entry-author-post",&tmpl_postauthor);
	get_template_block(buf, "entry-body-pre",&tmpl_prepost_body);
	get_template_block(buf, "entry-body-post",&tmpl_postpost_body);
	get_template_block(buf, "entry-readmore-pre",&tmpl_preread_more);
	get_template_block(buf, "entry-readmore-post",&tmpl_postread_more);

	free(buf);
	fclose(fd);

	return true;
}
