import 'package:flutter/material.dart';
import 'package:path_provider/path_provider.dart';
import 'package:pro_miniaudio/pro_miniaudio.dart';

import 'app.dart';

Future<void> main() async {
  WidgetsFlutterBinding.ensureInitialized();

  final temp = await getTemporaryDirectory();
  final path = '${temp.path}/log.txt';
  FileLogger.instance.enable(path, level: FileLogLevel.debug);
  print('Log path: $path');

  runApp(const MyApp());
}
