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

#include "main.h"
#include "storage.h"

int posts_showed = 0;

int main(int argc, char **argv)
{
	int i;
	char *err = "reading configuration";
	FILE *fd;
	char *url;
	char txt[1024];

	rsslist_init();

	if ( parse_config(argc,argv) )
	{
		err = NULL;

		if (cfg.parse)
		{
			fd = fopen(cfg.parse,"r");
			if (!fd)
			{
				printf("Cannot open rss-url listing file: '%s'\n"
						" ==> Use -h flag for help.\n",cfg.parse);
				return 1;
			}

			while(!feof(fd))
			{
				txt[0]=0;
				txt[1023]=0;
				fgets(txt,1022,fd);

				if (txt[0]==0) break;

				switch(txt[0])
				{
				case '#':
				case '\r':
				case '\n':
					continue;
				}

				url = strstr(txt,"http://");

				if (!url)
				{
					url = strstr(txt,"/");
				}

				if (!url)
				{
					fprintf(stderr,"Invalid rss-url listing file: '%s'\n",cfg.parse);
					return 1;
				}

				/* XXX check bof ? */
				url[-1] = '\0';
				url[strlen(url)-1] = '\0';

				rsslist_add(url,txt);
			}
			fclose(fd);
		}

		switch(cfg.o)
		{
		case O_HTML:
			if (cfg.template)
			{
				printf("%s",tmpl_header);
			} else {
				printf( "<html>\n <head>\n   "
				"<title>rss2html (http://www.nopcode.org/?t=rss2html)"
				"</title>\n "
				"<link rel=\"stylesheet\" href=\"rss2html.css\" "
				"type=\"text/css\" /></head>\n\n<body>");
			}
			break;
		case O_RSS:
			printf("<?xml version=\"1.0\" encoding=\"ISO-8859-15\"?>\n");
			printf("<rss version=\"2.0\"\n");
			printf(" xmlns:content=\"http://purl.org/rss/1.0/modules/content/\"\n");
			printf(" xmlns:wfw=\"http://wellformedweb.org/CommentAPI/\"\n");
			printf(" xmlns:dc=\"http://purl.org/dc/elements/1.1/\">\n");
			printf("<channel>\n");
			break;
		case O_XML2DOC:
			printf("<?xml version=\"1.0\" encoding=\"ISO-8859-15\"?>\n");
			printf("<xmldoc>\n");
			printf("<default font=\"Times-Roman\" autonum=\"true\" trim=\"false\" />\n");
			printf("<info title=\"Documents in XML\" author=\"pancake\" />\n");
			break;
		}
		
		for(i=0;i<n_rss_list;i++)
		{
			/* TODO: honor post number limits here */
			rss_show(rss_list[i]->file,
				rss_list[i]->url,
				rss_list[i]->txt);
		}

		/* planet */
		if (cfg.planet)
		{
			storage_order_by_date();
		}

		switch(cfg.o)
		{
		case O_HTML:
			if (cfg.template)
			{
				printf("%s",tmpl_footer);
			} else {
				printf("</body></html>\n");
			}
			break;
		case O_RSS:
			printf("</channel></rss>\n");
			break;
		case O_XML2DOC:
			printf("</xmldoc>\n");
			break;
		}
	} 

	/* END */
	rsslist_free();

	if (err)
	{
		printf("ERROR: '%s'\n",err);
		return 1;
	}

	return 0;
}
