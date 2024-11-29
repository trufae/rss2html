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

configuration cfg;

void
show_usage(prg)
	char *prg;
{
	printf( "Usage: %s [options] [url]    # version %s\n"
			" -c [file]      :: Use 'file' instead of ~/.rss2html.\n"
			" -d             :: Show the description text.\n"
			" -D [descr]     :: Set the description string.\n"
			" -e [wrd,wrd]   :: Grep comma separated words.\n"
			" -h             :: Shows help.\n"
			" -i             :: Ignore images.\n"
			" -j             :: Join bodies.\n"
			" -k             :: Keep temporally files.\n"
			" -T [template]  :: use this template for html output.\n"
			" -n [number]    :: Number of posts to read from each RSS.\n"
			" -N [number]    :: Total number of posts to be showed.\n"
			" -r [n_words]   :: Resume post body into n words.\n"
			" -l [lang]      :: Set language.\n"
			" -L [directory] :: Translate remote links to local directories.\n"
			" -o [format]    :: Select [txt|tml|html|tab|rss|mxml|xml2doc|url].\n"
			" -P [name@url]  :: Create a planet. (order by date, not by blog).\n"
			" -p [picture]   :: Set the picture.\n"
			" -t [tmpdir]    :: Set temporally directory.\n", prg,VERSION);
	exit(1);
}
	
int
parse_config(argc,argv)
	int argc;
	char **argv;
{
	int i;
	char *home   = getenv("HOME");
	char *tmpdir = getenv("TMPDIR");

	if (home)
	{
		cfg.parse=(char *)malloc(strlen(home)+20);
		sprintf(cfg.parse,"%s/.rss2html.links",home);
	}
	else
	{
		cfg.parse = 0;
	}

	if (tmpdir) cfg.tmpdir = strdup(tmpdir);
	else cfg.tmpdir = strdup("/tmp");

	cfg.o        = O_HTML;
	cfg.join     = 0;
	cfg.keep     = 0;
	cfg.grep     = 0;
	cfg.images   = 0;
	cfg.desc     = 0;
	cfg.pict     = 0;
	cfg.lang     = 0;
	cfg.showdesc = 0;
	cfg.template = NULL;
	cfg.planet   = 0;
	cfg.planet_url = NULL;
	cfg.planet_name = NULL;
	cfg.prefix   = 0;
	cfg.limit    = 0;
	cfg.Limit    = 0;

	for(i=1;i<argc;i++)
	{
		if (argv[i][0]!='-')
		{
			rsslist_add(argv[i],"cmdline");

			free(cfg.parse);

			cfg.parse = 0;

		} else
		if (!strcmp("-n",argv[i]))
		{
			i++;
			if (i<argc)
			{
				cfg.limit = atoi(argv[i]);
			} else show_usage(argv[0]);
		} else
		if (!strcmp("-N",argv[i]))
		{
			i++;
			if (i<argc)
			{
				cfg.Limit = atoi(argv[i]);
			} else show_usage(argv[0]);
		} else
		if (!strcmp("-r",argv[i]))
		{
			i++;
			if (i<argc)
			{
				cfg.resume = atoi(argv[i]);
			} else show_usage(argv[0]);
		} else
		if (!strcmp("-h",argv[i]))
		{
			show_usage(argv[0]);
			return 0;
		} else
		if (!strcmp("-d",argv[i]))
		{
			cfg.showdesc=1;
		} else
		if (!strcmp("-P",argv[i]))
		{
			cfg.planet = 1;
			i++;
			if (i<argc)
			{
				char *ptr = argv[i];
				ptr = strchr(ptr,'@');
				if (ptr == NULL)
				{
					fprintf(stderr,"error: Planet name must be name@url.\n");
					return 0;
				}
				ptr[0]='\0';
				cfg.planet_name = argv[i];
				cfg.planet_url = ptr + 1;
			} else show_usage(argv[0]);
		} else
		if (!strcmp("-D",argv[i]))
		{
			i++;
			if (i<argc)
			{
				cfg.desc=strdup(argv[i]);
			} else show_usage(argv[0]);
		} else
		if (!strcmp("-e",argv[i]))
		{
			i++;
			if (i<argc)
			{
				cfg.grep=strdup(argv[i]);
			} else show_usage(argv[0]);
		} else
		if (!strcmp("-l",argv[i]))
		{
			i++;
			if (i<argc)
			{
				cfg.lang=strdup(argv[i]);
			} else show_usage(argv[0]);
		} else
		if (!strcmp("-L",argv[i]))
		{
			i++;
			if (i<argc)
			{
				cfg.prefix=(char *)malloc(strlen(argv[i])+4);

				cfg.prefix[0]='\0';

				if (argv[i][0]!='/')
					strcpy(cfg.prefix,"./");

				strcat(cfg.prefix,argv[i]);

				if ((argv[i][strlen(argv[i])-1]!='/'))
					strcat(cfg.prefix,"/");

			} else show_usage(argv[0]);
		} else
		if (!strcmp("-p",argv[i]))
		{
			i++;
			if (i<argc)
			{
				cfg.pict=strdup(argv[i]);
			} else show_usage(argv[0]);
		} else
		if (!strcmp("-j",argv[i]))
		{
			cfg.join=1;
		} else
		if (!strcmp("-k",argv[i]))
		{
			cfg.keep=1;
		} else
		if (!strcmp("-i",argv[i]))
		{
			cfg.images=0;
		} else
		if (!strcmp("-t",argv[i]))
		{
			i++;
			if (i<argc)
			{
				free(cfg.tmpdir);
				cfg.tmpdir=strdup(argv[i]);
			}else{
				show_usage(argv[0]);
				return 0;
			}
		} else
		if (!strcmp("-T",argv[i]))
		{
			i++;
			if (i<argc)
			{
				free(cfg.template);
				cfg.template=strdup(argv[i]);
			} else {
				show_usage(argv[0]);
				return 0;
			}
		} else
		if (!strcmp("-o",argv[i]))
		{
			i++;
			if (i<argc)
			{
				if (!strcmp("txt",argv[i]))
					cfg.o=O_TXT;
				else if (!strcmp("mxml",argv[i]))
					cfg.o=O_MXML;
				else if (!strcmp("gem",argv[i]))
					cfg.o=O_GEM;
				else if (!strcmp("sh",argv[i]))
					cfg.o=O_SH;
				else if (!strcmp("html",argv[i]))
					cfg.o=O_HTML;
				else if (!strcmp("tml",argv[i]))
					cfg.o=O_TML;
				else if (!strcmp("tab",argv[i]))
					cfg.o=O_TAB;
				else if (!strcmp("xml2doc",argv[i]))
					cfg.o=O_XML2DOC;
				else if (!strcmp("rss",argv[i]))
					cfg.o=O_RSS;
				else if (!strcmp("url",argv[i]))
					cfg.o=O_URL;
				else 
					show_usage(argv[0]);
			}
			else
			{
					show_usage(argv[0]);
			}
		} else
		if (!strcmp("-c",argv[i]))
		{
			i++;
			if (i<argc)
			{
				cfg.parse=strdup(argv[i]);
			} else
				show_usage(argv[0]);
		}  else {
				show_usage(argv[0]);
				return 0;
		}
	}

	parse_template(cfg.template);
	
	return 1;
}
