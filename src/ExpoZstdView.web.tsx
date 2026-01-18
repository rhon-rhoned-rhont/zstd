import * as React from 'react';

import { ExpoZstdViewProps } from './ExpoZstd.types';

export default function ExpoZstdView(props: ExpoZstdViewProps) {
  return (
    <div>
      <iframe
        style={{ flex: 1 }}
        src={props.url}
        onLoad={() => props.onLoad({ nativeEvent: { url: props.url } })}
      />
    </div>
  );
}
