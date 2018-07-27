/* vim: set noai ts=4 sw=4:
 *
 * recovery-image-info.c
 *
 * Baikal-T ROM-image informational section parser
 *
 * (C) 2018 T-platforms. All right reserved.
 */
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <endian.h>
#include <time.h>

#include "cksum_memcrc.h"
#include "recovery-image-info.h"

static void rii_status_msg(enum rii_msg_mode mode, const char *fmt, ...)
{
	if (mode == RII_PRINT_MESSAGE || mode == RII_PRINT_BOTH) {
		va_list argptr;

		va_start(argptr, fmt);
		vprintf(fmt, argptr);
		va_end(argptr);

		putchar('\n');
	}

	if (mode == RII_PRINT_USAGE || mode == RII_PRINT_BOTH) {
		printf("Usage: recovery-image-info [OPTIONS]\n"
			   "Extract information from Baikal-T Recovery ROM-image.\n\n"
			   "Options:\n");
		printf(" -r, --rom=name        ROM-image file name (default %s)\n",
			   RII_DEFAULT_NAME);
		printf(" -o, --offset=address  Info-section address (default 0x%08x)\n",
			   RII_DEFAULT_ADDR);
		printf(" -h, --help            Print this help message and exit\n");
		printf(" -v, --version         Output version information and exit\n\n");
		printf("For mor information see recovey-image-info(1)\n");
	}
}

static int rii_parse_args(int argc, char *argv[], struct rii_data *data)
{
	static const char *optstring = ":r:o:hv";
	static struct option longopts[] = {
		{"rom", required_argument, NULL, 'r'},
		{"offset", required_argument, NULL, 'o'},
		{"help", no_argument, NULL, 'h'},
		{"version", no_argument, NULL, 'v'}
	};
	int longindex = 0;
	char *endptr;
	int opt;

	while (1) {
		opt = getopt_long(argc, argv, optstring, longopts, &longindex);

		switch (opt) {
		case 'r':
			data->fname = optarg;
			break;
		case 'o':
			errno = 0;
			data->offset = strtol(optarg, &endptr, 0);
			if (errno || optarg == endptr) {
				rii_status_msg(RII_PRINT_BOTH, "Error: Couldn't convert offset '%s' to integer\n", optarg);
				return RII_ERR_INVALID_ARG;
			}
			break;
		case 'h':
			rii_status_msg(RII_PRINT_USAGE, NULL);
			return RII_MSG_USAGE;
		case 'v':
			rii_status_msg(RII_PRINT_MESSAGE, "recovery-image-info v%s", RII_VERSION);
			return RII_MSG_VERSION;
		case ':':
			rii_status_msg(RII_PRINT_BOTH, "Error: Option '-%c' requires an argument\n", optopt);
			return RII_ERR_MISSING_ARG;
		case '?':
			rii_status_msg(RII_PRINT_BOTH, "Error: Invalid argument '%c' detected\n", optopt);
			return RII_ERR_INVALID_ARG;
		case -1:
			return RII_SUCCESS;
		default:
			break;
		}
	}

	return RII_SUCCESS;
}

static int rii_open_rom(struct rii_data *data)
{
	int status;

	data->fd = open(data->fname, O_RDONLY);
	if (data->fd < 0) {
		rii_status_msg(RII_PRINT_MESSAGE, "Error: File '%s' couldn't be opened",
					   data->fname);
		return RII_ERR_OPEN_FILE;
	}

	status = lseek(data->fd, data->offset, SEEK_SET);
	if (status < 0) {
		close(data->fd);
		rii_status_msg(RII_PRINT_MESSAGE, "Error: Could't set offset %d in file '%s'",
					   data->offset, data->fname);
		return RII_ERR_SEEK_FILE;
	}

	return RII_SUCCESS;
}

static void rii_close_rom(struct rii_data *data)
{
	close(data->fd);
}

static int rii_read_info(struct rii_data *data)
{
	size_t count = sizeof(struct rii_image_info);
	ssize_t status;
	uint32_t cksum;

	data->info = malloc(count);
	if (!data->info) {
		rii_status_msg(RII_PRINT_MESSAGE, "Error: No memory for info");
		return RII_ERR_NO_MEMORY;
	}

	status = read(data->fd, data->info, count);
	if (status != count) {
		status = RII_ERR_READ_OPS;
		rii_status_msg(RII_PRINT_MESSAGE, "Error: Failed to read info block of size %d",
					   count);
		goto err_free_info;
	}

	if (be64toh(data->info->magic) != RII_MAGIC) {
		status = RII_ERR_INVALID_MAG;
		rii_status_msg(RII_PRINT_MESSAGE, "Error: Invalid info block identifier");
		goto err_free_info;
	}

	cksum = htobe32(cksum_memcrc((unsigned char *)data->info, count - sizeof(data->info->crc)));
	if (cksum != data->info->crc) {
		status = RII_ERR_INVALID_MAG;
		rii_status_msg(RII_PRINT_MESSAGE, "Error: Invalid info block crc");
		goto err_free_info;
	}

	return RII_SUCCESS;

err_free_info:
	free(data->info);

	return status;
}

static void rii_free_info(struct rii_data *data)
{
	free(data->info);
}

static void rii_print_info(struct rii_data *data)
{
	struct rii_image_info *info = data->info;
	struct tm tm = {0};

	printf("Distribution:        %s %s for board %s\n", info->distro, info->version, info->machine);
	printf("Hostname:            %s\n", info->hostname);
	printf("Core systems:        u-boot %s, kernel %s\n", info->u_boot_version, info->kernel_version);
	printf("Built by:            gcc %s with features %s\n", info->compiler_version, info->compiler_features);
	strptime(info->datetime, "%Y%m%d%H%M%S", &tm);
	printf("Build date:          %d/%d/%d %d:%d:%d\n", tm.tm_mon + 1, tm.tm_mday, tm.tm_year + 1900, tm.tm_hour,
			tm.tm_min, tm.tm_sec);
	printf("SPI-flash info:      address 0x%08x, size %uKB (%u)\n", be32toh(info->rom_base),
			RII_KB(be32toh(info->rom_size)), be32toh(info->rom_size));
	printf("Bootloader section:  offset 0x%08x, size %uKB (%u)\n", be32toh(info->bootloader_base),
			RII_KB(be32toh(info->bootloader_size)), be32toh(info->bootloader_size));
	printf("Environment section: offset 0x%08x, size %uKB (%u)\n", be32toh(info->environment_base),
			RII_KB(be32toh(info->environment_size)), be32toh(info->environment_size));
	printf("Information section: offset 0x%08x, size %uKB (%u)\n", be32toh(info->information_base),
			RII_KB(be32toh(info->information_size)), be32toh(info->information_size));
	printf("Fitimage section:    offset 0x%08x, size %uKB (%u), %s\n", be32toh(info->fitimage_base),
			RII_KB(be32toh(info->fitimage_size)), be32toh(info->fitimage_size),
			info->fitimage_signed ? "signed" : "unsigned");
	printf("Kernel load address: vmlinuz 0x%08x, vmlinux 0x%08x\n", be32toh(info->vmlinuz_ldaddr),
			be32toh(info->vmlinux_ldaddr));
	printf("FDT load address:    0x%08x\n", be32toh(info->fdt_ldaddr));
	printf("Initrd load address: 0x%08x\n", be32toh(info->rd_ldaddr));
	printf("System utils:        %s\n", info->system_utils);
	printf("Extra utils:         %s\n", info->extra_utils);
	printf("Test benches:        %s\n", info->test_benches);
	printf("Extra linguas:       %s\n", info->extra_linguas);
}

int main(int argc, char *argv[])
{
	struct rii_data data = {
		.fname = RII_DEFAULT_NAME,
		.offset = RII_DEFAULT_ADDR,
		.fd = 0
	};
	int status;

	/* Parse input arguments */
	status = rii_parse_args(argc, argv, &data);
	if (status == RII_MSG_USAGE)
		return RII_SUCCESS;
	else if (status != RII_SUCCESS)
		return status;

	/* Open the ROM-image file setting the pointer to the specified position */
	status = rii_open_rom(&data);
	if (status != RII_SUCCESS)
		return status;

	/* Read ROM-image data from just opened file */
	status = rii_read_info(&data);
	if (status != RII_SUCCESS)
		goto close_rom;

	/* Parse the informational section of the image */
	rii_print_info(&data);

	/* Free allocated structures */
free_info:
	rii_free_info(&data);

close_rom:
	rii_close_rom(&data);

	return status;
}
