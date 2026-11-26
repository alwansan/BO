// Copyright 2026 BO-COOP contributors. SPDX-License-Identifier: MIT
// Runner-only actual Windows output graph. No game data or endpoint identities.
#include <windows.h>
#include <mmdeviceapi.h>
#include <xaudio2.h>
#include <cstdio>

struct Callback final : IXAudio2VoiceCallback {
    HANDLE complete=CreateEventW(nullptr,FALSE,FALSE,nullptr);
    ~Callback(){if(complete)CloseHandle(complete);}
    void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32) override {}
    void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override {}
    void STDMETHODCALLTYPE OnStreamEnd() override {}
    void STDMETHODCALLTYPE OnBufferStart(void*) override {}
    void STDMETHODCALLTYPE OnBufferEnd(void*) override {SetEvent(complete);}
    void STDMETHODCALLTYPE OnLoopEnd(void*) override {}
    void STDMETHODCALLTYPE OnVoiceError(void*,HRESULT) override {}
};

int main(){
    bool defaultEndpoint=false,graph=false,played=false;
    HRESULT apartment=CoInitializeEx(nullptr,COINIT_MULTITHREADED);
    if(SUCCEEDED(apartment)){
        IMMDeviceEnumerator* devices=nullptr;
        IMMDevice* device=nullptr;
        if(SUCCEEDED(CoCreateInstance(__uuidof(MMDeviceEnumerator),nullptr,CLSCTX_ALL,
                __uuidof(IMMDeviceEnumerator),reinterpret_cast<void**>(&devices)))){
            defaultEndpoint=SUCCEEDED(devices->GetDefaultAudioEndpoint(eRender,eConsole,&device));
            if(device)device->Release();
            devices->Release();
        }
        IXAudio2* engine=nullptr;
        IXAudio2MasteringVoice* master=nullptr;
        IXAudio2SourceVoice* source=nullptr;
        Callback callback;
        short silence[960]={};
        if(SUCCEEDED(XAudio2Create(&engine,0,XAUDIO2_DEFAULT_PROCESSOR))){
            if(SUCCEEDED(engine->CreateMasteringVoice(&master,2,48000,0,nullptr,nullptr,AudioCategory_Other))){
                WAVEFORMATEX format={WAVE_FORMAT_PCM,2,48000,192000,4,16,0};
                graph=SUCCEEDED(engine->CreateSourceVoice(&source,&format,0,XAUDIO2_DEFAULT_FREQ_RATIO,&callback));
                if(graph&&callback.complete){
                    XAUDIO2_BUFFER buffer={};
                    buffer.Flags=XAUDIO2_END_OF_STREAM;
                    buffer.AudioBytes=sizeof(silence);
                    buffer.pAudioData=reinterpret_cast<const BYTE*>(silence);
                    if(SUCCEEDED(source->SubmitSourceBuffer(&buffer))&&SUCCEEDED(source->Start()))
                        played=WaitForSingleObject(callback.complete,5000)==WAIT_OBJECT_0;
                }
            }
            if(source)source->DestroyVoice();
            if(master)master->DestroyVoice();
            engine->Release();
        }
        CoUninitialize();
    }
    std::printf("BO_RUNNER_AUDIO default_endpoint=%s graph=%s buffer_completed=%s\n",
        defaultEndpoint?"true":"false",graph?"true":"false",played?"true":"false");
    return defaultEndpoint&&graph&&played?0:1;
}
