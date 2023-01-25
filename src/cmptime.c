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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#define __USE_BSD
#include <time.h>

char *weeks[]=
{ "Sun", "Mon","Tue","Wed","Thu","Fri","Sat",0 };
char *months[]=
{ "Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec",0 };

int word_to_number(char *array[],char *word)
{
	int i;
	for(i=0;array[i];i++)
	{
		if (!strcmp(word,array[i]))
		{
			return i;
		}
	}
	return -1;
}

time_t rfc2822_to_utc(char *date_string)
{
	struct tm t;
	time_t t2;
	char wday[100];
	char mon[100];
	char utc[100];

	memset((struct tm*)&t,0,sizeof(struct tm));
	
	sscanf(date_string,"%4s %d %4s %d %d:%d:%d %5s",
		wday,
		&t.tm_mday,
		mon,
		&t.tm_year, 
		&t.tm_hour,
		&t.tm_min,
		&t.tm_sec,
		utc);

	wday[3]    = 0;
	t.tm_year -= 1900;
	t.tm_mon   = word_to_number(months,mon);
	t.tm_wday  = word_to_number(weeks, wday);

	t2 = mktime(&t);

	return t2;
}

char *rdf_to_rfc2822(char *tstamp)
{
	char *rfc;
	int ye, mo, da, ho, mi, se;

	rfc = (char *)malloc(1024);

	sscanf(tstamp,"%d-%d-%dT%d:%d:%dZ",
		&ye,&mo,&da,&ho,&mi,&se);

	if (--mo>11||mo<0) mo = 0;

	// TODO: Foo day does not exists
	sprintf(rfc,"Foo, %d %s %d %d:%d:%d +0100",
		da, months[mo], ye, ho, mi, se);

	return rfc;
}

/*
 * Compares two rfc2822 date strings.
 *
 * Returns the relation between date1 and date2:
 *   (date1  > date2)  > 0
 *   (date1 == date2)  = 0
 *   (date1 <  date2)  < 0
 */
int cmptime_rfc2822(char *date1, char *date2)
{
	return ( rfc2822_to_utc(date2) -
		rfc2822_to_utc(date1) );
}
