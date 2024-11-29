import 'package:equatable/equatable.dart';
import 'package:flutter/material.dart';

final class PlaybackItemVm extends Equatable {
  const PlaybackItemVm({
    required this.isPlaying,
    required this.onPlayPressed,
    required this.onStopPressed,
  });

  final bool isPlaying;
  final VoidCallback onPlayPressed;
  final VoidCallback onStopPressed;

  @override
  List<Object?> get props => [isPlaying];
}

class PlaybackItem extends StatelessWidget {
  const PlaybackItem({
    required this.vm,
    super.key,
  });

  final PlaybackItemVm vm;

  @override
  Widget build(BuildContext context) => Row(
        children: [
          IconButton(
            icon: Icon(vm.isPlaying ? Icons.stop : Icons.play_arrow),
            onPressed: () {
              vm.isPlaying ? vm.onStopPressed() : vm.onPlayPressed();
            },
          ),
        ],
      );
}
