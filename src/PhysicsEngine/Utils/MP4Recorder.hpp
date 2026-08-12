#pragma once
#include <string>
#include <vector>
#include <cstdio>
#include <glad/glad.h>

namespace PhysicsEngine {

class MP4Recorder {
public:
    MP4Recorder();
    ~MP4Recorder();

    bool startRecording(const std::string& filename, int width, int height, int fps = 60);
    void captureFrame();
    void stopRecording();

    bool isRecording() const { return recording; }
    int getRecordedFrames() const { return frameCount; }
    const std::string& getCurrentFilename() const { return currentFilename; }

private:
    bool recording;
    int width;
    int height;
    int fps;
    int frameCount;
    std::string currentFilename;
    FILE* ffmpegPipe;
    std::vector<unsigned char> pixelBuffer;
    std::vector<unsigned char> flippedBuffer;
    bool fallbackMode;
};

} // namespace PhysicsEngine
