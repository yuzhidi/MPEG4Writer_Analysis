    virtual status_t stop() { return reset(); }
    (���ɣ����ڻ᲻���޸ĵĲ��֣�ÿ�����ֻ��һ�����ݣ����ڵ���)                                                                                            
    
while��ѭ���������У�
mStszTableEntries->count() ����ΪͬmDoneͬ�ȵ��ж�����
���е�mSxxxTableEntries�ṹ�У�ֻ��mStszTableEntries��->count()��Ϊ�����߼���������ʹ��

wihle�е�д��cache��ΪANDROID_DEFAULT_CODEΪ0��û��ִ�У��ѵ�˵ԭ������������߼�����
off64_t offset = mIsAvc? mOwner->addLengthPrefixedSample_l(copy)
                                 : mOwner->addSample_l(copy);

mChunkSamples.push_back(copy); addOneStscTableEntry; bufferChunk��ɺ�while�ͻص�ѭ�������жϣ�

while log��
1.�����ݵ�buffer
2.���buffer�еĵ�����
3.���Max file size
4.���sizeͨ�����򿽱�buffer��copy

##threadEntryֻ��һ�����飬bufferChunk, update moov boxs.##
 



##########################################################################################################################################################
Logic:
operation:
    
    +---------+                                                                                    +---------+                                                                                             +---------+
    | source  |                                                                                       cache                                                                                                   File
    +---------+                                                                                    +---------+                                                                                             +---------+    
        |                                                                                           /|\     |                                                                                                  /|\
        |    MediaBuffer *buffer;                                                                    |      |                                                                                                   |
        |    mSource->read(&buffer))    // while ѭ��������                                          |      |                                                                                                   |
       \|/                                                                                           |     \|/                                                                                                  |
                                                                    MediaBuffer *copy;                                                                                                                          |
                                                                                                                                                                                      
                                                                                                            
                                                                    mChunkSamples.push_back(copy); //mChunkSamples:ʵ������
                                                                                                                        
                                                                    MPEG4Writer::Track::bufferChunk(timestampUs); // ע��������track                                                                                
                                                                        Chunk chunk(this, timestampUs, mChunkSamples);    //ע��this :owner, mChunkSamples:ʵ������
                                                                                    ���캯������ֻ��ʱ��                                                                                               
                                                                        mOwner->bufferChunk(chunk);ʵ�ʵ���MPEG4Writer::bufferChunk(const Chunk& chunk)              
                                                                                                                                                                                                       
                                                                            mChunkInfos���owner(�ҵ���Ӧ��mTrack)
                                                                            ��Ӧ��track: it->mChunks.push_back(chunk); //���ݴ���mChunks                                                                      
                                                                            mBufferedDataSize += chunk.mDataSize;
                                                                            mChunkReadyCondition.signal();                                                                                             
                                                                            ������������ף�ȡ�����̱߳���samples��chunk                                
                                                              
                                                                        mChunkSamples.clear();    
                                                                                                                 findChunkToWrite(&chunk) 
                                                                                                                     ��mChunkInfos�е�ȡ��ChunkInfo�����ChunkInfo��mChunks�Ƿ�Ϊ���ա�
																													 �Ӳ�Ϊ�յ�mChunks���ҵ�ʱ��mTimeStampUs��С��һ��Chunk��track��
																													 
																													 ���ø�track��mChunkInfos���ҵ���Ӧ��ChunkInfo�������һ��chunk��Ϊ�������ء�
																														�Ӹ�ChunkInfo�������chunk.
																												 
                                                                                                                 mChunkReadyCondition.wait(mLock);
                                                                                                                     �ȴ�chunk��д������
                                                                                                                    Ȼ���chunk�е�����д��file cache
                                                                                                                 writeChunkToFile(&chunk);    
                                                                                                                         chunk->mTrack->isAvc()
                                                                                                                             ? addLengthPrefixedSample_l(MediaBuffer *buffer)
                                                                                                                             : addSample_l(MediaBuffer *buffer);

                                                                                                                                // ��¼ÿ�� writedchunk size, �ܹ�WritedChunk number
                                                                                                                                WritedChunk *writedchunk = new WritedChunk(chunk->mTrack, chunkSize);
                                                                                                                                mWritedChunks.push_back(writedchunk);
                                                                                                                                chunk->mTrack->addWritedChunkNum();
                                                                                                                             
                                                                                                                IF(Done)
                                                                                                                writeAllChunks();
                                                                                                                    writeChunkToFile(&chunk);                                

##########################################################################################################################################################
operator:
        
                    above is doing by threads                                                                                    above is doing by threads
            ++++++++++++++++++++++++++++++++++++++++++++++++++                                                        ++++++++++++++++++++++++++++++++++++++++++++++++++
            android.MPEG4Writer.Track.threadEntry() video source                                                          android.MPEG4Writer.threadFunc()  writer
            android.MPEG4Writer.Track.threadEntry() audio source                
            ++++++++++++++++++++++++++++++++++++++++++++++++++                                                        ++++++++++++++++++++++++++++++++++++++++++++++++++
                    threads generated by below                                                                                    threads generated by below
                        
                
                        
            android:MPEG4Writer::Track::start()                                                                                    android:MPEG4Writer::start()                                
                                                                                                        
            vEncSetForceIframe                                                                                                    startWriterThread()
                                
            pthread_create(&mThread, &attr, ThreadWrapper, this);                
                                
            static MPEG4Writer::Track::ThreadWrapper(void *me)                                                                    static  MPEG4Writer::ThreadWrapper(void *me)
                                            track->threadEntry();                                                                                            writer->threadFunc();


##########################################################################################################################################################
data struct:

    struct Chunk {
        Track               *mTrack;        // Owner ��֤��mSamples���ݶ�Ӧ��track
        int64_t             mTimeStampUs;   // Timestamp of the 1st sample ����
        List<MediaBuffer *> mSamples;       // Sample data
#ifndef ANDROID_DEFAULT_CODE 
        int64_t                mDataSize;
        
        
        