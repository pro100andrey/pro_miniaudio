import 'package:flutter/material.dart';

import '../inputs/frequency_input.dart';
import '../selectors/waveform_selector.dart';
import 'playback_item.dart';

class PlaybackDeviceItem extends StatelessWidget {
  const PlaybackDeviceItem({
    required this.name,
    required this.onClosePressed,
    required this.waveformSelector,
    required this.frequencyInput,
    required this.playback,
    super.key,
  });

  final String name;
  final VoidCallback onClosePressed;
  final WaveformSelectorVm waveformSelector;
  final FrequencyInputVm frequencyInput;
  final PlaybackItemVm playback;

  @override
  Widget build(BuildContext context) => Card(
        child: Padding(
          padding: const EdgeInsets.all(16),
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              Row(
                children: [
                  Expanded(
                    child: Text(
                      name,
                      overflow: TextOverflow.ellipsis,
                      style:
                          Theme.of(context).textTheme.headlineSmall!.copyWith(
                                color: Colors.grey.shade600,
                              ),
                    ),
                  ),
                  IconButton(
                    icon: const Icon(Icons.close),
                    onPressed: onClosePressed,
                  ),
                ],
              ),
              const SizedBox(height: 16),
              Wrap(
                spacing: 16,
                runSpacing: 16,
                crossAxisAlignment: WrapCrossAlignment.center,
                children: [
                  SingleChildScrollView(
                    scrollDirection: Axis.horizontal,
                    child: WaveFromSelector(vm: waveformSelector),
                  ),
                  ConstrainedBox(
                    constraints: const BoxConstraints(
                      maxWidth: 100,
                    ),
                    child: FrequencyInput(vm: frequencyInput),
                  ),
                ],
              ),
              const SizedBox(height: 16),
              PlaybackItem(vm: playback),
            ],
          ),
        ),
      );
}
