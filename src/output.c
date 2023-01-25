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

int link_node = 0;

/* TODO: This must me moved to rssshow.c or viceversa */
void output_post(char *title, char *author, char *link, char *category, char *pubDate, char *content)
{

	switch(cfg.o)
	{
	case O_URL:
		printf("%s%s\n", get_prefix_string(), link);
		break;
	case O_TXT:
		printf("* %s\n",title);
		if (category)
		printf(" | Category: %s\n", category);
		if (pubDate && !cfg.planet)
		printf(" | Date:     %s\n", pubDate);
		if (cfg.planet)
		printf(" | Author:   %s\n", author);
		printf(" | URL:      %s%s\n",get_prefix_string(),link);
		if (cfg.showdesc&&content)
			printf("    %s\n\n",content);
		break;
	case O_TAB:
		printf("%s\t%s\t%s%s\n",content,title,get_prefix_string(),link);
		break;
	case O_RSS:
		printf("<item>\n  <author>%s</author>\n  <title>%s</title>\n  <link>%s%s</link>\n", 
		author, title,get_prefix_string(),link);
		if (content&&cfg.showdesc) 
		{
			printf("  <content:encoding><![CDATA[%s\n ]]></content:encoding>\n", content);
		}
		printf("</item>\n");
		break;
	case O_MXML:
		printf("<post>\n");
		printf(" <title> %s\n",title);
		printf(" <category> %s\n",category);
		if (pubDate)
		printf(" <date> %s\n",pubDate);
		if (content && cfg.showdesc)
		printf(" <body>\n%s\n",content);
		printf("</post>\n\n");
		break;
	case O_XML2DOC:
		printf("<page>\n");
		printf("<bookmark name=\"%s\" node=\"%d\" />\n", title, link_node);
		printf("<t size=\"20\">%s</t>\n",title);
		if (pubDate)
		printf("<t align=\"right\" size=\"12\">%s</t>\n", pubDate);
		printf("<sep />\n");
		if (content && cfg.showdesc)
		printf("<t>%s</t>\n", content);
		printf("</page>\n");
		break;
	case O_HTML:
	case O_TML:
	default:
		if (cfg.template)
		{
			if(tmpl_prepost)
			printf("%s",tmpl_prepost);

			if (tmpl_preauthor)
			printf("%s%s%s",
			tmpl_preauthor,
			author,
			tmpl_postauthor);

			printf("%s", tmpl_prepost_title);
			printf("<a name=\"node%d\" href=\"%s%s\">%s</a><br />\n",
				link_node, get_prefix_string(), link, title);
			//print pubDate ??//
			printf("%s", tmpl_postpost_title);

			if (cfg.showdesc && content)
			{
				printf("%s",tmpl_prepost_body);
				printf("%s",content);
				printf("%s",tmpl_postpost_body);
			}

			if (is_resumed)
			{
				printf("%s",tmpl_preread_more);
				printf("%s",link);
				printf("%s",tmpl_postread_more);
			}
			printf("%s",tmpl_postpost);
		} else {
			printf("<a name=\"node%d\" href=\"%s%s\">%s</a>",
				link_node, get_prefix_string(), link, title);

			if (pubDate)
			{
				printf("(%s)",pubDate);
			}
			printf("<br />\n");

			if (cfg.showdesc && content)
			{
				printf(
				"<table><tr><td>&nbsp;</td><td width='100%%'>"
				"%s</td></tr></table>\n", content);
			}

			if (is_resumed)
			{
				printf("<a href=\"%s\">&gt;&gt; read more</a><br /><br />",link);
			}
		}
		break;
	}

	link_node++;
}
