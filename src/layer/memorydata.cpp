// Tencent is pleased to support the open source community by making ncnn available.
//
// Copyright (C) 2017 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

#include "memorydata.h"

namespace ncnn {

DEFINE_LAYER_CREATOR(MemoryData)

MemoryData::MemoryData()
{
    one_blob_only = false;
    support_inplace = false;
}

#if NCNN_STDIO
#if NCNN_STRING
int MemoryData::load_param(FILE* paramfp)
{
    int nscan = fscanf(paramfp, "%d %d %d",
                       &w, &h, &c);
    if (nscan != 3)
    {
        fprintf(stderr, "MemoryData load_param failed %d\n", nscan);
        return -1;
    }

    return 0;
}
#endif // NCNN_STRING
int MemoryData::load_param_bin(FILE* paramfp)
{
    fread(&w, sizeof(int), 1, paramfp);

    fread(&h, sizeof(int), 1, paramfp);

    fread(&c, sizeof(int), 1, paramfp);

    return 0;
}

int MemoryData::load_model(FILE* binfp)
{
    int nread;

    if (c != 0)
    {
        data.create(w, h, c);
    }
    else if (h != 0)
    {
        data.create(w, h);
    }
    else if (w != 0)
    {
        data.create(w);
    }
    else // 0 0 0
    {
        data.create(1);
    }
    if (data.empty())
        return -100;

    for (int p=0; p<data.c; p++)
    {
        float* ptr = data.channel(p);
        nread = fread(ptr, data.w * data.h * sizeof(float), 1, binfp);
        if (nread != 1)
        {
            fprintf(stderr, "MemoryData read data failed %d\n", nread);
            return -1;
        }
    }

    return 0;
}
#endif // NCNN_STDIO

int MemoryData::load_param(const unsigned char*& mem)
{
    w = *(int*)(mem);
    mem += 4;

    h = *(int*)(mem);
    mem += 4;

    c = *(int*)(mem);
    mem += 4;

    return 0;
}

int MemoryData::load_model(const unsigned char*& mem)
{
    if (c != 0)
    {
        data.create(w, h, c);
    }
    else if (h != 0)
    {
        data.create(w, h);
    }
    else if (w != 0)
    {
        data.create(w);
    }
    else // 0 0 0
    {
        data.create(1);
    }
    if (data.empty())
        return -100;

    for (int p=0; p<data.c; p++)
    {
        float* ptr = data.channel(p);
        memcpy(ptr, mem, data.w * data.h * sizeof(float));
        mem += data.w * data.h * sizeof(float);
    }

    return 0;
}

int MemoryData::forward(const std::vector<Mat>& /*bottom_blobs*/, std::vector<Mat>& top_blobs) const
{
    Mat& top_blob = top_blobs[0];

    top_blob = data.clone();
    if (top_blob.empty())
        return -100;

    return 0;
}

} // namespace ncnn
