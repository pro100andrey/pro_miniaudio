import 'package:equatable/equatable.dart';
import 'package:flutter/material.dart';
import 'package:pro_miniaudio/pro_miniaudio.dart';

import '../../generated/assets.gen.dart';

class WaveformSelectorVm extends Equatable {
  const WaveformSelectorVm({
    required this.type,
    required this.onChanged,
  });

  final WaveformType type;
  final ValueChanged<WaveformType> onChanged;

  @override
  List<Object?> get props => [type];
}

class WaveFromSelector extends StatelessWidget {
  const WaveFromSelector({
    required this.vm,
    super.key,
  });

  final WaveformSelectorVm vm;

  @override
  Widget build(BuildContext context) => LayoutBuilder(
        builder: (context, constraints) => Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Text(
              'Waveform',
              style: Theme.of(context).textTheme.titleSmall!.copyWith(
                    fontWeight: FontWeight.bold,
                    color: Colors.grey.shade700,
                  ),
            ),
            const SizedBox(height: 8),
            ToggleButtons(
              isSelected: WaveformType.values.map((e) => e == vm.type).toList(),
              onPressed: (index) => vm.onChanged(WaveformType.values[index]),
              children: WaveformType.values.map((e) => e.icon.svg()).toList(),
            ),
          ],
        ),
      );
}

extension WaveformTypeExt on WaveformType {
  SvgGenImage get icon {
    switch (this) {
      case WaveformType.sine:
        return Assets.icons.sin;
      case WaveformType.sawtooth:
        return Assets.icons.sawtooth;
      case WaveformType.square:
        return Assets.icons.square;
      case WaveformType.triangle:
        return Assets.icons.triangle;
    }
  }
}
