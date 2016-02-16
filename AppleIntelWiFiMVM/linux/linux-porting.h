/* linux.h -- Definitions to make the linux code compile under OS X.
 *
 * Copyright (c) 2014 Laura Müller <laura-mueller@uni-duesseldorf.de>
 * Copyright (c) 2016 Aaron Mulder <ammulder@alumni.princeton.edu>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#ifndef __linux_porting_h
#define __linux_porting_h

#include <IOKit/IOLib.h>

/******************************************************************************/
#pragma mark -
#pragma mark Debugging
#pragma mark -
/******************************************************************************/

#define printk(args...) IOLog(args)
#define WARN(test, message) check_warn_condition(test, message)
#define WARN_ON(x) x
#define WARN_ON_ONCE(x) x
#define __rcu

static bool check_warn_condition(bool test, char *message) {
    if(test) IOLog("%s %s", "AppleIntelWiFiMVM", message);
    return test;
}

/******************************************************************************/
#pragma mark -
#pragma mark Bits and Bytes
#pragma mark -
/******************************************************************************/

#define HZ 1000 // Milliseconds.

#if defined(__LITTLE_ENDIAN__)
#define __LITTLE_ENDIAN 1234
#define __LITTLE_ENDIAN_BITFIELD

#elif defined(__BIG_ENDIAN__)
#define __BIG_ENDIAN 4321
#define __BIG_ENDIAN_BITFIELD

#endif // ENDIAN

#define s8      SInt8
#define u8      UInt8
#define u16     UInt16
#define u32     UInt32
#define u64     UInt64
#define s32     SInt32
#define s64     SInt64
#define __be16  SInt16
#define __be32  SInt32
#define __be64  SInt64
#define __le16  SInt16
#define __le32  SInt32
#define __le64  SInt64
#define __s8    SInt8
#define __s16   SInt16
#define __s32   SInt32
#define __s64   SInt64
#define __u8    UInt8
#define __u16   UInt16
#define __u32   UInt32
#define __u64   UInt64

#define __sum16 UInt16

#define ALIGN_MASK(x, mask) (((x) + (mask)) & ~(mask))
#define ALIGN(x, a)         ALIGN_MASK(x, (typeof(x))(a) - 1)

#define cpu_to_le16(x) OSSwapHostToLittleInt16(x)
#define cpu_to_le32(x) OSSwapHostToLittleInt32(x)
#define cpu_to_le64(x) OSSwapHostToLittleInt64(x)
#define le16_to_cpu(x) OSSwapLittleToHostInt16(x)
#define le32_to_cpu(x) OSSwapLittleToHostInt32(x)
#define le64_to_cpu(x) OSSwapLittleToHostInt64(x)

#define cpu_to_be16(x) OSSwapHostToBigInt16(x)
#define cpu_to_be32(x) OSSwapHostToBigInt32(x)
#define cpu_to_be64(x) OSSwapHostToBigInt64(x)
#define be16_to_cpu(x) OSSwapBigToHostInt16(x)
#define be32_to_cpu(x) OSSwapBigToHostInt32(x)
#define be64_to_cpu(x) OSSwapBigToHostInt64(x)

#define le16_to_cpus(x) ((*x) = OSSwapLittleToHostInt16((*x)))
#define le32_to_cpus(x) ((*x) = OSSwapLittleToHostInt32((*x)))
#define le64_to_cpus(x) ((*x) = OSSwapLittleToHostInt64((*x)))

#define container_of(ptr, type, member) ({                                     \
const typeof( ((type *)0)->member ) *__mptr = (ptr);                       \
(type *)( (char *)__mptr - offsetof(type,member) );})

//#define BITS_PER_LONG           LONG_BIT
#define BITS_PER_LONG           64
#define BIT(nr)                 (1UL << (nr))
#define BIT_MASK(nr)            (1UL << ((nr) % BITS_PER_LONG))
#define BIT_WORD(nr)            ((nr) / BITS_PER_LONG)
#define BITS_PER_BYTE           8
#define BITS_TO_LONGS(bits)     (((bits)+BITS_PER_LONG-1)/BITS_PER_LONG)

#define ARRAY_SIZE(x)           (sizeof(x) / sizeof((x)[0]))

#define min_t(type,x,y) \
({ type __x = (x); type __y = (y); __x < __y ? __x: __y; })

#define max_t(type, x, y) \
({ type __x = (x); type __y = (y); __x > __y ? __x: __y; })

#if 0
enum bool_t
{
    false = 0,
    true = 1
};
typedef enum bool_t bool;
#endif

#define dma_addr_t  IOPhysicalAddress64

#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)

static inline int atomic_dec_and_test(volatile SInt32 * addr)
{
    return ((OSDecrementAtomic(addr) == 1) ? 1 : 0);
}

static inline int atomic_inc_and_test(volatile SInt32 * addr)
{
    return ((OSIncrementAtomic(addr) == -1) ? 1 : 0);
}

#define atomic_inc(v) OSIncrementAtomic(v)
#define atomic_dec(v) OSDecrementAtomic(v)

static inline int
test_bit(int nr, const volatile unsigned long *addr)
{
    return (OSAddAtomic(0, addr) & (1 << nr)) != 0;
}

static inline void
set_bit(unsigned int nr, volatile unsigned long *addr)
{
    OSTestAndSet(nr, (volatile UInt8 *)addr);
}

static inline void
clear_bit(unsigned int nr, volatile unsigned long *addr)
{
    OSTestAndClear(nr, (volatile UInt8 *)addr);
}

static inline int
test_and_clear_bit(unsigned int nr, volatile unsigned long *addr)
{
    return !OSTestAndClear(nr, (volatile UInt8 *)addr);
}

static inline int
test_and_set_bit(unsigned int nr, volatile unsigned long *addr)
{
    return OSTestAndSet(nr, (volatile UInt8 *)addr);
}

/******************************************************************************/
#pragma mark -
#pragma mark Read/Write Registers
#pragma mark -
/******************************************************************************/

OS_INLINE
void
_OSWriteInt8(
             volatile void               * base,
             uintptr_t                     byteOffset,
             uint16_t                      data
             )
{
    *(volatile uint8_t *)((uintptr_t)base + byteOffset) = data;
}

OS_INLINE
uint8_t
_OSReadInt8(
            const volatile void               * base,
            uintptr_t                     byteOffset
            )
{
    return *(volatile uint8_t *)((uintptr_t)base + byteOffset);
}

#define OSWriteLittleInt8(base, byteOffset, data) \
_OSWriteInt8((base), (byteOffset), (data))
#define OSReadLittleInt8(base, byteOffset) \
_OSReadInt8((base), (byteOffset))

#define writew(hw, reg, val16)     OSWriteLittleInt16((hw->hw_addr), (reg), (val16))
#define writel(hw, reg, val32)     OSWriteLittleInt32((hw->hw_addr), (reg), (val32))

#define readw(hw, reg)      OSReadLittleInt16((hw->hw_addr), (reg))
#define readl(hw, reg)      OSReadLittleInt32((hw->hw_addr), (reg))

#define __er32(hw, reg)      OSReadLittleInt32((hw->hw_addr), (reg))

#define E1000_WRITE_REG_ARRAY(a, reg, offset, value) \
(OSWriteLittleInt32((hw->hw_addr), (reg + ((offset) << 2)), (value)))

#define E1000_READ_REG_ARRAY(a, reg, offset) \
(OSReadLittleInt32((hw->hw_addr), (reg + ((offset) << 2))))

#define wmb() OSSynchronizeIO()

#define __er16flash(hw, reg) \
OSReadLittleInt16((hw->flash_address), (reg))

#define __er32flash(hw, reg) \
OSReadLittleInt32((hw->flash_address), (reg))

#define  __ew16flash(hw, reg,  val) \
OSWriteLittleInt16((hw->flash_address), (reg), (val))

#define  __ew32flash(hw, reg, val) \
OSWriteLittleInt32((hw->flash_address), (reg), (val))

/******************************************************************************/
#pragma mark -
#pragma mark Locks
#pragma mark -
/******************************************************************************/

#define spinlock_t  IOSimpleLock *
#define atomic_t    volatile SInt32


#define spin_lock_init(slock)                           \
do                                                      \
{                                                       \
if (*slock == NULL)                                   \
{                                                     \
*(slock) = IOSimpleLockAlloc();                     \
}                                                     \
} while (0)

#define spin_lock(lock)

#define spin_unlock(lock)

#define spin_lock_irqsave(lock,flags)

#define spin_trylock_irqsave(lock,flags)

#define spin_unlock_irqrestore(lock,flags)

#define usec_delay(x)           IODelay(x)
#define msec_delay(x)           IOSleep(x)
#define udelay(x)               IODelay(x)
#define mdelay(x)               IODelay(1000*(x))
#define msleep(x)               IOSleep(x)

#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))
#define usleep_range(min, max)	msleep(DIV_ROUND_UP(min, 1000))

enum
{
    GFP_KERNEL,
    GFP_ATOMIC,
};

#define EIO     kIOReturnError
#define EINVAL  kIOReturnBadArgument
#define ERFKILL kIOReturnError
#define ENOMEM  kIOReturnNoMemory

#define __iomem volatile
#define __devinit

#define LINUX_VERSION_CODE 30000
#define KERNEL_VERSION(x,y,z) (x*10000+100*y+z)

#define irqreturn_t int


#define __always_unused

#define DISABLED_CODE 0

struct pci_dev {
    UInt16 vendor;
    UInt16 device;
    UInt16 subsystem_vendor;
    UInt16 subsystem_device;
    UInt16 maxSnoop;
    UInt16 maxNoSnoop;
    UInt8 revision;
};

#define dev_err(x,y,z)

/**
 * is_zero_ether_addr - Determine if give Ethernet address is all zeros.
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Return true if the address is all zeroes.
 *
 * Please note: addr must be aligned to u16.
 */
static inline bool is_zero_ether_addr(const u8 *addr)
{
#if defined(CONFIG_HAVE_EFFICIENT_UNALIGNED_ACCESS)
    return ((*(const u32 *)addr) | (*(const u16 *)(addr + 4))) == 0;
#else
    return (*(const u16 *)(addr + 0) |
            *(const u16 *)(addr + 2) |
            *(const u16 *)(addr + 4)) == 0;
#endif
}

/**
 * is_multicast_ether_addr - Determine if the Ethernet address is a multicast.
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Return true if the address is a multicast address.
 * By definition the broadcast address is also a multicast address.
 */
static inline bool is_multicast_ether_addr(const u8 *addr)
{
    return 0x01 & addr[0];
}

/**
 * is_valid_ether_addr - Determine if the given Ethernet address is valid
 * @addr: Pointer to a six-byte array containing the Ethernet address
 *
 * Check that the Ethernet address (MAC) is not 00:00:00:00:00:00, is not
 * a multicast address, and is not FF:FF:FF:FF:FF:FF.
 *
 * Return true if the address is valid.
 *
 * Please note: addr must be aligned to u16.
 */
static inline bool is_valid_ether_addr(const u8 *addr)
{
    /* FF:FF:FF:FF:FF:FF is a multicast address so we don't need to
     * explicitly check for it here. */
    return !is_multicast_ether_addr(addr) && !is_zero_ether_addr(addr);
}

#define e_dbg(format, arg...)
#define e_err(format, arg...)
#define e_info(format, arg...)
#define e_warn(format, arg...)
#define e_notice(format, arg...)

#define	DEFINE_MUTEX(x)	void x##_dummy(){}
#define	mutex_lock(x)
#define	mutex_unlock(x)

#define net_device e1000_adapter
#define netdev_priv(x)  ((struct e1000_adapter *)x)

#define do_div(lat_ns, speed) \
(lat_ns) = (UInt64)(lat_ns) / (speed)

#define ether_crc_le(length, data) _kc_ether_crc_le(length, data)

static inline unsigned int _kc_ether_crc_le(int length, unsigned char *data)
{
    unsigned int crc = 0xffffffff;  /* Initial value. */
    while(--length >= 0) {
        unsigned char current_octet = *data++;
        int bit;
        for (bit = 8; --bit >= 0; current_octet >>= 1) {
            if ((crc ^ current_octet) & 1) {
                crc >>= 1;
                crc ^= 0xedb88320U;
            } else
                crc >>= 1;
        }
    }
    return crc;
}

/**
 * kmemdup - duplicate region of memory
 *
 * @src: memory region to duplicate
 * @len: memory region length
 * @gfp: GFP mask to use
 */
static void *kmemdup(const void *src, size_t len, int gfp) {
    void *p;
    p = IOMalloc(len);
    if (p)
        memcpy(p, src, len);
    return p;
}


#define VLAN_ETH_FRAME_LEN	1518	/* Max. octets in frame sans FCS */

#endif
