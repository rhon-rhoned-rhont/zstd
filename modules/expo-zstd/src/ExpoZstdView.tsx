import { requireNativeView } from 'expo';
import * as React from 'react';

import { ExpoZstdViewProps } from './ExpoZstd.types';

const NativeView: React.ComponentType<ExpoZstdViewProps> =
  requireNativeView('ExpoZstd');

export default function ExpoZstdView(props: ExpoZstdViewProps) {
  return <NativeView {...props} />;
}
