// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
package com.racas.black.coop;
/** Source-matched original game-thread HOST only: queue TLS PREPARE, not READY.
 * This class has no game-engine, network or Android dependencies.
 */
final class RoomOriginalHostStagePrepareGate {
    private static final long FRESH_NS=60000000000L;
    private int sourceEpisode,level=-1,stage=-1,preparedEpisode,nextEpoch;
    private long observedAtNs,preparedRevision;
    void reset(){sourceEpisode=preparedEpisode=nextEpoch=0;level=stage=-1;
        observedAtNs=preparedRevision=0;}
    boolean observe(int episode,int level,int stage,long nowNs){
        if(episode<=0||level<0||level>8||stage<0||stage>15||nowNs<=0||
           (sourceEpisode>0&&episode<sourceEpisode))return false;
        if(episode==sourceEpisode){
            if(level!=this.level||stage!=this.stage)return false;
            observedAtNs=nowNs;return false;
        }
        sourceEpisode=episode;this.level=level;this.stage=stage;
        observedAtNs=nowNs;return true;
    }
    int nextOffer(long revision,int humans,boolean hasTicket,long nowNs){
        if(sourceEpisode<=0||level<0||level>7||stage<0||stage>15||
           revision<=0||humans<1||humans>3||hasTicket||
           observedAtNs<=0||nowNs<observedAtNs||
           nowNs-observedAtNs>FRESH_NS||nextEpoch==Integer.MAX_VALUE||
           (preparedEpisode==sourceEpisode&&preparedRevision==revision))return 0;
        return nextEpoch+1; // NOT original local episode; HOST story epoch.
    }
    boolean markOffered(long revision,int epoch){
        if(revision<=0||sourceEpisode<=0||nextEpoch==Integer.MAX_VALUE||
           epoch<=0||epoch!=nextEpoch+1)return false;
        preparedEpisode=sourceEpisode;preparedRevision=revision;nextEpoch=epoch;
        return true;
    }
    // A HOST pressing START in a room of exactly 2 humans may create an
    // authenticated stage-0 PREPARE before the real original level begins.
    // Does NOT launch the original game or imply any scene was loaded.
    int nextTwoPhoneFirstStageEpoch(long revision,int remoteHumans,
                                    boolean hasTicket){
        if(revision<=0||remoteHumans!=1||hasTicket||
           nextEpoch==Integer.MAX_VALUE)return 0;
        return nextEpoch+1;
    }
    boolean markTwoPhoneFirstStageOffered(long revision,int epoch){
        if(revision<=0||epoch<=0||epoch!=nextEpoch+1)return false;
        nextEpoch=epoch;
        preparedRevision=revision;
        // Same original episode cannot be implicitly re-offered at a
        // different stage by a subsequent normal original source tick.
        preparedEpisode=sourceEpisode;
        return true;
    }
    int level(){return level;}
    int stage(){return stage;}
}