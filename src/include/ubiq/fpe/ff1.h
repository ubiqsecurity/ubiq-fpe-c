#ifndef UBIQ_FPE_FF1_H
#define UBIQ_FPE_FF1_H

#include <sys/cdefs.h>
#include <stdint.h>
#include <stddef.h>

__BEGIN_DECLS

struct ff1_ctx;

/*
 * Create a context instance for use with the FF1 algorithm
 *
 * The created instance can be used for encryption or decryption
 * or both. The instance is not thread-safe, though. For example,
 * multiple instances can be created and used in different, individual
 * threads. However, a single instance cannot be used for simultaneous
 * encryptions/decryptions in multiple threads.
 *
 * @ctx: Pointer to location to store pointer to context data
 *       Caller supplies the address to a pointer. This function
 *       will fill it in
 * @keybuf: Pointer to key data
 * @keylen: Number of bytes in the key (must be 16, 24, 32)
 * @twkbuf: Pointer to tweak data (may be NULL)
 * @twklen: Number of bytes of tweak data (may be 0)
 * @mintwklen: The minimum number of bytes allowed for the tweak data
 * @maxtwklen: The maximum number of bytes allowed for the tweak data
 *             This number may be 0 to indicate that the maximum is unlimited
 * @radix: The radix of the plain/cipher text data
 *
 * @return 0 on success or a negative error number on failure
 */
int ff1_ctx_create(struct ff1_ctx ** const ctx,
                   const uint8_t * const keybuf, const size_t keylen,
                   const uint8_t * const twkbuf, const size_t twklen,
                   const size_t mintwklen, const size_t maxtwklen,
                   const unsigned int radix);

int ff1_ctx_create_custom_radix(struct ff1_ctx ** const ctx,
                   const uint8_t * const keybuf, const size_t keylen,
                   const uint8_t * const twkbuf, const size_t twklen,
                   const size_t mintwklen, const size_t maxtwklen,
                   const uint8_t * const custom_radix_str);

/*
 * Encrypt data using the FF1 algorithm
 *
 * Encryption is a "one-shot" operation. The entire plain text must
 * be passed in, and the entire cipher text is returned. The cipher
 * text uses the same "alphabet" as the plain text and will be the
 * same length as the plain text.
 *
 * @ctx: The pointer returned by the create function
 * @Y: A pointer to the location to output the cipher text. This space
 *     is allocated by the caller and must be at least as long as the
 *     plain text. The parameter must include space for a nul-terminator.
 * @X: A pointer to the location of the plain text. The plain text must
 *     be nul-terminated
 * @T: A pointer to the "tweak" parameter. This is a sequence of bytes
 *     and may be NULL. In the event that the tweak is NULL, the tweak
 *     supplied to the create function will be used.
 * @t: The number of bytes pointed to by @T. If T is NULL, t is a
 *     don't-care
 *
 * @return 0 on success or a negative error number on failure
 */
int ff1_encrypt(struct ff1_ctx * const ctx,
                char * const Y,
                const char * const X,
                const uint8_t * const T, const size_t t);
/*
 * Decrypt data using the FF1 algorithm
 *
 * Decryption is a "one-shot" operation. The entire cipher text must
 * be passed in, and the entire plain text is returned.
 *
 * @ctx: The pointer returned by the create function
 * @Y: A pointer to the location to output the plain text. This space
 *     is allocated by the caller and must be at least as long as the
 *     cipher text. The parameter must include space for a nul-terminator.
 * @X: A pointer to the location of the cipher text. The cipher text must
 *     be nul-terminated
 * @T: A pointer to the "tweak" parameter. This is a sequence of bytes
 *     and may be NULL. In the event that the tweak is NULL, the tweak
 *     supplied to the create function will be used. The tweak must be
 *     the same as that used to perform the encryption
 * @t: The number of bytes pointed to by @T. If T is NULL, t must be zero.
 *
 * @return 0 on success or a negative error number on failure
 */
int ff1_decrypt(struct ff1_ctx * const ctx,
                char * const Y,
                const char * const X,
                const uint8_t * const T, const size_t t);

/*
 * Destroy the context structure associated with the FF1 algorithm
 *
 * @ctx: The pointer returned by the create function
 */
void ff1_ctx_destroy(struct ff1_ctx * const ctx);

__END_DECLS

#endif
