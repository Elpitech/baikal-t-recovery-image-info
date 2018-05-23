/* vim: set noai ts=4 sw=4:
 *
 * recovery-image-info.h
 *
 * Header-file of the informational section parser
 *
 * (C) 2018 T-platforms. All right reserved.
 */
#ifndef __RECOVERY_IMAGE_INFO_H__
#define __RECOVERY_IMAGE_INFO_H__

#include <sys/types.h>
#include <stdint.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define RII_VERSION			PACKAGE_VERSION

#define RII_MSG_VERSION		2
#define RII_MSG_USAGE		1
#define RII_SUCCESS			0
#define RII_ERR_MISSING_ARG	-1
#define RII_ERR_INVALID_ARG	-2
#define RII_ERR_OPEN_FILE	-3
#define RII_ERR_SEEK_FILE	-4
#define RII_ERR_NO_MEMORY	-5
#define RII_ERR_READ_OPS	-6
#define RII_ERR_INVALID_MAG	-7
#define RII_ERR_INVALID_CRC	-8

#define RII_KB(bytes) ((bytes) / 1024)
#define RII_MB(bytes) ((bytes) / 1024 / 1024)

#ifndef RII_DEFAULT_NAME
#	define RII_DEFAULT_NAME	"/dev/mtd2"
#endif

#ifndef RII_DEFAULT_ADDR
#	define RII_DEFAULT_ADDR	0x0U
#endif

#ifndef RII_MAGIC
#	define RII_MAGIC	0xDEADBEEFBAADF00DULL
#endif

#define RII_NAME_LEN	64
#define RII_VERS_LEN	16
#define RII_DATE_LEN	16
#define RII_SLIST_LEN	128
#define RII_LLIST_LEN	512

enum rii_msg_mode {
	RII_PRINT_BOTH,
	RII_PRINT_MESSAGE,
	RII_PRINT_USAGE
};

struct rii_data {
	const char *fname;
	off_t offset;
	struct rii_image_info *info;

	int fd;
};

struct rii_image_info {
	uint64_t magic;

	char distro[RII_NAME_LEN];
	char version[RII_VERS_LEN];
	char machine[RII_NAME_LEN];
	char hostname[RII_NAME_LEN];

	char u_boot_version[RII_VERS_LEN];
	char kernel_version[RII_VERS_LEN];
	char compiler_version[RII_VERS_LEN];
	char compiler_features[RII_SLIST_LEN];
	char date[RII_DATE_LEN];

	uint32_t rom_base;
	uint32_t rom_size;

	uint32_t bootloader_base;
	uint32_t bootloader_size;
	uint32_t environment_base;
	uint32_t environment_size;
	uint32_t information_base;
	uint32_t information_size;
	uint32_t fitimage_base;
	uint32_t fitimage_size;
	uint8_t fitimage_signed;

	char system_utils[RII_LLIST_LEN];
	char extra_utils[RII_LLIST_LEN];
	char test_benches[RII_LLIST_LEN];
	char extra_linguas[RII_SLIST_LEN];

	uint32_t crc;
} __attribute__ ((packed));

#endif /* __RECOVERY_IMAGE_INFO_H__ */
