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

/* key-value pairs */
struct pair
{
	char *key;
	char *value;
};

/* each RSS have one post per post */
struct post
{
	int n_keys;
	struct pair *keys;
};

/* each post */
struct blog
{
	char *url;
	char *title;
	int n_posts;
	struct post *posts;
};

/* the planet */
struct planet
{
	int n_blogs;
	struct blog *blogs;
};

struct planet storage;

void storage_init();
void storage_free();

void storage_add_blog(char *title, char *url);
void storage_add_post();
void storage_set(char *key, char *value);
void storage_order_by_date();

int cmptime_rfc2822(char *date1, char *date2);
