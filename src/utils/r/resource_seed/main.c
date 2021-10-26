/*
 * $QNXLicenseC:
 * Copyright 2015, QNX Software Systems.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"). You
 * may not reproduce, modify or distribute this software except in
 * compliance with the License. You may obtain a copy of the License
 * at: http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OF ANY KIND, either express or implied.
 *
 * This file may contain contributions from others, either as
 * contributors under the License or as licensors under other terms.
 * Please review this entire file for other proprietary rights or license
 * notices, as well as the QNX Development Suite License Guide at
 * http://licensing.qnx.com/license-guide/ for other information.
 * $
 */


/*
 * This program creates resource database. This allows drivers to request and
 * reserve resources. This program should be run when the system boots before
 * loading any drivers that need resource (e.g. audio drivers).
 */

#include <sys/rsrcdbmgr.h>
#include <inttypes.h>
#include <stdlib.h>
#include <ctype.h>
#include <strings.h>
#include <unistd.h>
#include <sys/slog.h>
#include <sys/slogcodes.h>
#include <stdio.h>

#define BUFF_SIZE (256 + 1)

char   *opts[] = {
	"mem",
	"irq",
	"ioport",
	"dma",
	"pcimem",
	NULL
};

#define RES_TYPES 5

typedef struct resource
{
	int			type;
	uint64_t	start;
	uint64_t	end;
	char		*name;
} resource_t;


static int
parse_opts (char *str, resource_t *res)
{
	int		i;
	char	*p;
	char	*delims = { "=," };

	p = strtok (str, delims);
	for (i = 0; i < RES_TYPES; i++) {
		if (!strcmp (p, opts[i])) {
			res->type = i;
			p = strtok (NULL, delims);
			if (!isdigit(p[0])) {
				res->name = p;
				p = strtok (NULL, delims);
			}
			res->start = strtoul (p, NULL, NULL);
			p = strtok (NULL, delims);
			res->end = strtoul (p, NULL, NULL);
			return 1;
		}
	}
	return 0;
}


//
//  Private:    creates the rsrcdbmgr
//              returns 0 on success, or -1
//
static int
create_rsrcdbmgr (resource_t res)
{
	rsrc_alloc_t	item;

	switch (res.type) {
	case RSRCDBMGR_IRQ:
	case RSRCDBMGR_IO_PORT:
	case RSRCDBMGR_MEMORY:
	case RSRCDBMGR_DMA_CHANNEL:
	case RSRCDBMGR_PCI_MEMORY:
		memset (&item, 0, sizeof (item));
		item.start = res.start;
		item.end   = res.end;
		item.flags = res.type | RSRCDBMGR_FLAG_NOREMOVE;
		if (res.name) {
			item.flags |= RSRCDBMGR_FLAG_NAME;
			item.name   = res.name;
		}

		slogf (_SLOG_SETCODE (_SLOGC_PROC, 0), _SLOG_INFO, "Seeded %s %s, %lld to %lld",
			opts[res.type], res.name ? res.name : "", item.start, item.end);

		if (rsrcdbmgr_create (&item, 1) == -1) {
			slogf (_SLOG_SETCODE (_SLOGC_PROC, 1), _SLOG_ERROR,
				"rsrcdbmgr_create() failed @ %s:%d\n", __FILE__, __LINE__);
			return -1;
		}
		break;
	default:
		break;
	}
	return 0;
}

/*-------------------------------------------------------------------------
	main
-------------------------------------------------------------------------*/
int
main (int argc, char *argv[])
{
	int			i;
	int			machine_unknown = 0;
	char		buf[BUFF_SIZE];
	resource_t	res[RES_TYPES];

	for (i = 0; i < RES_TYPES; i++) {
		res[i].type = -1;
		res[i].name = NULL;
	}

	/* Now check board we are on */
	confstr (_CS_MACHINE, buf, BUFF_SIZE);
	if (strncmp (buf, "OMAP3530", 8) == 0) {
		res[RSRCDBMGR_DMA_CHANNEL].type  = RSRCDBMGR_DMA_CHANNEL;
		res[RSRCDBMGR_DMA_CHANNEL].start = 0;
		res[RSRCDBMGR_DMA_CHANNEL].end   = 31;
	} else if (strcmp (buf, "nVidia_Tegra2(T20)_Devlite_Boards") == 0) {
		res[RSRCDBMGR_DMA_CHANNEL].type  = RSRCDBMGR_DMA_CHANNEL;
		res[RSRCDBMGR_DMA_CHANNEL].start = 0;
		res[RSRCDBMGR_DMA_CHANNEL].end   = 15;
	} else if (strcmp (buf, "OMAP3730_BEAGLE_Rev_xm") == 0) {
		res[RSRCDBMGR_DMA_CHANNEL].type  = RSRCDBMGR_DMA_CHANNEL;
		res[RSRCDBMGR_DMA_CHANNEL].start = 0;
		res[RSRCDBMGR_DMA_CHANNEL].end   = 31;
	} else if (strcmp (buf, "DM814X") == 0) {
		res[RSRCDBMGR_DMA_CHANNEL].type  = RSRCDBMGR_DMA_CHANNEL;
		res[RSRCDBMGR_DMA_CHANNEL].start = 0;
		res[RSRCDBMGR_DMA_CHANNEL].end   = 63;
	} else
		machine_unknown = 1;

	if (!machine_unknown) {
		for (i = 0; i < RES_TYPES; i++) {
			if (res[i].type != -1)
				create_rsrcdbmgr (res[i]);
		}
	}

	if (machine_unknown && argc == 1) {
		slogf (_SLOG_SETCODE (_SLOGC_PROC, 0), _SLOG_INFO,
			"Hardware not recognized, Failed to seed any resources\n");
		fprintf (stderr, "Hardware not recognized, Failed to seed any resources\n");
		return 1;
	}

	for (i = 1; i < argc; i++) {
		res[0].type = -1;
		res[i].name = NULL;
		parse_opts (argv[i], &res[0]);
		if (res[0].type != -1)
			create_rsrcdbmgr (res[0]);
	}

	return 0;
}

#if defined(__QNXNTO__) && defined(__USESRCVERSION)
#include <sys/srcversion.h>
__SRCVERSION("$URL: http://svn.ott.qnx.com/product/branches/6.6.0/trunk/utils/r/resource_seed/main.c $ $Rev: 787078 $")
#endif
