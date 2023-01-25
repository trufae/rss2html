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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int subst_char(char *string, char from, char to)
{
	char *ptr = string;
	int changes = 0;

	while(*ptr)
	{
		if ( (*ptr) == from )
		{
			*ptr = to;
			changes++;
		}
		ptr = ptr + 1;
	}

	return changes;
}

char *subst_string(char *org, char *from, char *to)
{
	char *orig = strdup(org);
	char *ptr, *ptr2;
	int offset = strlen(from) - strlen(to);
	int i, times;

	for ( times=0, ptr = orig; ptr != NULL; times++ )
	{
		ptr = strstr(ptr, from);
		if (ptr) ptr = ptr + strlen(from);
	}

	ptr = (char *)malloc(strlen(orig) + (offset * times) + 1 );

	for(i=0;*orig;)
	{
		ptr2 = strstr(orig,from);
		if (ptr2)
		{
			ptr2[0]='\0';
			strcpy(ptr + i, orig);
			i += strlen(orig);
			strcpy(ptr + i, to);
			i += strlen(to);
			ptr2[0]=from[0]; // recover
			orig = ptr2 + strlen(from);
		} else {
			strcpy(ptr + i, orig);
			orig[0]=0;
			break;
		}
	}

	return ptr;
}

char *htmlize_string(char *str)
{
	char *ptr = str;
	char *ptr2 = NULL;

	ptr2 = subst_string( ptr, "&gt;", ">");
	ptr = subst_string( ptr2, "&lt;", "<");
	free(ptr2);
	ptr2 = subst_string( ptr, "&nbsp;", " ");
	free(ptr);
	/* XXX tricky hack to subst &amp -> & */
	ptr = subst_string( ptr2, "&amp;", "_");
	free(ptr2);
	ptr2 = subst_string( ptr, "_amp;", "&");
	free(ptr);
	ptr = subst_string( ptr2, "&quot;", "\"");

	return ptr;
}
