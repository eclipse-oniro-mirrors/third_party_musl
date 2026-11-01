/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * The 2 MiB alignment of prelink_align_data makes lld emit a PT_LOAD with
 * p_align == 0x200000, so the loader must use KPMD_SIZE when THP is on.
 * Zero-initialized on purpose: it lands in .bss, so filesz == 0.
 */
volatile int prelink_align_data __attribute__((aligned(0x200000)));

int prelink_align_probe(void)
{
	return 42;
}
