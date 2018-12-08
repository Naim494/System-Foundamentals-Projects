#include "data.h"
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h>
#include <debug.h>
#include "csapp.h"

/*
 * Create a blob with given content and size.
 * The content is copied, rather than shared with the caller.
 * The returned blob has one reference, which becomes the caller's
 * responsibility.
 *
 * @param content  The content of the blob.
 * @param size  The size in bytes of the content.
 * @return  The new blob, which has reference count 1.
 */
BLOB *blob_create(char *content, size_t size)
{
    BLOB *blb = malloc(sizeof(BLOB));
    char *content_cpy = malloc(size);
    strcpy(content_cpy, content);
    blb -> content = content_cpy;
    debug("bp content -> %s", blb -> content);
    blb -> size = size;
    pthread_mutex_init(&blb -> mutex, NULL);
    blob_ref(blb, blb -> content);

    debug("Create blob with content %p, size %d -> %p", blb -> content, (int)size, blb);

    return blb;
}

/*
 * Increase the reference count on a blob.
 *
 * @param bp  The blob.
 * @param why  Short phrase explaining the purpose of the increase.
 * @return  The blob pointer passed as the argument.
 */
BLOB *blob_ref(BLOB *bp, char *why)
{
    pthread_mutex_lock(&bp -> mutex);
    ++bp -> refcnt;
    debug("Increase reference count on blob %p [%s] (%d -> %d) for newly created blob",
    bp, bp -> content, (bp -> refcnt)-1, bp -> refcnt);
    bp -> prefix = why;
    pthread_mutex_unlock(&bp -> mutex);

    debug("bp content adrs-> %p", bp -> content);

    return bp;
}

/*
 * Decrease the reference count on a blob.
 * If the reference count reaches zero, the blob is freed.
 *
 * @param bp  The blob.
 * @param why  Short phrase explaining the purpose of the decrease.
 */
void blob_unref(BLOB *bp, char *why)
{
    debug("Decrease blob refcnt of blob -> %p (%s)", bp, why);
    pthread_mutex_lock(&bp -> mutex);
    if(--bp -> refcnt == 0)
    {
        debug("bp adrs-> %p", bp);
        debug("bp content adrs-> %p", bp -> content);
        free(bp -> content);
        free(bp);

    }
    debug("************************** (1) **********************");

    pthread_mutex_unlock(&bp -> mutex);
}

/*
 * Compare two blobs for equality of their content.
 *
 * @param bp1  The first blob.
 * @param bp2  The second blob.
 * @return 0 if the blobs have equal content, nonzero otherwise.
 */
int blob_compare(BLOB *bp1, BLOB *bp2)
{
    debug("Compare blobs %p and %p", bp1, bp2);
    debug("bp content adrs-> %p", bp1 -> content);
    debug("bp content adrs-> %p", bp2 -> content);
    return strcmp(bp1 -> content, bp2 -> content);

}

/*
 * Hash function for hashing the content of a blob.
 *
 * @param bp  The blob.
 * @return  Hash of the blob.
 */
int blob_hash(BLOB *bp)
{
    debug("Create hash from blob -> %p", bp);
    int hash = 0;
    int len = strlen(bp -> content);
    char *cpy = bp -> content;

    for (int i = 0; i < len; i++)
        hash = 31 * hash + *cpy++;

    debug("bp content adrs-> %p", bp -> content);

    return hash;
}

/*
 * Create a key from a blob.
 * The key inherits the caller's reference to the blob.
 *
 * @param bp  The blob.
 * @return  the newly created key.
 */
KEY *key_create(BLOB *bp)
{
    debug("Create key from blob -> %p", bp);
    KEY *key = malloc(sizeof(KEY));
    key -> blob = bp;
    key -> hash = blob_hash(bp);
    debug("Create key from blob %p -> %p [%s]", bp, key, bp -> content);

    debug("bp content adrs-> %p", bp -> content);

    return key;
}

/*
 * Dispose of a key, decreasing the reference count of the contained blob.
 * A key must be disposed of only once and must not be referred to again
 * after it has been disposed.
 *
 * @param kp  The key.
 */
void key_dispose(KEY *kp)
{
    debug("Dispose key: %p", kp);
    blob_unref(kp -> blob, "key is disposed");

    //debug("bp content adrs-> %p", kp -> bp -> content);
    free(kp);
}

/*
 * Compare two keys for equality.
 *
 * @param kp1  The first key.
 * @param kp2  The second key.
 * @return  0 if the keys are equal, otherwise nonzero.
 */
int key_compare(KEY *kp1, KEY *kp2)
{
    debug("Compare keys: %p and %p ", kp1,kp2);
    if(kp1 -> hash == kp2 -> hash)
        return blob_compare(kp1 -> blob, kp2 -> blob);
    //debug("bp content adrs-> %p", bp -> content);

    return 1;
}

/*
 * Create a version of a blob for a specified creator transaction.
 * The version inherits the caller's reference to the blob.
 * The reference count of the creator transaction is increased to
 * account for the reference that is stored in the version.
 *
 * @param tp  The creator transaction.
 * @param bp  The blob.
 * @return  The newly created version.
 */
VERSION *version_create(TRANSACTION *tp, BLOB *bp)
{
    debug("Create version");
    VERSION *ver = malloc(sizeof(VERSION));
    ver -> creator = tp;
    trans_ref(tp, "created version for it");
    ver -> blob = bp;
    ver -> next = (VERSION*)NULL;
    ver -> prev = (VERSION*)NULL;

    //debug("bp content adrs-> %p", bp -> content);

    return ver;
}

/*
 * Dispose of a version, decreasing the reference count of the
 * creator transaction and contained blob.  A version must be
 * disposed of only once and must not be referred to again once
 * it has been disposed.
 *
 * @param vp  The version to be disposed.
 */
void version_dispose(VERSION *vp)
{
    debug("Dispose version");
    blob_unref(vp -> blob, "version is disposed");
    trans_unref(vp -> creator, "version is disposed");

    //debug("bp content adrs-> %p", bp -> content);
    free(vp);
}




