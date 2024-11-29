import 'package:equatable/equatable.dart';
import 'package:flutter/material.dart';

class DeviceActionsVm extends Equatable {
  const DeviceActionsVm({
    required this.createPlayback,
    required this.createCapture,
  });

  final VoidCallback createPlayback;
  final VoidCallback createCapture;

  @override
  List<Object?> get props => [];
}

class DeviceActions extends StatelessWidget {
  const DeviceActions({
    required this.vm,
    super.key,
  });

  final DeviceActionsVm vm;

  @override
  Widget build(BuildContext context) => PopupMenuButton(
        onSelected: (value) {
          switch (value) {
            case 0:
              vm.createPlayback();
            case 1:
              vm.createCapture();
            case _:
              throw Exception('Invalid value: $value');
          }
        },
        itemBuilder: (context) => [
          const PopupMenuItem(
            value: 0,
            child: Text('Create playback'),
          ),
          const PopupMenuItem(
            value: 1,
            child: Text('Create capture'),
          ),
        ],
      );
}
