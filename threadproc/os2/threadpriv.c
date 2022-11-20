/* Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "apr_arch_threadproc.h"
#include "apr_thread_proc.h"
#include "apr_portable.h"
#include "apr_general.h"
#include "apr_errno.h"
#include "apr_lib.h"
#include "apr_arch_file_io.h"

APR_DECLARE(apr_status_t) apr_threadkey_private_create(apr_threadkey_t **key,
                                                       void (*dest)(void *),
                                                       apr_pool_t *pool)
{
    (*key) = (apr_threadkey_t *)apr_palloc(pool, sizeof(apr_threadkey_t));

    if ((*key) == NULL) {
        return APR_ENOMEM;
    }

    (*key)->pool = pool;
    return APR_FROM_OS_ERROR(DosAllocThreadLocalMemory(1, &((*key)->key)));
}

APR_DECLARE(apr_status_t) apr_threadkey_private_get(void **new, apr_threadkey_t *key)
{
    (*new) = (void *)*(key->key);
    return APR_SUCCESS;
}

APR_DECLARE(apr_status_t) apr_threadkey_private_set(void *priv, apr_threadkey_t *key)
{
    *(key->key) = (ULONG)priv;
    return APR_SUCCESS;
}

APR_DECLARE(apr_status_t) apr_threadkey_private_delete(apr_threadkey_t *key)
{
    return APR_FROM_OS_ERROR(DosFreeThreadLocalMemory(key->key));
}

APR_DECLARE(apr_status_t) apr_threadkey_data_get(void **data, const char *key,
                                                 apr_threadkey_t *threadkey)
{
    return apr_pool_userdata_get(data, key, threadkey->pool);
}

APR_DECLARE(apr_status_t) apr_threadkey_data_set(void *data, const char *key,
                                                 apr_status_t (*cleanup) (void *),
                                                 apr_threadkey_t *threadkey)
{
    return apr_pool_userdata_set(data, key, cleanup, threadkey->pool);
}

APR_DECLARE(apr_status_t) apr_os_threadkey_get(apr_os_threadkey_t *thekey, apr_threadkey_t *key)
{
    *thekey = key->key;
    return APR_SUCCESS;
}

APR_DECLARE(apr_status_t) apr_os_threadkey_put(apr_threadkey_t **key,
                                               apr_os_threadkey_t *thekey,
                                               apr_pool_t *pool)
{
    if (pool == NULL) {
        return APR_ENOPOOL;
    }
    if ((*key) == NULL) {
        (*key) = (apr_threadkey_t *)apr_pcalloc(pool, sizeof(apr_threadkey_t));
        (*key)->pool = pool;
    }
    (*key)->key = *thekey;
    return APR_SUCCESS;
}
