import { registerWebModule, NativeModule } from 'expo';

import { ExpoZstdModuleEvents } from './ExpoZstd.types';

class ExpoZstdModule extends NativeModule<ExpoZstdModuleEvents> {
  PI = Math.PI;
  async setValueAsync(value: string): Promise<void> {
    this.emit('onChange', { value });
  }
  hello() {
    return 'Hello world! 👋';
  }
}

export default registerWebModule(ExpoZstdModule, 'ExpoZstdModule');
