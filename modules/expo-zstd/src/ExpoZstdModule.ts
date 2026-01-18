import { NativeModule, requireNativeModule } from 'expo';

import { ExpoZstdModuleEvents } from './ExpoZstd.types';

declare class ExpoZstdModule extends NativeModule<ExpoZstdModuleEvents> {
  PI: number;
  hello(): string;
  setValueAsync(value: string): Promise<void>;
}

// This call loads the native module object from the JSI.
export default requireNativeModule<ExpoZstdModule>('ExpoZstd');
