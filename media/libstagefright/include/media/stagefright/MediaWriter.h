/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MEDIA_WRITER_H_

#define MEDIA_WRITER_H_

#include <utils/RefBase.h>
#include <media/stagefright/MediaSource.h>
#include <media/IMediaRecorderClient.h>
#include <media/stagefright/MediaMuxer.h>

namespace android {

struct MediaWriter : public RefBase {
    MediaWriter()
        : mMaxFileSizeLimitBytes(0),
          mMaxFileDurationLimitUs(0) {
    }

    virtual status_t addSource(const sp<MediaSource> &source) = 0;
    virtual bool reachedEOS() = 0;
    virtual status_t start(MetaData *params = NULL) = 0;
    virtual status_t stop() = 0;
    virtual status_t pause() = 0;
    virtual status_t setCaptureRate(float /* captureFps */) {
        ALOG(LOG_WARN, "MediaWriter", "setCaptureRate unsupported");
        return ERROR_UNSUPPORTED;
    }

    virtual void setMaxFileSize(int64_t bytes) { mMaxFileSizeLimitBytes = bytes; }
    virtual void setMaxFileDuration(int64_t durationUs) { mMaxFileDurationLimitUs = durationUs; }
    virtual void setListener(const sp<IMediaRecorderClient>& listener) {
        mListener = listener;
    }
    virtual void setMuxerListener(const wp<MediaMuxer>& muxer) { mMuxer = muxer; }

    virtual status_t dump(int /*fd*/, const Vector<String16>& /*args*/) {
        return OK;
    }

    virtual void setStartTimeOffsetMs(int /*ms*/) {}
    virtual int32_t getStartTimeOffsetMs() const { return 0; }
    virtual status_t setNextFd(int /*fd*/) { return INVALID_OPERATION; }

protected:
    virtual ~MediaWriter() {}
    int64_t mMaxFileSizeLimitBytes;
    int64_t mMaxFileDurationLimitUs;
    sp<IMediaRecorderClient> mListener;
    wp<MediaMuxer> mMuxer;

    void notify(int msg, int ext1, int ext2) {
        ALOG(LOG_VERBOSE, "MediaWriter", "notify msg:%d, ext1:%d, ext2:%d", msg, ext1, ext2);
        if (mListener != nullptr) {
            mListener->notify(msg, ext1, ext2);
        }
        sp<MediaMuxer> muxer = mMuxer.promote();
        if (muxer != nullptr) {
            muxer->notify(msg, ext1, ext2);
        }
    }
private:
    MediaWriter(const MediaWriter &);
    MediaWriter &operator=(const MediaWriter &);
};

}  // namespace android

#endif  // MEDIA_WRITER_H_
