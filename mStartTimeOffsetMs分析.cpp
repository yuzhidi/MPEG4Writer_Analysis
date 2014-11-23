    ALOGV("[%s], [%d], mStartTimeOffsetMs:%d",__FUNCTION__,__LINE__, mStartTimeOffsetMs);

	开始时间的处理会考虑到音视频同步，丢音频帧。needDropAudioFrame这里先不研究。

	R:\mt6595_kk\out\target\product\ckt95_lw_kk\system\lib>adb logcat -v time -s StagefrightRecorder MPEG4Writer  | grep -E "mStartTimeOffsetMs|startTimeOffsetUs|Start time offset|startTimeUs|mStartTimeRealUs|mStartTimestampUs"
	01-06 03:32:35.100 D/StagefrightRecorder( 4350): [setupMPEG4Recording], [1804], mStartTimeOffsetMs:-1
	01-06 03:32:35.100 D/StagefrightRecorder( 4350): [startMPEG4Recording] startTimeUs=4751835177
	01-06 03:32:35.102 D/MPEG4Writer( 4350): video Track  start,mStartTimeRealUs=4751835177
	01-06 03:32:35.102 D/MPEG4Writer( 4350): [start], [2843], startTimeOffsetUs:-1000
	01-06 03:32:35.102 D/MPEG4Writer( 4350): [start], [2848], startTimeOffsetUs:700000
	01-06 03:32:35.102 D/MPEG4Writer( 4350): [start], [2857], startTimeUs:4751835177
	01-06 03:32:35.102 I/MPEG4Writer( 4350): Start time offset: 700000 us
	01-06 03:32:35.102 D/MPEG4Writer( 4350): [start], [2861], startTimeUs:4752535177
	01-06 03:32:35.621 D/MPEG4Writer( 4350): audio Track  start,mStartTimeRealUs=4751835177
	01-06 03:32:35.621 D/MPEG4Writer( 4350): [start], [2843], startTimeOffsetUs:-1000
	01-06 03:32:35.621 D/MPEG4Writer( 4350): [start], [2848], startTimeOffsetUs:700000
	01-06 03:32:35.621 D/MPEG4Writer( 4350): [start], [2857], startTimeUs:4751835177
	01-06 03:32:35.621 I/MPEG4Writer( 4350): Start time offset: 700000 us
	01-06 03:32:35.621 D/MPEG4Writer( 4350): [start], [2861], startTimeUs:4752535177    //最终的startTimeUs就是在原来的基础上加了70000
	01-06 04:04:04.156 D/MPEG4Writer( 5410): In while threadEntry, 4004
	01-06 04:04:04.157 I/MPEG4Writer( 5410): [threadEntry], [4075], isCodecConfig:1     //第一帧带codec config
	01-06 04:04:04.181 D/MPEG4Writer( 5410): In while threadEntry, 4004
	01-06 04:04:04.181 I/MPEG4Writer( 5410): [threadEntry], [4075], isCodecConfig:1
	01-06 03:32:36.033 D/MPEG4Writer( 4350): Video mStartTimestampUs=16892us
	01-06 03:32:36.114 D/MPEG4Writer( 4350): Audio mStartTimestampUs=105915us
	01-06 03:32:37.042 D/MPEG4Writer( 4350): Audio mStartTimestampUs = 105915, moovStartTimeUs16892: //反复测试, moovStartTimeUs就是mStartTimestampUs，Video mStartTimestampUs

########################################################StagefrightRecorder########################################################
StagefrightRecorder，MPEG4Writer都定义了mStartTimeOffsetMs，目前看来StagefrightRecorder的没有作用。

mStartTimeOffsetMs: 是-1，没有用

	android.StagefrightRecorder.mStartTimeOffsetMs
	./include/media/stagefright/MPEG4Writer.h:126:    int32_t mStartTimeOffsetMs;
	
	StagefrightRecorder的mStartTimeOffsetMs在setupMPEG4Recording的时候没有意义。
	mStartTimeOffsetMs = mEncoderProfiles->getStartTimeOffsetMs(mCameraId);
	01-06 02:17:24.018 D/StagefrightRecorder( 2119): [setupMPEG4Recording], [1804], mStartTimeOffsetMs:-1


kKeyTime:
	这个值最终会用在MPEG4Writer.mStartTimeRealUs
	startMPEG4Recording()
		int64_t startTimeUs = systemTime() / 1000;
		sp<MetaData> meta = new MetaData;
		setupMPEG4MetaData(startTimeUs, totalBitRate, &meta);
				(*meta)->setInt64(kKeyTime, startTimeUs);

		01-06 02:45:44.173 D/StagefrightRecorder( 2662): [startMPEG4Recording] startTimeUs=1940908437
		01-06 02:45:44.176 D/MPEG4Writer( 2662): video Track  start,mStartTimeRealUs=1940908437
		01-06 02:45:44.176 I/MPEG4Writer( 2662): Start time offset: 700000 us
		01-06 02:45:44.648 D/MPEG4Writer( 2662): audio Track  start,mStartTimeRealUs=1940908437


########################################################MPEG4Writer########################################################
MPEG4Writer的mStartTimeOffsetMs没有直接访问，都是通过函数取得
./include/media/stagefright/MPEG4Writer.h:87:    int32_t getStartTimeOffsetMs() const { return mStartTimeOffsetMs; }

static const int64_t kInitialDelayTimeUs     = 700000LL//为什么是这个值？这个延迟可以保证threadEntry都准备好？那要看看读取第一帧数据threadEntry

startTimeUs:

	最终的时间会用到:
		meta->setInt64(kKeyTime, startTimeUs);
	status_t err = mSource->start(meta.get()); 

*********************************研究threadEntry中处理时间
mFirstSampleTimeRealUs：做调试用
        int64_t initialDelayUs =
            mFirstSampleTimeRealUs - mStartTimeRealUs - startTimeOffsetUs;

下面这段代码
		if (mStszTableEntries->count() == 0) {
            mFirstSampleTimeRealUs = systemTime() / 1000;
            mStartTimestampUs = timestampUs; //
            mOwner->setStartTimestampUs(mStartTimestampUs); //mStartTimestampUs是timestampUs
            previousPausedDurationUs = mStartTimestampUs;
#ifndef ANDROID_DEFAULT_CODE
			ALOGD("%s mStartTimestampUs=%lldus", mIsAudio?"Audio":"Video", mStartTimestampUs);
			if (!mIsAudio) {
				mOwner->setVideoStartTimeUs(mStartTimestampUs); //mVideoStartTimeUs是timestampUs
				mOwner->notify(MEDIA_RECORDER_EVENT_INFO, MEDIA_RECORDER_INFO_START_TIMER, 0); //以收到video第一个sample作为录像的开始
			}
#endif
        }
	