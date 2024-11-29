import 'package:collection/collection.dart';
import 'package:flutter/material.dart';
import 'package:pro_miniaudio/pro_miniaudio.dart';

class FormatsInfoDialog extends StatelessWidget {
  const FormatsInfoDialog({
    required this.title,
    required this.formats,
    super.key,
  });

  final String title;
  final List<SupportedFormat> formats;

  @override
  Widget build(BuildContext context) => AlertDialog(
        insetPadding: const EdgeInsets.all(2),
        title: Text(title),
        content: SingleChildScrollView(
          child: SingleChildScrollView(
            scrollDirection: Axis.horizontal,
            child: DataTable(
              headingRowHeight: 28,
              dataRowMaxHeight: 26,
              dataRowMinHeight: 20,
              columns: const [
                DataColumn(label: Text('Index')),
                DataColumn(label: Text('Format')),
                DataColumn(label: Text('Channels')),
                DataColumn(label: Text('Sample Rate')),
              ],
              rows: formats
                  .mapIndexed(
                    (index, format) => DataRow(
                      cells: [
                        DataCell(Text('$index')),
                        DataCell(Text(format.format.name)),
                        DataCell(Text('${format.channels}')),
                        DataCell(Text('${format.sampleRate}')),
                      ],
                    ),
                  )
                  .toList(),
            ),
          ),
        ),
        actions: [
          TextButton(
            onPressed: () => Navigator.of(context).pop(),
            child: const Text('Close'),
          ),
        ],
      );
}
