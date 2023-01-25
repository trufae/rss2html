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
#include "storage.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void storage_init()
{
	storage.n_blogs = 0;
	storage.blogs = (struct blog*)malloc(1);
}

/* TODO */
void storage_free()
{
	int i,j,k;

	for (i=0;i<storage.n_blogs;i++)
	{
		for (j=0;j<storage.blogs[i].n_posts;j++)
		{
			for (k=0;k<storage.blogs[i].posts->n_keys;k++)
			{
				free (storage.blogs[i].posts[j].keys[k].key);
				free (storage.blogs[i].posts[j].keys[k].value);
			}

			free(storage.blogs[i].posts);
			/* TODO url, title */
		}
		free(storage.blogs);
	}
}

void storage_add_blog(char *title, char *url)
{
	int bid = storage.n_blogs;

	storage.blogs = (struct blog*)realloc(storage.blogs,sizeof(struct blog)*(storage.n_blogs+1));
	storage.blogs[bid].title = strdup(title);
	storage.blogs[bid].url = strdup(url);
	storage.blogs[bid].n_posts = 0;
	storage.blogs[bid].posts = (struct post *)malloc(1);
	storage.n_blogs++;
}

void storage_add_post()
{
	int bid = storage.n_blogs - 1;
	int pid;

	if (bid < 0)
	{
		fprintf(stderr,"A post needs a blog.\n");
		exit(1);
	}

	pid = storage.blogs[bid].n_posts++;
	storage.blogs[bid].posts = 
		(struct post *)realloc(
		storage.blogs[bid].posts,
		sizeof(struct post)*storage.blogs[bid].n_posts);

	storage.blogs[bid].posts[pid].n_keys = 0;
	storage.blogs[bid].posts[pid].keys = (struct pair *)malloc(1);

	storage_set("author",storage.blogs[bid].title);
}

void storage_set(char *key, char *value)
{
	unsigned int i;
	int pid;
	int bid = storage.n_blogs - 1;
	struct post p; // = storage.blogs[bid].posts[storage.n_blogs-1];

	if (key==NULL || value==NULL) return;

	if (bid < 0)
	{
		fprintf(stderr,"A post needs a blog.\n");
		exit(1);
	}
	pid = storage.blogs[bid].n_posts - 1;

	p = storage.blogs[bid].posts[pid];

	/* looking for key in pair array */
	for (i=0;i<p.n_keys;i++)
	{
		if (!strcmp(key, p.keys[i].key))
		{
			free(p.keys[i].value);
			p.keys[i].value = strdup(value);
		}
	}

	/* key not found */
	if (i == p.n_keys)
	{
		p.n_keys++;
		p.keys = (struct pair*)realloc(p.keys, sizeof(struct pair)*(p.n_keys));
		p.keys[i].key = strdup(key);
		p.keys[i].value = strdup(value);
	}

	storage.blogs[bid].posts[pid]=p;
}

char *storage_get(struct post p, char *key)
{
	int i;

	for(i=0;i<p.n_keys;i++)
	{
		if (!strcmp(key, p.keys[i].key))
		{
			return p.keys[i].value;
		}
	}

	return NULL;
}

void storage_order_by_date()
{
	int i,j,k;
	struct post *posts;
	struct post tmp_post;
	int size=0;
	char *one,*two;

	for (i=0; i<storage.n_blogs; i++)
	{
		for (j=0;j<storage.blogs[i].n_posts;j++)
		{
			size++;
		}
	}

	posts=(struct post*)malloc((size+1)*sizeof(struct post));

	for (k=i=0; i<storage.n_blogs; i++)
	{
		for (j=0;j<storage.blogs[i].n_posts;j++,k++)
		{
			posts[k]=storage.blogs[i].posts[j];
		}
	}

	/* order */
	for (i=size-1;i>=0;i--)
	{
		for(j=0;j<i;j++)
		{
			one = storage_get(posts[j],"pubDate");
			two = storage_get(posts[j+1],"pubDate");
			if (one==NULL || two==NULL)
			{
				fprintf(stderr,"No pubdate defined for *this feed.\n");
			} else
			if (cmptime_rfc2822(one,two)>0)
			{
				tmp_post=posts[j];
				posts[j]=posts[j+1];
				posts[j+1]=tmp_post;
			}
		}
	}
	for(i=0;i<size;i++)
	{
		char *title=storage_get(posts[i],"title");
		char *author=storage_get(posts[i],"author");
		char *link=storage_get(posts[i],"link");
		char *category=storage_get(posts[i],"category");
		char *pubDate=storage_get(posts[i],"pubDate");
		char *content=storage_get(posts[i],"content");
		/*
		if (author == NULL)
		{
			if (posts[i].author)
			author=posts[i].author;
			else
			author = strdup("nobody");
			
		}
		*/

		output_post(title,author,link,category,pubDate,content);
	}

	free(posts);
}

