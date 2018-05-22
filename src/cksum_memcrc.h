/* vim: set noai ts=4 sw=4:
 *
 * cksum_memcrc.h
 *
 * Baikal-T ROM-image crc32 (POSIX 1003.1) calculator
 *
 * (C) 2018 T-platforms. All right reserved.
 */
#ifndef __CKSUM_MEMCRC__
#define __CKSUM_MEMCRC__

#include <stdint.h>

extern uint32_t cksum_memcrc(const unsigned char *b, size_t n);

#endif /* __CKSUM_MEMCRC__ */
