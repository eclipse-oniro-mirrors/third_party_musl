/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OHOS_SO_DEDUP_H
#define OHOS_SO_DEDUP_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
enum {
	SO_DEDUP_LINKED = 0,
	SO_DEDUP_NO_MATCH = 1,
	SO_DEDUP_ALREADY_LINKED = 2,
	SO_DEDUP_CONFIG_UNAVAILABLE = 3,
	SO_DEDUP_VERSION_MISMATCH = 4,
};
#define SO_DEDUP_ERROR UINT32_MAX

/*
 * Creates a symbolic link at library_path, which must not already exist.
 * The link target is a SO file under /system/lib64/dedupSource selected from
 * deduplist.json using two-layer fallback:
 *   Layer 1: match original_sha256 against the sha256 configured for an
 *            API version, then derive the path from the matched version key.
 *   Layer 2: fall back to the newest configured version. The sha256 does
 *            not match the app's original so, returns VERSION_MISMATCH.
 *
 * System callers only: the interface is reserved for the system installation
 * path (BMS/installd). A call made from a third-party application process is
 * rejected with SO_DEDUP_ERROR and errno EACCES before any configuration or
 * filesystem access happens.
 *
 * Returns SO_DEDUP_LINKED when a symbolic link is created,
 * SO_DEDUP_ALREADY_LINKED when the target already links to the selected SO,
 * SO_DEDUP_NO_MATCH when no configured SO can be selected,
 * SO_DEDUP_CONFIG_UNAVAILABLE when deduplist.json cannot be found,
 * SO_DEDUP_VERSION_MISMATCH when the linked version does not match the
 * original sha256 (Layer 2 fallback), or
 * SO_DEDUP_ERROR on failure. errno is set only when SO_DEDUP_ERROR is returned.
 */
uint32_t OH_LinkDedupedSo(const char *library_path, const char *original_sha256);

#ifdef __cplusplus
}
#endif

#endif /* OHOS_SO_DEDUP_H */
