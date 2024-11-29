/*
 * Copyright (C) 2005
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAXFILESIZE 512000

#define bool int
#define true 1
#define false 0

enum {
	O_TML,
	O_HTML,
	O_TXT,
	O_TAB,
	O_MXML,
	O_XML2DOC,
	O_RSS,
	O_URL,
	O_SH,
	O_GEM,
};

typedef struct {
	char *url;
	char *file;
	char *txt;
} rsslist;

typedef struct {
	char *template;
	char *tmpdir;
	char *parse;
	int planet;
	char *planet_name;
	char *planet_url;
	char *desc,*pict;
	char *lang,*grep;
	char *prefix;
	int limit;
	int Limit;
	int showdesc;
	int resume;
	int o,join,keep,images;
} configuration;

extern rsslist **rss_list;
extern int n_rss_list;
extern configuration cfg;
extern bool is_resumed;

extern char *tmpl_header;
extern char *tmpl_footer;
extern char *tmpl_pretitle;
extern char *tmpl_posttitle;
extern char *tmpl_prepost;
extern char *tmpl_prepost_title;
extern char *tmpl_postpost_title;
extern char *tmpl_prepost_body;
extern char *tmpl_postpost_body;
extern char *tmpl_preread_more;
extern char *tmpl_postread_more;
extern char *tmpl_postpost;
extern char *tmpl_preauthor;
extern char *tmpl_postauthor;;

extern bool is_resumed;
void output_post(char *title, char *author, char *link, char *category, char *pubDate, char *content, char *media);
void do_resume(char *);
char *htmlize_string(char *);
char *rdf_to_rfc2822(char *tstamp);

void rsslist_init();
void rsslist_add(char *url,char *txt);
void rsslist_free();

void parse_buffer(char *url, char *txt, char *buf);
bool parse_template(char *file);
char *download(const char *str);
int parse_config(int argc, char **argv);
void rss_show(char *file, char *url,char *txt);

char* get_prefix_string();
