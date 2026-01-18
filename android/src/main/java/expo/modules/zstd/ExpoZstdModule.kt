package expo.modules.zstd

import expo.modules.kotlin.modules.Module
import expo.modules.kotlin.modules.ModuleDefinition

class ExpoZstdModule : Module() {

  companion object {
    init {
      System.loadLibrary("expozstd")
    }
  }

  // JNI methods implemented in expozstd_jni.cpp
  private external fun nativeDecompressToFile(input: ByteArray, outPath: String): Boolean
  private external fun nativeDecompressFileToFile(inputPath: String, outPath: String): Boolean

  override fun definition() = ModuleDefinition {
    Name("ExpoZstd")

    Function("decompressToFile") { input: ByteArray, outPath: String ->
      nativeDecompressToFile(input, outPath)
    }

    Function("decompressFileToFile") { inputPath: String, outPath: String ->
      nativeDecompressFileToFile(inputPath, outPath)
    }
  }
}
