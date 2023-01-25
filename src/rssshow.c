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

#include <unistd.h>
#include "main.h"

void rss_show(char *file, char *url,char *txt)
{
	FILE *fd;
	char *buf;
	long size;
	static int count = 0;

	if (!memcmp(":#:",file,3))
	{
		fprintf(stderr,"Internal error: (%s) %s\n\n", url, file+3);
		free(file);
		return;
	}

	fd = fopen(file,"r");
	if (!fd)
	{
		fprintf(stderr,"Cannot Open File : '%s'.\n", file);
		return;
	}

	fseek(fd,0,SEEK_END);
	size = ftell(fd);
	fseek(fd,0,SEEK_SET);
	
	if (size>MAXFILESIZE)
	{
		fprintf(stderr,"File %s too big.<br>\n",file);
		return ;
	}

	buf = (char *)malloc(size+1);
	fread(buf,size,1,fd);
	buf[size] = 0;
	fclose(fd);

	/* parse buffer */
	if (!cfg.join)
	{
		switch(cfg.o)
		{
		case O_TXT:
			printf("--[ %s ]--[ %s ]--\n",txt,file);
			break;

		case O_RSS:
			if (count++) break;
			if (cfg.planet)
			{
				txt = cfg.planet_name;
				url = cfg.planet_url;
			}
			printf("  <title>%s</title>\n",txt);
			printf("  <link>%s</link>\n",url);
			if (cfg.lang)
			printf("  <language>%s</language>\n",cfg.lang);
			if (cfg.desc)
			printf("  <description>%s</description>\n",cfg.desc);
			if (cfg.pict)
			printf("  <image><url>%s</url></image>\n",cfg.pict);
			break;

		case O_TAB:
		case O_MXML:
			break;

		case O_HTML:
			if (cfg.template)
			{
				if (cfg.desc)
				{
					if (tmpl_pretitle)
					printf("%s",tmpl_pretitle);
					printf("%s",(cfg.desc)?cfg.desc:txt);
					printf("%s",txt);
					if (tmpl_posttitle)
					printf("%s",tmpl_posttitle);
				}
			} else {
				printf("<h2><b>:: <a href=\"%s\">%s</a></b></h2>\n",
						url,(cfg.desc)?cfg.desc:txt);
			}
			break;
		}
	}
	
	parse_buffer(url,txt,buf);
	free(buf);

	/* only remove if it's from network */
	if (strstr(url,"://"))
	if (!cfg.keep)
		unlink(file);
}
