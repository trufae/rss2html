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

bool is_resumed = false;

void do_resume(char *buf)
{
	int i;

	is_resumed = false;

	if (cfg.resume == 0)
		return;

	while(i<cfg.resume)
	{
		if (buf[0] == '\0')
			break;

		buf = buf + 1; /** *buf++; forward **/	
		if (buf[0] == ' ')
		{
			i++;
		}
	}

	if (buf[0])
	{
		is_resumed = true;
	}
	buf[0]='\0';
}
