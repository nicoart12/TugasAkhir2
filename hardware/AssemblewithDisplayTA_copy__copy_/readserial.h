// #include "var.h"

// double readDoubleFromSerial()
// {
//   String input = "";
//   while (true)
//   {
//     if (ESP_BT.available() > 0)
//     {
//       char c = ESP_BT.read();
//       if (c == '\n')
//       {
//         return input.toDouble();
//       }
//       else
//       {
//         input += c;
//       }
//     }
//   }
// }

// int readIntFromSerial()
// {
//   String input = "";
//   while (true)
//   {
//     if (ESP_BT.available() > 0)
//     {
//       char c = ESP_BT.read();
//       if (c == '\n')
//       {
//         return input.toInt();
//       }
//       else
//       {
//         input += c;
//       }
//     }
//   }
// }

// char readSingleCharFromSerial()
// {
//   while (true)
//   {
//     if (ESP_BT.available() > 0)
//     {
//       return ESP_BT.read();
//     }
//   }
// }