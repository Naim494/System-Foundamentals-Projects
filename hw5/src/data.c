// #include "data.h"
// #include <stdlib.h>
// #include <semaphore.h>
// #include <pthread.h>
// #include <string.h>

// /*
//  * Create a blob with given content and size.
//  * The content is copied, rather than shared with the caller.
//  * The returned blob has one reference, which becomes the caller's
//  * responsibility.
//  *
//  * @param content  The content of the blob.
//  * @param size  The size in bytes of the content.
//  * @return  The new blob, which has reference count 1.
//  */
// BLOB *blob_create(char *content, size_t size)
// {
//     BLOB *blb = malloc(sizeof(BLOB));
//     char *content_cpy;
//     content_cpy = content;

//     blb -> content = content_cpy;
//     blb -> size = size;
//     pthread_mutex_init(&blb -> mutex, NULL);
//     blob_ref(blb, "newly created blob");

//     return blb;
// }

// /*
//  * Increase the reference count on a blob.
//  *
//  * @param bp  The blob.
//  * @param why  Short phrase explaining the purpose of the increase.
//  * @return  The blob pointer passed as the argument.
//  */
// BLOB *blob_ref(BLOB *bp, char *why)
// {
//     pthread_mutex_lock(&bp -> mutex);
//     ++bp -> refcnt;
//     bp -> prefix = why;
//     pthread_mutex_unlock(&bp -> mutex);

//     return bp;
// }

// /*
//  * Decrease the reference count on a blob.
//  * If the reference count reaches zero, the blob is freed.
//  *
//  * @param bp  The blob.
//  * @param why  Short phrase explaining the purpose of the decrease.
//  */
// void blob_unref(BLOB *bp, char *why)
// {
//     pthread_mutex_lock(&bp -> mutex);
//     if(--bp -> refcnt == 0)
//         free(bp);

//     //bp -> prefix = why;
//     pthread_mutex_unlock(&bp -> mutex);
// }

// /*
//  * Compare two blobs for equality of their content.
//  *
//  * @param bp1  The first blob.
//  * @param bp2  The second blob.
//  * @return 0 if the blobs have equal content, nonzero otherwise.
//  */
// int blob_compare(BLOB *bp1, BLOB *bp2)
// {
//     return strcmp(bp1 -> content, bp2 -> content);
// }

// /*
//  * Hash function for hashing the content of a blob.
//  *
//  * @param bp  The blob.
//  * @return  Hash of the blob.
//  */
// int blob_hash(BLOB *bp)
// {
//     int hash = 0;
//     int len = strlen(bp -> content);

//     for (int i = 0; i < len; i++)
//         hash = 31 * hash + *(bp -> content)++;

//     return hash;
// }

// /*
//  * Create a key from a blob.
//  * The key inherits the caller's reference to the blob.
//  *
//  * @param bp  The blob.
//  * @return  the newly created key.
//  */
// KEY *key_create(BLOB *bp)
// {
//     KEY *key = malloc(sizeof(KEY));
//     key -> blob = bp;
//     key -> hash = blob_hash(bp);
//     //blob_ref(bp, "created key for it");

//     return key;
// }

// /*
//  * Dispose of a key, decreasing the reference count of the contained blob.
//  * A key must be disposed of only once and must not be referred to again
//  * after it has been disposed.
//  *
//  * @param kp  The key.
//  */
// void key_dispose(KEY *kp)
// {
//     blob_unref(kp -> blob, "key is disposed");
//     free(kp);
// }

// /*
//  * Compare two keys for equality.
//  *
//  * @param kp1  The first key.
//  * @param kp2  The second key.
//  * @return  0 if the keys are equal, otherwise nonzero.
//  */
// int key_compare(KEY *kp1, KEY *kp2)
// {
//     if(kp1 -> hash == kp2 -> hash)
//         return blob_compare(kp1 -> blob, kp2 -> blob);

//     return 1;
// }

// /*
//  * Create a version of a blob for a specified creator transaction.
//  * The version inherits the caller's reference to the blob.
//  * The reference count of the creator transaction is increased to
//  * account for the reference that is stored in the version.
//  *
//  * @param tp  The creator transaction.
//  * @param bp  The blob.
//  * @return  The newly created version.
//  */
// VERSION *version_create(TRANSACTION *tp, BLOB *bp)
// {
//     VERSION *ver = malloc(sizeof(VERSION));
//     ver -> creator = tp;
//     trans_ref(tp, "created version for it");
//     ver -> blob = bp;
//     //blob_ref(bp, "created version for it");    /* NOT SURE OF THIS */
//     ver -> next = (VERSION*)NULL;
//     ver -> prev = (VERSION*)NULL;

//     return ver;
// }

// /*
//  * Dispose of a version, decreasing the reference count of the
//  * creator transaction and contained blob.  A version must be
//  * disposed of only once and must not be referred to again once
//  * it has been disposed.
//  *
//  * @param vp  The version to be disposed.
//  */
// void version_dispose(VERSION *vp)
// {
//     blob_unref(vp -> blob, "version is disposed");
//     trans_unref(vp -> creator, "version is disposed");
//     free(vp);
// }




