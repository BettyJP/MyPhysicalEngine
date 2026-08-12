#define _CRT_SECURE_NO_WARNINGS
#include "PhysicsEngine/Utils/MP4Recorder.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <filesystem>

#if defined(_WIN32)
#define POPEN _popen
#define PCLOSE _pclose
#else
#define POPEN popen
#define PCLOSE pclose
#endif

namespace PhysicsEngine {

MP4Recorder::MP4Recorder()
    : recording(false), width(0), height(0), fps(60), frameCount(0), ffmpegPipe(nullptr), fallbackMode(false) {}

MP4Recorder::~MP4Recorder() {
    if (recording) {
        stopRecording();
    }
}

bool MP4Recorder::startRecording(const std::string& filename, int width, int height, int fps) {
    if (recording) {
        std::cout << "[MP4Recorder] Recording is already in progress." << std::endl;
        return false;
    }

    this->width = width;
    this->height = height;
    this->fps = fps;
    this->frameCount = 0;
    this->currentFilename = filename;
    this->fallbackMode = false;

    // Ensure parent directories exist
    std::filesystem::path outputPath(filename);
    if (outputPath.has_parent_path()) {
        std::filesystem::create_directories(outputPath.parent_path());
    }

    // Check if ffmpeg is available in PATH
    int checkResult = std::system("where ffmpeg >nul 2>&1");
    if (checkResult != 0) {
        // Retry with system ffmpeg check on windows
        checkResult = std::system("ffmpeg -version >nul 2>&1");
    }

    std::filesystem::path captureDir = outputPath.has_parent_path() ? (outputPath.parent_path() / "captures") : std::filesystem::path("captures");

    if (checkResult != 0) {
        std::cout << "\n========================================================\n";
        std::cout << " [MP4Recorder] NOTICE: FFmpeg is not installed / not in PATH.\n";
        std::cout << " MP4 encoding is unavailable until FFmpeg is installed.\n";
        std::cout << " To enable MP4 recording, run this command in PowerShell:\n";
        std::cout << "   winget install Gyan.FFmpeg\n";
        std::cout << " Switching to frame image sequence mode (" << captureDir.string() << "/)...\n";
        std::cout << "========================================================\n\n";

        fallbackMode = true;
        std::filesystem::create_directories(captureDir);
    } else {
        // Allocate pixel buffers for RGB (3 bytes per pixel)
        size_t bufferSize = static_cast<size_t>(width) * height * 3;
        pixelBuffer.resize(bufferSize);
        flippedBuffer.resize(bufferSize);

        // Build FFmpeg command line for Maximum Quality (Lossless)
        std::filesystem::path extPath(filename);
        std::string ext = extPath.extension().string();
        for (auto& c : ext) c = tolower(c);

        std::ostringstream cmd;
        cmd << "ffmpeg -y -f rawvideo -vcodec rawvideo -s " << width << "x" << height
            << " -pix_fmt rgb24 -r " << fps << " -i - ";

        if (ext == ".mov") {
            // Apple ProRes 422 HQ (Professional Video Production Quality)
            cmd << "-c:v prores_ks -profile:v 3 -pix_fmt yuv422p10le \"" << filename << "\"";
            std::cout << "[MP4Recorder] Encoding Mode: Apple ProRes 422 HQ (Maximum Quality)" << std::endl;
        } else if (ext == ".mkv") {
            // Lossless H.264 YUV444p
            cmd << "-c:v libx264 -preset medium -crf 0 -pix_fmt yuv444p \"" << filename << "\"";
            std::cout << "[MP4Recorder] Encoding Mode: Lossless H.264 4:4:4 (Maximum Quality)" << std::endl;
        } else {
            // Lossless H.264 MP4 (CRF 0, maximum compatibility & crystal clear visual quality)
            cmd << "-c:v libx264 -preset medium -crf 0 -pix_fmt yuv420p \"" << filename << "\"";
            std::cout << "[MP4Recorder] Encoding Mode: Lossless H.264 MP4 (CRF 0 / Maximum Quality)" << std::endl;
        }

        std::cout << "[MP4Recorder] Launching FFmpeg pipe..." << std::endl;
        ffmpegPipe = POPEN(cmd.str().c_str(), "wb");

        if (!ffmpegPipe) {
            std::cerr << "[MP4Recorder] ERROR: Failed to open FFmpeg pipe!" << std::endl;
            fallbackMode = true;
            std::filesystem::create_directories("captures");
        } else {
            std::cout << "[MP4Recorder] Started MP4 recording to: " << filename << std::endl;
        }
    }

    recording = true;
    return true;
}

void MP4Recorder::captureFrame() {
    if (!recording) return;

    // Read OpenGL pixels (ensure byte alignment)
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixelBuffer.data());

    // Flip Y axis (OpenGL bottom-left origin -> video top-left origin)
    size_t rowStride = static_cast<size_t>(width) * 3;
    for (int y = 0; y < height; ++y) {
        const unsigned char* srcRow = pixelBuffer.data() + (height - 1 - y) * rowStride;
        unsigned char* dstRow = flippedBuffer.data() + y * rowStride;
        std::memcpy(dstRow, srcRow, rowStride);
    }

    if (!fallbackMode && ffmpegPipe) {
        // Write raw RGB frame to FFmpeg stdin pipe
        size_t bytesWritten = std::fwrite(flippedBuffer.data(), 1, flippedBuffer.size(), ffmpegPipe);
        if (bytesWritten != flippedBuffer.size()) {
            std::cerr << "[MP4Recorder] Warning: Failed to write full frame buffer to FFmpeg pipe." << std::endl;
        }
    } else if (fallbackMode) {
        // Fallback: Write PPM frame file
        std::filesystem::path outputPath(currentFilename);
        std::filesystem::path captureDir = outputPath.has_parent_path() ? (outputPath.parent_path() / "captures") : std::filesystem::path("captures");
        
        std::ostringstream frameFilename;
        frameFilename << captureDir.string() << "/frame_" << std::setfill('0') << std::setw(5) << frameCount << ".ppm";
        FILE* fp = std::fopen(frameFilename.str().c_str(), "wb");
        if (fp) {
            std::fprintf(fp, "P6\n%d %d\n255\n", width, height);
            std::fwrite(flippedBuffer.data(), 1, flippedBuffer.size(), fp);
            std::fclose(fp);
        }
    }

    frameCount++;
}

void MP4Recorder::stopRecording() {
    if (!recording) return;

    if (!fallbackMode && ffmpegPipe) {
        std::fflush(ffmpegPipe);
        int exitCode = PCLOSE(ffmpegPipe);
        ffmpegPipe = nullptr;
        std::cout << "[MP4Recorder] Recording stopped. Video saved to: " << currentFilename
                  << " (" << frameCount << " frames, exit code: " << exitCode << ")" << std::endl;
    } else if (fallbackMode) {
        std::cout << "[MP4Recorder] Frame sequence capture stopped. (" << frameCount << " frames saved in captures/)" << std::endl;
    }

    recording = false;
}

} // namespace PhysicsEngine
