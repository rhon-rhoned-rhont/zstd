import { requireNativeModule } from "expo-modules-core";

const ExpoZstd = requireNativeModule("ExpoZstd");

export async function decompressToFile(
  input: Uint8Array,
  outPath: string,
): Promise<boolean> {
  return ExpoZstd.decompressToFile(input, outPath);
}

export async function decompressFileToFile(
  inputPath: string,
  outPath: string,
): Promise<boolean> {
  return ExpoZstd.decompressFileToFile(inputPath, outPath);
}
