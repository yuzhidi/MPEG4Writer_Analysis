    ALOGV("[%s], [%d], mStartTimeOffsetMs:%d",__FUNCTION__,__LINE__, mStartTimeOffsetMs);

	R:\mt6595_kk\out\target\product\ckt95_lw_kk\system\lib>adb logcat -v time -s StagefrightRecorder MPEG4Writer  | grep -E "mStartTimeOffsetMs|startTimeOffsetUs|Start time offset|startTimeUs|mStartTimeRealUs"
01-06 02:53:39.941 D/StagefrightRecorder( 4350): [setupMPEG4Recording], [1804], mStartTimeOffsetMs:-1
01-06 02:53:39.941 D/StagefrightRecorder( 4350): [startMPEG4Recording] startTimeUs=2416676724
01-06 02:53:39.943 D/MPEG4Writer( 4350): video Track  start,mStartTimeRealUs=2416676724
01-06 02:53:39.943 D/MPEG4Writer( 4350): [start], [2843], startTimeOffsetUs:-1000
01-06 02:53:39.943 D/MPEG4Writer( 4350): [start], [2848], startTimeOffsetUs:700000
01-06 02:53:39.943 D/MPEG4Writer( 4350): [start], [2857], startTimeUs:2416676724
01-06 02:53:39.943 I/MPEG4Writer( 4350): Start time offset: 700000 us
01-06 02:53:39.943 D/MPEG4Writer( 4350): [start], [2861], startTimeUs:2417376724
01-06 02:53:40.530 D/MPEG4Writer( 4350): audio Track  start,mStartTimeRealUs=2416676724
01-06 02:53:40.530 D/MPEG4Writer( 4350): [start], [2843], startTimeOffsetUs:-1000
01-06 02:53:40.530 D/MPEG4Writer( 4350): [start], [2848], startTimeOffsetUs:700000
01-06 02:53:40.530 D/MPEG4Writer( 4350): [start], [2857], startTimeUs:2416676724
01-06 02:53:40.530 I/MPEG4Writer( 4350): Start time offset: 700000 us
01-06 02:53:40.530 D/MPEG4Writer( 4350): [start], [2861], startTimeUs:2417376724    //���յ�startTimeUs������ԭ���Ļ����ϼ���70000

=================================================================StagefrightRecorder=========================================================================
StagefrightRecorder��MPEG4Writer��������mStartTimeOffsetMs��Ŀǰ����StagefrightRecorder��û�����á�

mStartTimeOffsetMs: ��-1��û����

	android.StagefrightRecorder.mStartTimeOffsetMs
	./include/media/stagefright/MPEG4Writer.h:126:    int32_t mStartTimeOffsetMs;
	
	StagefrightRecorder��mStartTimeOffsetMs��setupMPEG4Recording��ʱ��û�����塣
	mStartTimeOffsetMs = mEncoderProfiles->getStartTimeOffsetMs(mCameraId);
	01-06 02:17:24.018 D/StagefrightRecorder( 2119): [setupMPEG4Recording], [1804], mStartTimeOffsetMs:-1


kKeyTime:
	���ֵ���ջ�����MPEG4Writer.mStartTimeRealUs
	startMPEG4Recording()
		int64_t startTimeUs = systemTime() / 1000;
		sp<MetaData> meta = new MetaData;
		setupMPEG4MetaData(startTimeUs, totalBitRate, &meta);
				(*meta)->setInt64(kKeyTime, startTimeUs);

		01-06 02:45:44.173 D/StagefrightRecorder( 2662): [startMPEG4Recording] startTimeUs=1940908437
		01-06 02:45:44.176 D/MPEG4Writer( 2662): video Track  start,mStartTimeRealUs=1940908437
		01-06 02:45:44.176 I/MPEG4Writer( 2662): Start time offset: 700000 us
		01-06 02:45:44.648 D/MPEG4Writer( 2662): audio Track  start,mStartTimeRealUs=1940908437


=================================================================MPEG4Writer=========================================================================
MPEG4Writer��mStartTimeOffsetMsû��ֱ�ӷ��ʣ�����ͨ������ȡ��
./include/media/stagefright/MPEG4Writer.h:87:    int32_t getStartTimeOffsetMs() const { return mStartTimeOffsetMs; }

static const int64_t kInitialDelayTimeUs     = 700000LL//Ϊʲô�����ֵ������ӳٿ��Ա�֤threadEntry��׼���ã���Ҫ������ȡ��һ֡����threadEntry

startTimeUs:

	���յ�ʱ����õ�:
		meta->setInt64(kKeyTime, startTimeUs);
	status_t err = mSource->start(meta.get()); 