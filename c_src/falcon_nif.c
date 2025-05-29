
#include "inner.h"
#include "falcon.h"
#include "tools.c"
#include <stdio.h>
#include <string.h>
#include "erl_nif.h"

// #define FALSE 0
// #define TRUE 1
#define CRYPTO_LOGN (unsigned)9 // falcon-512 = 9 | falcon-1024 = 10
#define PUBKEY_LEN FALCON_PUBKEY_SIZE(CRYPTO_LOGN)
#define PRIVKEY_LEN FALCON_PRIVKEY_SIZE(CRYPTO_LOGN)
#define SIGN_TYPE FALCON_SIG_COMPRESSED
#define SIGN_LEN FALCON_SIG_COMPRESSED_MAXSIZE(CRYPTO_LOGN)
#define SIGN_MAX_LEN FALCON_SIG_PADDED_SIZE(CRYPTO_LOGN)
#define SIGN_START_SIZE 41
// #define EXPKEY_LEN FALCON_EXPANDEDKEY_SIZE(CRYPTO_LOGN)

const uint8_t START[] = {57, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static ERL_NIF_TERM
gen_key_pair(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    ErlNifBinary pub_bin, priv_bin;
    if (argc != 0)
        return enif_make_badarg(env);

    void *pubkey, *privkey;
    uint8_t *tmpkg;
    shake256_context rng;
    size_t tmpkg_len, pubkey_len, privkey_len;

    tmpkg_len = FALCON_TMPSIZE_KEYGEN(CRYPTO_LOGN);
    pubkey_len = FALCON_PUBKEY_SIZE(CRYPTO_LOGN);
    privkey_len = FALCON_PRIVKEY_SIZE(CRYPTO_LOGN);

    pubkey = xmalloc(pubkey_len);
    privkey = xmalloc(privkey_len);
    tmpkg = xmalloc(tmpkg_len);

    memset(pubkey, 0, pubkey_len);
    memset(privkey, 0, privkey_len);

    shake256_init_prng_from_system(&rng);
    int r = falcon_keygen_make(&rng, CRYPTO_LOGN, privkey, privkey_len, pubkey, pubkey_len, tmpkg, tmpkg_len);

    if (r != 0)
        return enif_make_tuple2(env, enif_make_atom(env, "error"), enif_make_atom(env, "keygen_fail"));

    if (enif_alloc_binary(pubkey_len, &pub_bin))
    {
        memcpy(pub_bin.data, pubkey, pub_bin.size);

        if (enif_alloc_binary(privkey_len, &priv_bin))
        {
            memcpy(priv_bin.data, privkey, privkey_len);
            xfree(pubkey);
            xfree(privkey);
            xfree(tmpkg);

            return enif_make_tuple3(
                env,
                enif_make_atom(env, "ok"),
                enif_make_binary(env, &pub_bin),
                enif_make_binary(env, &priv_bin));
        }
    }

    xfree(pubkey);
    xfree(privkey);
    xfree(tmpkg);

    return enif_make_tuple2(env, enif_make_atom(env, "error"), enif_make_atom(env, "mem_alloc"));
}

static ERL_NIF_TERM
gen_pub_key(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    ErlNifBinary pub_bin, priv_bin;
    if (argc != 1 || !enif_inspect_binary(env, argv[0], &priv_bin))
        return enif_make_badarg(env);

    if (priv_bin.size != PRIVKEY_LEN)
        return enif_make_tuple2(env, enif_make_atom(env, "error"), enif_make_atom(env, "key_size"));

    void *pubkey;
    uint8_t *tmpmp;
    size_t pubkey_len, tmpmp_len, privkey_len;
    
    pubkey_len = FALCON_PUBKEY_SIZE(CRYPTO_LOGN);
    privkey_len = FALCON_PRIVKEY_SIZE(CRYPTO_LOGN);
    tmpmp_len = FALCON_TMPSIZE_MAKEPUB(CRYPTO_LOGN);

    pubkey = xmalloc(pubkey_len);
    tmpmp = xmalloc(tmpmp_len);

    memset(pubkey, 0, PUBKEY_LEN);

    int r = falcon_make_public(pubkey, PUBKEY_LEN,
                               priv_bin.data, PRIVKEY_LEN, tmpmp, tmpmp_len);

    if (r != 0)
        return enif_make_tuple2(env, enif_make_atom(env, "error"), enif_make_atom(env, "makepub_fail"));

    if (enif_alloc_binary(PUBKEY_LEN, &pub_bin))
    {
        memcpy(pub_bin.data, pubkey, PUBKEY_LEN);
        xfree(pubkey);
        xfree(tmpmp);

        return enif_make_tuple2(
            env,
            enif_make_atom(env, "ok"),
            enif_make_binary(env, &pub_bin));
    }

    xfree(pubkey);
    xfree(tmpmp);
    return enif_make_tuple2(env, enif_make_atom(env, "error"), enif_make_atom(env, "mem_alloc"));
}

static ERL_NIF_TERM
gen_keys_from_seed(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    ErlNifBinary seed_bin, pub_bin, priv_bin;

    if (argc != 1 || !enif_inspect_binary(env, argv[0], &seed_bin))
    {
        return enif_make_badarg(env);
    }

    void *pubkey, *privkey;
    uint8_t *tmpkg;
    size_t tmpkg_len;
    shake256_context rng;

    tmpkg_len = FALCON_TMPSIZE_KEYGEN(CRYPTO_LOGN);

    pubkey = xmalloc(PUBKEY_LEN);
    privkey = xmalloc(PRIVKEY_LEN);
    tmpkg = xmalloc(tmpkg_len);

    memset(privkey, 0, PRIVKEY_LEN);
    memset(pubkey, 0, PUBKEY_LEN);

    shake256_init_prng_from_seed(&rng, seed_bin.data, seed_bin.size);
    int r = falcon_keygen_make(&rng, CRYPTO_LOGN, privkey, PRIVKEY_LEN, pubkey, PUBKEY_LEN, tmpkg, tmpkg_len);

    if (r != 0)
        return enif_make_tuple2(env, enif_make_atom(env, "error"), enif_make_atom(env, "keygen_fail"));

    if (enif_alloc_binary(PUBKEY_LEN, &pub_bin))
    {
        memcpy(pub_bin.data, pubkey, PUBKEY_LEN);

        if (enif_alloc_binary(PRIVKEY_LEN, &priv_bin))
        {
            memcpy(priv_bin.data, privkey, PRIVKEY_LEN);
            xfree(pubkey);
            xfree(privkey);
            xfree(tmpkg);

            return enif_make_tuple3(
                env,
                enif_make_atom(env, "ok"),
                enif_make_binary(env, &pub_bin),
                enif_make_binary(env, &priv_bin));
        }
    }
}

static ERL_NIF_TERM
sign(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    ErlNifBinary priv_bin, msg_bin, sig_bin;

    if (argc != 2 ||
        !enif_inspect_binary(env, argv[0], &priv_bin) ||
        !enif_inspect_binary(env, argv[1], &msg_bin))
    {
        return enif_make_badarg(env);
    }

    if (priv_bin.size != PRIVKEY_LEN)
        return enif_make_tuple2(env, enif_make_atom(env, "error"), enif_make_atom(env, "key_size"));

    size_t sig_len, tmpsd_len;
    uint8_t *sig;
    uint8_t *tmpsd;
    shake256_context rng;

    sig_len = SIGN_MAX_LEN;
    tmpsd_len = FALCON_TMPSIZE_SIGNDYN(CRYPTO_LOGN);

    sig = xmalloc(sig_len);
    tmpsd = xmalloc(tmpsd_len);

    memset(sig, 0, sig_len);

    shake256_init_prng_from_system(&rng);
    int r = falcon_sign_dyn(&rng, sig, &sig_len, SIGN_TYPE,
                            priv_bin.data, priv_bin.size,
                            msg_bin.data, msg_bin.size, tmpsd, tmpsd_len);

    if (r != 0)
        return enif_make_tuple2(env, enif_make_atom(env, "error"), enif_make_atom(env, "sign_failed"));

    //SIGN_MAX_LEN == sig_len ? SIGN_MAX_LEN + 1 : sig_len;
    size_t sig_size = sig_len - SIGN_START_SIZE;

    if (enif_alloc_binary(sig_size, &sig_bin))
    {
        memcpy(sig_bin.data, sig + SIGN_START_SIZE, sig_size);
        xfree(tmpsd);
        xfree(sig);

        return enif_make_tuple2(env, enif_make_atom(env, "ok"), enif_make_binary(env, &sig_bin));
    }

    xfree(tmpsd);
    xfree(sig);
    return enif_make_tuple2(env, enif_make_atom(env, "error"), enif_make_atom(env, "mem_alloc"));
}

static ERL_NIF_TERM
verify(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[])
{
    ErlNifBinary msg_bin, sig_bin, pub_bin;

    if (argc != 3 ||
        !enif_inspect_binary(env, argv[0], &msg_bin) ||
        !enif_inspect_binary(env, argv[1], &sig_bin) ||
        !enif_inspect_binary(env, argv[2], &pub_bin))
    {
        return enif_make_badarg(env);
    }

    if (pub_bin.size != PUBKEY_LEN)
        return enif_make_tuple2(env, enif_make_atom(env, "error"), enif_make_atom(env, "key_size"));

    if (sig_bin.size > SIGN_MAX_LEN)
        return enif_make_tuple2(env, enif_make_atom(env, "error"), enif_make_atom(env, "sign_size"));

    size_t tmpvv_len = FALCON_TMPSIZE_VERIFY(CRYPTO_LOGN);
    uint8_t *tmpvv = xmalloc(tmpvv_len);

    // size_t sig_size = sig_bin.size == (SIGN_MAX_LEN + 1) ? (sig_bin.size - 1) : sig_bin.size;
    uint8_t *sig;
    size_t sig_size = sig_bin.size + SIGN_START_SIZE;
    sig = xmalloc(sig_size);
    memcpy(sig, &START, SIGN_START_SIZE);
    memcpy(sig + SIGN_START_SIZE, sig_bin.data, sig_bin.size);

    int r = falcon_verify(sig, sig_size, SIGN_TYPE,
                          pub_bin.data, pub_bin.size, msg_bin.data, msg_bin.size,
                          tmpvv, tmpvv_len);

    xfree(tmpvv);
    xfree(sig);
    if (r != 0)
        return enif_make_atom(env, "error");

    return enif_make_atom(env, "ok");
}

static ErlNifFunc nif_funcs[] = {
    {"gen_key_pair", 0, gen_key_pair},
    {"gen_pub_key", 1, gen_pub_key},
    {"gen_keys_from_seed", 1, gen_keys_from_seed},
    {"sign", 2, sign},
    {"verify", 3, verify}};

ERL_NIF_INIT(Elixir.Falcon, nif_funcs, NULL, NULL, NULL, NULL)