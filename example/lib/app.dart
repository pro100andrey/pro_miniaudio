import 'package:flutter/material.dart';

import 'pages/home.dart';

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) => MaterialApp(
        debugShowCheckedModeBanner: false,
        theme: buildTheme(),
        initialRoute: '/',
        routes: {
          '/': (context) => const HomePage(),
        },
      );
}

ThemeData _baseTheme = ThemeData.light();

ThemeData buildTheme() => _baseTheme.copyWith(
      inputDecorationTheme: const InputDecorationTheme(
        filled: true,
        border: OutlineInputBorder(),
        isDense: true,
      ),
    );
