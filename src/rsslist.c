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

#include "main.h"

rsslist **rss_list;
int n_rss_list;

void rsslist_init()
{
	rss_list = (rsslist **)malloc(1);
	n_rss_list = 0;
}

void rsslist_add(char *url,char *txt)
{
	char *url2;

	rss_list = (rsslist **)realloc(rss_list,sizeof(rsslist*)*(n_rss_list+1));
	rss_list[n_rss_list] = (rsslist*)malloc(sizeof(rsslist));

	if (!strcmp(txt,"cmdline"))
	{
		url2 = strchr(url,'@');

		if (url2 != NULL)
		{
			txt = url;
			url = url2 + 1;
			url2[0] = 0;
		}
	}

	rss_list[n_rss_list]->url = strdup(url);

	if (!strncmp(url,"http://",7))
		rss_list[n_rss_list]->file = (char *)download(url);
	else
		rss_list[n_rss_list]->file = strdup(url);	

	rss_list[n_rss_list]->txt = strdup(txt);
	n_rss_list++;
}

void rsslist_free()
{
	register int i;

	for(i=0;i<n_rss_list;i++)
	{
		free(rss_list[i]->url);
	}

	free(rss_list);

	n_rss_list = 0;
}
