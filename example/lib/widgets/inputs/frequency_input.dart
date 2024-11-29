import 'package:equatable/equatable.dart';
import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

class FrequencyInputVm extends Equatable {
  const FrequencyInputVm({
    required this.frequency,
    required this.onChanged,
  });

  final double frequency;
  final ValueChanged<double> onChanged;

  @override
  List<Object?> get props => [frequency];
}

class FrequencyInput extends StatefulWidget {
  const FrequencyInput({
    required this.vm,
    super.key,
  });

  final FrequencyInputVm vm;

  @override
  State<FrequencyInput> createState() => _FrequencyInputState();
}

class _FrequencyInputState extends State<FrequencyInput> {
  late TextEditingController _controller;

  @override
  void initState() {
    final value = '${widget.vm.frequency}';

    _controller = TextEditingController(text: value);

    super.initState();
  }

  @override
  Widget build(BuildContext context) => Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Text(
            'Hz',
            style: Theme.of(context).textTheme.titleSmall!.copyWith(
                  fontWeight: FontWeight.bold,
                  color: Colors.grey.shade700,
                ),
          ),
          const SizedBox(height: 8),
          TextField(
            controller: _controller,
            inputFormatters: [
              FilteringTextInputFormatter.allow(RegExp(r'^\d+\.?\d{0,2}')),
            ],
            onChanged: (value) {
              final frequency = double.tryParse(value) ?? 0.0;

              widget.vm.onChanged(frequency);
            },
          ),
        ],
      );
}
