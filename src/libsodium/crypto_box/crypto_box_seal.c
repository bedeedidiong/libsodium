
#include <string.h>

#include "crypto_box.h"
#include "crypto_generichash.h"
#include "utils.h"

static int
_crypto_box_seal_nonce(unsigned char *nonce,
                       const unsigned char *pk1, const unsigned char *pk2)
{
    crypto_generichash_state st;

    crypto_generichash_init(&st, NULL, 0U, crypto_box_NONCEBYTES);
    crypto_generichash_update(&st, pk1, crypto_box_PUBLICKEYBYTES);
    crypto_generichash_update(&st, pk2, crypto_box_PUBLICKEYBYTES);
    crypto_generichash_final(&st, nonce, crypto_box_NONCEBYTES);

    return 0;
}

int
crypto_box_seal(unsigned char *out, const unsigned char *in,
                unsigned long long inlen, const unsigned char *pk)
{
    unsigned char nonce[crypto_box_NONCEBYTES];
    unsigned char epk[crypto_box_PUBLICKEYBYTES];
    unsigned char esk[crypto_box_SECRETKEYBYTES];
    int           ret;

    if (crypto_box_keypair(epk, esk) != 0) {
        return -1;
    }
    memcpy(out, epk, crypto_box_PUBLICKEYBYTES);
    _crypto_box_seal_nonce(nonce, epk, pk);
    ret = crypto_box_easy(out + crypto_box_PUBLICKEYBYTES, in, inlen,
                          nonce, pk, esk);
    sodium_memzero(esk, sizeof esk);

    return ret;
}

int
crypto_box_seal_open(unsigned char *out, const unsigned char *in,
                     unsigned long long inlen,
                     const unsigned char *pk, const unsigned char *sk)
{
    unsigned char nonce[crypto_box_NONCEBYTES];

    if (inlen < crypto_box_SEALBYTES) {
        return -1;
    }
    _crypto_box_seal_nonce(nonce, in, pk);

    (void) sizeof(int[crypto_box_PUBLICKEYBYTES < crypto_box_SEALBYTES ? 1 : -1]);
    return crypto_box_open_easy(out, in + crypto_box_PUBLICKEYBYTES,
                                inlen - crypto_box_PUBLICKEYBYTES,
                                nonce, in, sk);
}

size_t
crypto_box_sealbytes(void)
{
    return crypto_box_SEALBYTES;
}
