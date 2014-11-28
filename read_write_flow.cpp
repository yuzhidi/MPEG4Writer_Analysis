    virtual status_t stop() { return reset(); }
    (技巧，对于会不断修改的部分，每行最好只有一个内容，便于调整)                                                                                            
    
while的循环条件还有：
mStszTableEntries->count() 是作为同mDone同等的判断条件
所有的mSxxxTableEntries结构中，只有mStszTableEntries的->count()作为条件逻辑控制条件使用

wihle中的写入cache因为ANDROID_DEFAULT_CODE为0而没有执行，难道说原生代码是这个逻辑？？
off64_t offset = mIsAvc? mOwner->addLengthPrefixedSample_l(copy)
                                 : mOwner->addSample_l(copy);

mChunkSamples.push_back(copy); addOneStscTableEntry; bufferChunk完成后，while就回到循环条件判断；

while log：
1.读数据到buffer
2.检查buffer中的的数据
3.检查Max file size
4.检查size通过，则拷贝buffer到copy

##threadEntry只做一件事情，bufferChunk, update moov boxs.##
 



##########################################################################################################################################################
Logic:
operation:
    
    +---------+                                                                                    +---------+                                                                                             +---------+
    | source  |                                                                                       cache                                                                                                   File
    +---------+                                                                                    +---------+                                                                                             +---------+    
        |                                                                                           /|\     |                                                                                                  /|\
        |    MediaBuffer *buffer;                                                                    |      |                                                                                                   |
        |    mSource->read(&buffer))    // while 循环的条件                                          |      |                                                                                                   |
       \|/                                                                                           |     \|/                                                                                                  |
                                                                    MediaBuffer *copy;                                                                                                                          |
                                                                                                                                                                                      
                                                                                                            
                                                                    mChunkSamples.push_back(copy); //mChunkSamples:实际数据
                                                                                                                        
                                                                    MPEG4Writer::Track::bufferChunk(timestampUs); // 注意这里是track                                                                                
                                                                        Chunk chunk(this, timestampUs, mChunkSamples);    //注意this :owner, mChunkSamples:实际数据
                                                                                    构造函数参数只有时间                                                                                               
                                                                        mOwner->bufferChunk(chunk);实际调用MPEG4Writer::bufferChunk(const Chunk& chunk)              
                                                                                                                                                                                                       
                                                                            mChunkInfos检查owner(找到对应的mTrack)
                                                                            对应的track: it->mChunks.push_back(chunk); //数据存入mChunks                                                                      
                                                                            mBufferedDataSize += chunk.mDataSize;
                                                                            mChunkReadyCondition.signal();                                                                                             
                                                                            到这里可以明白，取数据线程保存samples到chunk                                
                                                              
                                                                        mChunkSamples.clear();    
                                                                                                                 findChunkToWrite(&chunk) 
                                                                                                                     从mChunkInfos中的取出ChunkInfo，检查ChunkInfo的mChunks是否为不空。
																													 从不为空的mChunks中找到时间mTimeStampUs最小的一个Chunk的track。
																													 
																													 再用该track到mChunkInfos中找到对应的ChunkInfo，将其第一个chunk作为参数返回。
																														从该ChunkInfo中清除该chunk.
																												 
                                                                                                                 mChunkReadyCondition.wait(mLock);
                                                                                                                     等待chunk中写入数据
                                                                                                                    然后把chunk中的数据写入file cache
                                                                                                                 writeChunkToFile(&chunk);    
                                                                                                                         chunk->mTrack->isAvc()
                                                                                                                             ? addLengthPrefixedSample_l(MediaBuffer *buffer)
                                                                                                                             : addSample_l(MediaBuffer *buffer);

                                                                                                                                // 记录每个 writedchunk size, 总共WritedChunk number
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
        Track               *mTrack;        // Owner 保证了mSamples数据对应的track
        int64_t             mTimeStampUs;   // Timestamp of the 1st sample 所在
        List<MediaBuffer *> mSamples;       // Sample data
#ifndef ANDROID_DEFAULT_CODE 
        int64_t                mDataSize;
        
        
        