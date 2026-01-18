#include <jni.h>
#include <string>
#include <android/log.h>
#include <stdio.h>

extern "C" {
#include "zstd.h"
}

#define LOG_TAG "ExpoZstd"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__)

static bool writeAll(FILE *fp, const void *data, size_t size) {
  if (!fp || !data) return false;
  const uint8_t *ptr = static_cast<const uint8_t *>(data);
  size_t totalWritten = 0;
  while (totalWritten < size) {
    size_t written = fwrite(ptr + totalWritten, 1, size - totalWritten, fp);
    if (written == 0) return false;
    totalWritten += written;
  }
  return true;
}

static bool decompressFileToFile(const char *inputPath, const char *outputPath) {
  if (!inputPath || !outputPath) return false;

  FILE *in = fopen(inputPath, "rb");
  if (!in) {
    LOGE("Failed opening input file: %s", inputPath);
    return false;
  }

  FILE *out = fopen(outputPath, "wb");
  if (!out) {
    LOGE("Failed opening output file: %s", outputPath);
    fclose(in);
    return false;
  }

  ZSTD_DStream *dstream = ZSTD_createDStream();
  if (!dstream) {
    LOGE("Failed to create ZSTD_DStream");
    fclose(in);
    fclose(out);
    return false;
  }

  size_t initResult = ZSTD_initDStream(dstream);
  if (ZSTD_isError(initResult)) {
    LOGE("ZSTD_initDStream error: %s", ZSTD_getErrorName(initResult));
    ZSTD_freeDStream(dstream);
    fclose(in);
    fclose(out);
    return false;
  }

  const size_t inChunkSize = ZSTD_DStreamInSize();
  const size_t outChunkSize = ZSTD_DStreamOutSize();

  void *inChunk = malloc(inChunkSize);
  void *outChunk = malloc(outChunkSize);
  if (!inChunk || !outChunk) {
    LOGE("malloc failed for stream buffers");
    free(inChunk);
    free(outChunk);
    ZSTD_freeDStream(dstream);
    fclose(in);
    fclose(out);
    return false;
  }

  bool ok = true;
  while (ok) {
    size_t readBytes = fread(inChunk, 1, inChunkSize, in);
    if (readBytes == 0) {
      break;
    }

    ZSTD_inBuffer inBuf = { inChunk, readBytes, 0 };
    while (inBuf.pos < inBuf.size) {
      ZSTD_outBuffer outBuf = { outChunk, outChunkSize, 0 };
      size_t res = ZSTD_decompressStream(dstream, &outBuf, &inBuf);
      if (ZSTD_isError(res)) {
        LOGE("ZSTD_decompressStream error: %s", ZSTD_getErrorName(res));
        ok = false;
        break;
      }

      if (outBuf.pos > 0) {
        if (!writeAll(out, outChunk, outBuf.pos)) {
          LOGE("Failed writing decompressed output to: %s", outputPath);
          ok = false;
          break;
        }
      }
    }
  }

  free(inChunk);
  free(outChunk);
  ZSTD_freeDStream(dstream);
  fclose(in);
  fclose(out);

  return ok;
}

/**
 * Native method:
 * boolean nativeDecompressToFile(byte[] input, String outPath)
 */
extern "C"
JNIEXPORT jboolean JNICALL
Java_expo_modules_zstd_ExpoZstdModule_nativeDecompressToFile(
    JNIEnv *env,
    jobject /* thiz */,
    jbyteArray input,
    jstring outPath
) {
  if (input == nullptr || outPath == nullptr) return JNI_FALSE;

  const jsize inputSize = env->GetArrayLength(input);
  if (inputSize <= 0) return JNI_FALSE;

  jbyte *inputBytes = env->GetByteArrayElements(input, nullptr);
  const char *outPathC = env->GetStringUTFChars(outPath, nullptr);

  FILE *fp = fopen(outPathC, "wb");
  if (!fp) {
    LOGE("Failed opening output file: %s", outPathC);
    env->ReleaseByteArrayElements(input, inputBytes, JNI_ABORT);
    env->ReleaseStringUTFChars(outPath, outPathC);
    return JNI_FALSE;
  }

  ZSTD_DStream *dstream = ZSTD_createDStream();
  if (!dstream) {
    LOGE("Failed to create ZSTD_DStream");
    fclose(fp);
    env->ReleaseByteArrayElements(input, inputBytes, JNI_ABORT);
    env->ReleaseStringUTFChars(outPath, outPathC);
    return JNI_FALSE;
  }

  size_t initResult = ZSTD_initDStream(dstream);
  if (ZSTD_isError(initResult)) {
    LOGE("ZSTD_initDStream error: %s", ZSTD_getErrorName(initResult));
    ZSTD_freeDStream(dstream);
    fclose(fp);
    env->ReleaseByteArrayElements(input, inputBytes, JNI_ABORT);
    env->ReleaseStringUTFChars(outPath, outPathC);
    return JNI_FALSE;
  }

  ZSTD_inBuffer inBuf = { inputBytes, (size_t)inputSize, 0 };
  const size_t outChunkSize = ZSTD_DStreamOutSize();
  void *outChunk = malloc(outChunkSize);
  if (!outChunk) {
    LOGE("malloc failed for output chunk (%zu bytes)", outChunkSize);
    ZSTD_freeDStream(dstream);
    fclose(fp);
    env->ReleaseByteArrayElements(input, inputBytes, JNI_ABORT);
    env->ReleaseStringUTFChars(outPath, outPathC);
    return JNI_FALSE;
  }

  bool ok = true;
  while (inBuf.pos < inBuf.size) {
    ZSTD_outBuffer outBuf = { outChunk, outChunkSize, 0 };
    size_t res = ZSTD_decompressStream(dstream, &outBuf, &inBuf);
    if (ZSTD_isError(res)) {
      LOGE("ZSTD_decompressStream error: %s", ZSTD_getErrorName(res));
      ok = false;
      break;
    }

    if (outBuf.pos > 0) {
      if (!writeAll(fp, outChunk, outBuf.pos)) {
        LOGE("Failed writing decompressed output to: %s", outPathC);
        ok = false;
        break;
      }
    }
  }

  free(outChunk);
  ZSTD_freeDStream(dstream);
  fclose(fp);
  env->ReleaseByteArrayElements(input, inputBytes, JNI_ABORT);
  env->ReleaseStringUTFChars(outPath, outPathC);

  return ok ? JNI_TRUE : JNI_FALSE;
}

/**
 * Native method:
 * boolean nativeDecompressFileToFile(String inputPath, String outPath)
 */
extern "C"
JNIEXPORT jboolean JNICALL
Java_expo_modules_zstd_ExpoZstdModule_nativeDecompressFileToFile(
    JNIEnv *env,
    jobject /* thiz */,
    jstring inputPath,
    jstring outPath
) {
  if (inputPath == nullptr || outPath == nullptr) return JNI_FALSE;

  const char *inputPathC = env->GetStringUTFChars(inputPath, nullptr);
  const char *outPathC = env->GetStringUTFChars(outPath, nullptr);

  bool ok = decompressFileToFile(inputPathC, outPathC);

  env->ReleaseStringUTFChars(inputPath, inputPathC);
  env->ReleaseStringUTFChars(outPath, outPathC);

  return ok ? JNI_TRUE : JNI_FALSE;
}
